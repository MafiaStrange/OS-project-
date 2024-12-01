# File Encryption and Decryption using AES

This Python script provides a simple way to encrypt and decrypt files using the Advanced Encryption Standard (AES) with CBC mode. It ensures the security of your data by combining password-based key derivation, salt, and padding mechanisms.

## Features
- **File Encryption**: Protect sensitive files by encrypting them with a password.
- **File Decryption**: Restore encrypted files using the correct password.
- **Secure Key Generation**: Uses PBKDF2-HMAC with SHA-256 and a salt for generating encryption keys.

## Prerequisites

This script requires the `cryptography` library. Install it using pip if you don’t already have it:

```bash
pip install cryptography
```

## How It Works

### Key Derivation
- A 256-bit AES key is derived using PBKDF2-HMAC with:
  - A user-provided password
  - A random 16-byte salt
  - 100,000 iterations for computational difficulty

### Encryption
1. A random 16-byte Initialization Vector (IV) is generated for AES in CBC mode.
2. The file is padded using PKCS7 to match the AES block size.
3. The padded data is encrypted and stored along with the salt and IV.

### Decryption
1. The salt and IV are extracted from the encrypted file.
2. The same key derivation process regenerates the AES key using the password and salt.
3. The encrypted data is decrypted, and the padding is removed to restore the original file.

## Usage

### 1. Run the Script
```bash
python aes_encryption.py
```

### 2. Choose an Option
- **Encrypt a File**: Protect a file with a password.
- **Decrypt a File**: Restore an encrypted file using the same password.

### Example

#### Encryption
```bash
Choose an option (1/2): 1
Enter the file to encrypt: sensitive_data.txt
Enter the output encrypted file name: encrypted_data.bin
Enter a password for encryption: my_secure_password
File 'sensitive_data.txt' has been encrypted and saved as 'encrypted_data.bin'.
```

#### Decryption
```bash
Choose an option (1/2): 2
Enter the file to decrypt: encrypted_data.bin
Enter the output decrypted file name: decrypted_data.txt
Enter the password for decryption: my_secure_password
File 'encrypted_data.bin' has been decrypted and saved as 'decrypted_data.txt'.
```

## File Format
- The encrypted file stores:
  1. **Salt** (16 bytes)
  2. **IV** (16 bytes)
  3. **Encrypted Data** (variable length)

## Notes
- Keep the password secure! Without it, the data cannot be recovered.
- Do not modify the encrypted file, as this will result in decryption failure.



