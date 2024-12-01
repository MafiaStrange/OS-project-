import tkinter as tk
from tkinter import simpledialog, messagebox
import binascii
import keyring
import hashlib
import sys
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding
from cryptography.hazmat.backends import default_backend
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad
import os
import xattr


def verify_password(file_name,password):
    try:
        # retrieve the stored value from the keyring
        stored_hash = keyring.get_password("FilePasswordManager",f"{file_name}_hash")
        stored_salt = keyring.get_password("FilePasswordManager",f"{file_name}_salt")

        if not stored_hash or not stored_salt :
            return False # No hash or salt found, invalid password
        
        # convert stored salt from hex strings back to bytes
        salt = binascii.unhexlify(stored_salt)

        # hash the provided password with the stored salt
        computed_hash = hashlib.pbkdf2_hmac('sha256',password.encode('utf-8'),salt,100000).hex()

        # compare the computed hash with the stored hash
        return computed_hash == stored_hash

    except Exception as e :
        print(f"Error verifying password : {e}")
        return False

def ask_password(file_name):
    # Create a Tkinter root window (this will not appear)
    root = tk.Tk()
    root.withdraw()  # Hide the root window
    
    # Ask for the password using a password entry
    password = simpledialog.askstring("Password Required", f"Enter your password for file {file_name} :", show="*")
    
    # Check if the user pressed cancel (password will be None)
    if password is None:
        print("Password input was canceled.")
        sys.exit(1)
    
    password_correct = verify_password(file_name, password)

    if(password_correct):
        print(f"Received the right password : {password}")
        sys.exit(0)
    else :
        messagebox.showerror("Access Denied", "Incorrect Password!!")
        sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Error: Please provide a file name.")
        sys.exit(1)
    
    file_name = sys.argv[1]
    file_name = os.path.basename(file_name)
    ask_password(file_name)