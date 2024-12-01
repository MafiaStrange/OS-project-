#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <string.h>
#include <libgen.h>

static int (*real_open)(const char *pathname, int flags, mode_t mode) = NULL;
static ssize_t (*real_write)(int fd, const void* buf, size_t count) = NULL;

char* get_file_path_from_fd(int fd){
    // construct the path to /proc/self/fd/<fd>
    char fd_path[256];
    snprintf(fd_path,sizeof(fd_path),"/proc/self/fd/%d",fd);

    // Allocate buffer for the resolved path
    char* file_path = malloc(256 * sizeof(char));
    if (file_path == NULL){
        fprintf(stderr,"Memory allocation failed");
        return NULL;
    }

    // use readlink to resolve the path
    ssize_t len = readlink(fd_path, file_path, 256);
    if(len == -1){         
        perror("readlink");
        free(file_path);
        return NULL;
    }

    file_path[len] = '\0';

    // extracting the file name
    char* file_name = strdup(basename(file_path)); // duplicating to avoid changing the original path
    if (file_name == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        free (file_path);
        return NULL;
    }

    free(file_path);
    return file_name;
}

// to get the file attribute
int get_attribute(const char *pathname){
    char value[10];
    ssize_t len = getxattr(pathname,"user.password_protect",value,sizeof(value));
    if(len == -1){
        return 0; // return 0 if the attribute doesn't exist
    }
    value[len] = '\0';
    return atoi(value);
}

int get_hidden_attribute(const char *pathname){
    char value[10];
    ssize_t len = getxattr(pathname,"user.hidden",value,sizeof(value));
    if(len == -1){
        return 0; // return 0 if the attribute doesn't exist
    }
    value[len] = '\0';
    return atoi(value);
}

char* new_path(const char *pathname){
    char* file_name = malloc(256 * sizeof(char));
    if(file_name == NULL){
        fprintf(stderr,"Memory allocation failed\n");
        exit(1);
    }

    ssize_t len = getxattr(pathname,"user.hidden_file",file_name,256);
    if(len == -1){
        fprintf(stderr,"Encrypted file name missing for the file %s\n",pathname);
        free(file_name);
        exit(1);
    }
    file_name[len] = '\0';
    return file_name;
}

// Our wrapper for the open() system call
int open(const char *pathname, int flags, ...) {
    // Initialize the real open() function (if it's not already done)
    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
        if (!real_open) {
            fprintf(stderr, "Error: %s\n", dlerror());
            return -1;  // Return an error if dlsym failed
        }
    }


    // If it is a temporary file... do normal open() syscall
    const char *base_name = strrchr(pathname, '/');
    base_name = base_name ? base_name + 1 : pathname; // Get the file name from path

    if (strstr(base_name, ".swp") != NULL || strstr(base_name, ".tmp") != NULL) {
        // Temporary file, call the real open() directly
        va_list args;
        va_start(args, flags);

        mode_t mode = 0;
        if (flags & O_CREAT) {
            mode = va_arg(args, mode_t);
        }

        va_end(args);
        return real_open(pathname, flags, mode);
    }

    // Handle variadic arguments only if necessary
    va_list args;
    va_start(args, flags);

    mode_t mode = 0;  // Initialize mode to a default value
    if (flags & O_CREAT) {
        mode = va_arg(args, mode_t);  // Extract mode argument if the O_CREAT flag is set
    }

    va_end(args);

    // check if it is a hidden flag
    // int hidden_flag = get_hidden_attribute(pathname);
    // if (hidden_flag == 1){
    //     // print the message and deny opening the file
    //     fprintf(stderr,"Cannot open the file. It is password-protected :%s\n", pathname);
    //     return -1; // denying the open() syscall
    // }

    int fd = -1; // the file descriptor

    // if the file is not created, check for the metadata bit
    if(!(flags & O_CREAT)){
        int protect_flag = get_attribute(pathname);

        // if the metadata bit is 1, ask for password
        if(protect_flag == 1){
            char command[256];
            snprintf(command, sizeof(command), "python3 password_verification.py '%s'", pathname);
            int ret = system(command);

            if(ret != 0){
                fprintf(stderr,"Access denied: Incorrect password for %s\n",pathname);
                return -1;
            }

            char* new_pathname = new_path(pathname);
            fd = real_open(new_pathname,flags,mode);
            free(new_pathname);
            return fd;
        }
    }

    // Call the real open() system call to actually create the file
    if(fd == -1){
        fd = real_open(pathname, flags, mode);
    }

    // Check if the file is successfully created
    if (fd != -1 && (flags & O_CREAT)) {
        // Call the Python script to prompt for password protection
        char command[256];
        snprintf(command, sizeof(command), "python3 file_encryption_handler.py '%s'", pathname);
        system(command);  // Execute the Python script with the filename as an argument
    }

    return fd;
}

ssize_t write(int fd, const void* buf, size_t count){
    if (!real_write){
        real_write = dlsym(RTLD_NEXT, "write");
    }

    // check if the file descriptor corresponds to a password-protected file
    char* pathname = get_file_path_from_fd(fd);
    if (!pathname){
        return real_write(fd, buf, count);
    }

    // Check for temporary files
    const char *base_name = strrchr(pathname, '/');
    base_name = base_name ? base_name + 1 : pathname;

    if (base_name[0] == '.' || strstr(base_name, ".swp") != NULL || strstr(base_name,".tmp") != NULL) {
        free(pathname);
        return real_write(fd, buf, count); // Skip handling temporary files
    }

    int protect_flag = get_attribute(pathname);
    if (protect_flag == 1){
        // Handle the write for a password-protected file
        // Ensure the data is written to the hidden file
        char* hidden_path = new_path(pathname);

        // Delete the old encrypted file
        if(access(hidden_path,F_OK) == 0){ // check if the file exists 
            if(remove(hidden_path) != 0){
                fprintf(stderr, "Failed to deleted old hidden file %s\n",hidden_path);
                free(hidden_path);
                free(pathname);
                return -1;
            }
        }

        int hidden_fd = open(hidden_path, O_WRONLY); // open the hidden file in write only mode
        free(hidden_path);
        free(pathname);

        if(hidden_fd < 0){
            fprintf(stderr, "Error opening hidden file for write.\n");
            return -1;
        }

        ssize_t result = real_write(hidden_fd, buf, count);
        close(hidden_fd);
        return result;
    }

    // perform the write operation
    free(pathname);
    return real_write(fd, buf, count);
}
