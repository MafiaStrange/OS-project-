from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from Crypto.Random import get_random_bytes
import hashlib
import secrets
import xattr
import keyring
import tkinter as tk
from tkinter import simpledialog, messagebox
import sys
import os

def encrypt_file_name(file_name, password, salt):
    # Derive file path and file name

    # file_path = os.path.dirname(file)
    # file_name = os.path.basename(file)

    # Derive the encryption key
    key = hashlib.pbkdf2_hmac('sha256', password.encode('utf-8'), salt, 100000, dklen=32)
    cipher = AES.new(key, AES.MODE_CBC)
    iv = cipher.iv

    # Encrypt the file name
    padded_name = pad(file_name.encode('utf-8'), AES.block_size)
    encrypted_name = cipher.encrypt(padded_name)

    # Encode to a safe format for file systems
    encrypted_name_hex = iv.hex() + encrypted_name.hex()

    # Prefix with a dot to make it hidden
    hidden_encrypted_name = f".{encrypted_name_hex}"

    # Rename the original file to the hidden file
    # hidden_file_path = os.path.join(file_path, hidden_encrypted_name)
    hidden_file_path = os.path.join(os.path.dirname(file_name), hidden_encrypted_name)
    os.rename(file_name, hidden_file_path)

    # Create a placeholder file
    with open(file_name, 'w') as placeholder:
        placeholder.write(f"This file is protected. Access requires a password.\n")

    # Store metadata about the hidden file in extended attributes
    set_file_attribute_hiddenfile(file_name, hidden_encrypted_name)
    print(f"File '{file_name}' is now protected and hidden as '{hidden_file_path}'.")


def set_file_attribute_hiddenfile(file_name, hidden_name):
    try:
        # Set the hidden file name as an extended attribute
        xattr.setxattr(file_name, "user.hidden_file", hidden_name.encode('utf-8'))
        print(f"Set extended attribute 'user.hidden_file' to: {hidden_name}")
    except Exception as e:
        print(f"Failed to set extended attribute: {e}")

    try:
        xattr.setxattr(file_name,"user.password_protect","1".encode('utf-8'))
        print(f"Set extended attribute 'user.password_protect' to : 1")
    except Exception as e:
        print(f"Failed to set extended attribute: {e}")

    try:
        xattr.setxattr(hidden_name,"user.hidden","1".encode('utf-8'))
        print(f"Set extended attribute 'user.hidden' for the file: {hidden_name}")
    except Exception as e:
        print(f"Failed to set extended attribute: {e}")
        
def generate_salt():
    return secrets.token_bytes(16)

def hash_password(password,salt):
    """
    Hashing the password using sha-256
    """
    hashed = hashlib.pbkdf2_hmac('sha256',password.encode('utf-8'),salt,100000)
    return hashed.hex()

def store_in_keyring(file_name,hashed_password,salt):
        try:
            keyring.set_password("FilePasswordManager",f"{file_name}_hash",hashed_password)
            keyring.set_password("FilePasswordManager",f"{file_name}_salt",salt.hex())
            print(f"Hashed password and salt stored in keyring for file : {file_name}")
        except Exception as e:
            print(f"Failed to store hashed password in keyring : {e}")

def password_prompt(file_name):
    root = tk.Tk()
    root.withdraw()  # Hide the root window

    # Ask the user if they want to password-protect the file
    response = simpledialog.askstring("File Creation Detected",
                                      f"Do you want to password-protect the file '{file_name}'? (yes/no)")

    if response and response.lower() == "yes":
        while True:
            # Prompt for password
            password = simpledialog.askstring("Password Protection", "Enter password (minimum 8 characters):", show="*")
            if password is None:
                set_file_attribute(file_name, 0)  # Set attribute to "0" when canceled
                return
            if len(password) >= 8:
                # Store the key into the keyring in this part
                salt = generate_salt()
                hashed_password = hash_password(password,salt)
                store_in_keyring(file_name,hashed_password,salt)
                # set_file_attribute(file_name, 1)  # Set attribute to "1" for password protection
                # Encrypt the file after setting the password
                encrypt_file_name(file_name, password, salt)
                return
            else:
                messagebox.showerror("Invalid Password", "Password must be at least 8 characters long.")
    else:
        set_file_attribute(file_name, 0)  # Set attribute to "0" for no password protection

def set_file_attribute(file_name, protect_flag):
    # print("enter")
    # Set the extended attribute to 1 (password protect) or 0 (no password)
    try:
        # Use pyxattr.setxattr to set the extended attribute
        xattr.setxattr(file_name, "user.password_protect", str(protect_flag).encode('utf-8'))
        print(f"Set extended attribute 'user.password_protect' to: {protect_flag}")
    except Exception as e:
        print(f"Failed to set extended attribute: {e}")


if __name__ == "__main__":
    # Get the file name passed as a command-line argument
    if len(sys.argv) != 2:
        print("Error: Please provide a file name.")
        sys.exit(1)
    
    file_name = sys.argv[1]
    file_name = os.path.basename(file_name)
    password_prompt(file_name)


#sudo apt install python3-pyxattr

# or download from souce => 
# git clone https://github.com/iustin/pyxattr.git
#cd pyxattr
#python3 setup.py install --user

# pip3 install pycryptodome
#pip3 install pycryptodomex