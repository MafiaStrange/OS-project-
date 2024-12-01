// SPDX-License-Identifier: BSD-3-Clause
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

char LICENSE[] SEC("license") = "Dual BSD/GPL";

#define enc_dir "/home/vboxuser/encrypted/"

#define enc_size 25
#define file_size 33

struct array{
	char name[file_size];
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 8192);
    __type(key, unsigned int);
    __type(value, struct array);
} map_files SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 8192);
    __type(key, unsigned int);
    __type(value, unsigned int);
} map_fds SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 8192);
    __type(key, struct array);
    __type(value, unsigned int);
} hashed_files SEC(".maps");

// Add (1 -> filename) on map_files if interested in file

SEC("tracepoint/syscalls/sys_enter_openat")
int handle_openat_enter(struct trace_event_raw_sys_enter *ctx)
{
    size_t pid_tgid = bpf_get_current_pid_tgid();
    int pid = pid_tgid >> 32;
    
    unsigned int one = 1;
    char fpath[file_size] = {};

    int extension_index_start = -1; // stores the index from where the extension of the file starts
    
    bpf_probe_read_user(&fpath, file_size, (char*)ctx->args[1]);
    
    for(int i = 0; i < enc_size; i++){
    	if(fpath[i] != enc_dir[i]){
    		return 0;
    	}
    }
    
    // Terminate if it is a hidden file or if the file name is not given
    if(fpath[enc_size] == '\0' || fpath[enc_size] == '.') return 0;

    for(int i = enc_size; i < file_size; i++){
        if(fpath[i] == '\0'){
            // fname[i-enc_size] = '\0';
            break;
        }

        if(fpath[i] == '.' && extension_index_start == -1){
            extension_index_start = i;
        }

        // fname[i-enc_size] = fpath[i];
    }
    
    bpf_printk("OPEN ENTER: %s\n", fpath);
    
    struct array elem;
    struct array hashed_elem;

    int len_i = 0;
    int len;
    
    for(len_i = 0; len_i < file_size && fpath[len_i] != '\0'; len_i++){
    	elem.name[len_i] = fpath[len_i];
        if(len_i >= enc_size && len_i < extension_index_start){
            hashed_elem.name[len_i] =  (char) (((unsigned int) fpath[len_i] + 1) % 256);
        }
        else{
            hashed_elem.name[len_i] = fpath[len_i];
        }
    }

    if(len_i != file_size){
        elem.name[len_i] = '\0';
        hashed_elem.name[len_i] = '\0';
        len = len_i;
    }

    else{
        elem.name[file_size-1] = '\0';
        hashed_elem.name[file_size-1] = '\0';
        len = file_size;
    }

    char encr[file_size];

    for(int i = 0; i < file_size; i++){
        encr[i] = hashed_elem.name[i];
    }

    // If the file is already an encrypted version of another file.. do not encrypt it further
    if(bpf_map_lookup_elem(&hashed_files, &elem) != NULL) return 0;
 

    // printing encrypted file name -> for debugging purpose
    bpf_printk("Encrypted filename: %s\n", encr);

    encr[file_size-1] = '\0';
    
    // bpf_map_update_elem(&map_files, &one, &elem, BPF_ANY);

    bpf_map_update_elem(&map_files, &one, &hashed_elem, BPF_ANY);

    // updating the file-hashed file map
    bpf_map_update_elem(&hashed_files, &hashed_elem, &one, BPF_ANY);

    // rewriting the user argument
    long err = bpf_probe_write_user((char*) ctx->args[1], encr, file_size);

    if(err){
        bpf_printk("Error in write_user: %ld\n",err);
        return 0;
    }

    bpf_printk("Successfully Rewritten file");


    // To check if the argument is updated
    char check_arr[file_size] = {};
    bpf_probe_read_user(&check_arr, file_size, (char*)ctx->args[1]);
    bpf_printk("Updated argument value is %s\n",check_arr);

    return 0;
}

// If map_files[1] has a valid filepath prefix, then add fd to map_fds map

// Clean map_files

SEC("tracepoint/syscalls/sys_exit_openat")
int handle_openat_exit(struct trace_event_raw_sys_exit *ctx)
{
    // Check if we're a process thread of interest
    size_t pid_tgid = bpf_get_current_pid_tgid();
    int pid = pid_tgid >> 32;
    
    unsigned int fd = (unsigned int)ctx->ret;
    
    
    // FIle Path Check
    
    unsigned int one = 1;
    char fname[64] = {};

    struct array* fname_ptr = bpf_map_lookup_elem(&map_files, &one);
    
    if (fname_ptr == NULL) {
        return 0;
    }
    
    //bpf_printk("OPEN EXIT START: %s | %d\n", hfname_ptr->name, fd);
    
    for(int i = 0; i < enc_size; i++){
    	if(fname_ptr->name[i] != enc_dir[i]){
    		return 0;
    	}
    }
    
    if(fname_ptr->name[enc_size] == '\0' || fname_ptr->name[enc_size] == '.') return 0;
    
    // Update map_fds
    
    bpf_map_update_elem(&map_fds, &fd, &one, BPF_ANY);
    bpf_printk("OPEN EXIT END: %s | %d\n", fname_ptr->name, fd);
    
    // Cleanup
    
    bpf_map_delete_elem(&map_files, &one);
    
    return 0;
}

// Clean map_fds


SEC("tracepoint/syscalls/sys_enter_close")
int handle_close_enter(struct trace_event_raw_sys_enter *ctx)
{
    // Check if we're a process thread of interest
    size_t pid_tgid = bpf_get_current_pid_tgid();
    int pid = pid_tgid >> 32;
    
    unsigned int fd = (unsigned int) ctx->args[0];
    
    unsigned int* present = bpf_map_lookup_elem(&map_fds, &fd);
    
    if(present == NULL){
    	return 0;	
    }
    
    bpf_map_delete_elem(&map_fds, &fd);
    bpf_printk("CLOSE ENTER: %d\n", fd);

    return 0;
}
