# RSA File Encryption and Decryption

## Description:
This program provides a secure method to encrypt and decrypt files using a combination of RSA and AES algorithms. It generates RSA key pairs for asymmetric encryption of an AES key, which is then used for file encryption/decryption. AES ensures efficient data encryption, while RSA secures the AES key, combining speed and security.

## Prerequisites:
1. Python 3.6 or higher installed.
2. The `cryptography` library installed. You can install it using pip:
   ```bash
   pip install cryptography
   ```
3. Basic knowledge of file paths and command-line operations.

## Running Instructions:

1. **Clone or Download the Script:**
   Ensure the script is saved on your local machine as a `.py` file.

2. **Run the Script:**
   Execute the script in a terminal or command prompt:
   ```bash
   python rsa_file_encryption.py
   ```

3. **Menu Options:**
   - The program presents the following options:
     1. **Generate RSA keys**
        - Prompts for the file names to save the private and public keys.
        - Example:
          ```
          Enter the private key file name: private_key.pem
          Enter the public key file name: public_key.pem
          ```
     2. **Encrypt a file**
        - Prompts for the file to encrypt, the output encrypted file name, and the public key file.
        - Example:
          ```
          Enter the file to encrypt: plaintext.txt
          Enter the output encrypted file name: encrypted_file.enc
          Enter the public key file: public_key.pem
          ```
     3. **Decrypt a file**
        - Prompts for the encrypted file, the output decrypted file name, and the private key file.
        - Example:
          ```
          Enter the file to decrypt: encrypted_file.enc
          Enter the output decrypted file name: decrypted_text.txt
          Enter the private key file: private_key.pem
          ```
     4. **Exit**
        - Exits the program.

4. **File Encryption and Decryption Process:**
   - **Encryption:**
     - A 256-bit AES key is randomly generated and encrypted using the RSA public key.
     - The AES key and initialization vector (IV) are written to the output file, followed by the AES-encrypted file content.
   - **Decryption:**
     - The RSA private key is used to decrypt the AES key from the input file.
     - The decrypted AES key and IV are used to decrypt the file content.

5. **Key Management:**
   - Keep your private key secure. Anyone with access to the private key can decrypt files encrypted with the corresponding public key.
   - Backup your keys to prevent data loss.

## Example Workflow:
1. Generate RSA keys:
   ```
   Enter the private key file name: private_key.pem
   Enter the public key file name: public_key.pem
   Keys generated and saved: private_key.pem, public_key.pem
   ```
2. Encrypt a file:
   ```
   Enter the file to encrypt: document.txt
   Enter the output encrypted file name: document.enc
   Enter the public key file: public_key.pem
   File encrypted and saved to: document.enc
   ```
3. Decrypt a file:
   ```
   Enter the file to decrypt: document.enc
   Enter the output decrypted file name: document_decrypted.txt
   Enter the private key file: private_key.pem
   File decrypted and saved to: document_decrypted.txt
   ```

## Notes:
- The RSA key size is set to 2048 bits for secure key exchange.
- AES uses a 256-bit key with CFB mode for file encryption, ensuring a strong encryption mechanism.
- The program does not encrypt extremely large files directly using RSA, as RSA is computationally expensive and suited for encrypting small data such as keys.
- Ensure proper file permissions for the private key file to prevent unauthorized access.


