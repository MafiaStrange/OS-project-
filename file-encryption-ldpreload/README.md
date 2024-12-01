# Description:

This project implements a file protection system that intercepts file system calls (open() and write()) to provide password-based access control. It uses a combination of C (via dynamic linking) and Python for encryption, password management, and metadata handling.

# Prerequisites:

## Required phython libraries

    sudo apt install python3-tk
    sudo apt install python3-pyxattr
    pip3 install pycryptodome keyring pyxattr
    pip install cryptography
    sudo apt-get install libglib2.0-dev


# File Descriptions:

## file_protection_wrapper.c:
1. Intercepts open() and write() calls. 
2. Checks for file attributes, invokes Python scripts for password management.

## file_encryption_handler.py:
1. Handles encryption of file names using AES (CBC mode). 
2. Manages password storage in the system keyring.  
3. Uses extended attributes to store metadata.

## password_verification.py:
1. Verifies user passwords against stored hashes. 
2. Decrypts file names if the correct password is provided.

# How It Works
1. Intercepting open() Calls:
   - When a file is opened, the library checks for the user.password_protect attribute.
    - If set, it invokes the password verification script (password_verification.py).

2. Encrypting File Names:
    - When a new file is created, users are prompted to set a password.
    - The file name is encrypted using AES and stored as a hidden file.
    - A placeholder file is created with a warning message.

3. Intercepting write() Calls:
    - Ensures that any writes to protected files are redirected to the encrypted hidden file.

4. Extended Attributes:
    - user.password_protect: Indicates if the file is password-protected.
    - user.hidden_file: Stores the encrypted, hidden file name.

# Running Instructions:

**Step1:** Compile the file_protection_wrapper.c file into a shared object (.so) file

    gcc -fPIC -shared -o file_protection_wrapper.so file_protection_wrapper.c -ldl

**Step2:** Set the LD_PRELOAD environment variable to preload the shared library:

    export LD_PRELOAD=./file_protection_wrapper.so

This ensures all programs that use open() and write() are intercepted by the library.

**Step3:** Create and protect the file

    touch example.txt
    
When prompted,choose whether to password_protect the file,if "yes" then enter a password(minimum 8 characters).

The file name will be encrypted and replaced with a place holder.

**Step4:** Access the protected file

    cat example.txt
    
Enter the correct password when prompted.

**Step5:** Disable wrapper

To stop intercepting system calls, unset the LD_PRELOAD variable

    unset LD_PRELOAD
