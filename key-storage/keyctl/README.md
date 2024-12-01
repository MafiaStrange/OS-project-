# Keyctl Key Management Script

## Description

`keyctl` is a command-line utility used to manage the Linux kernel's key management facility. This script demonstrates the use of `keyctl` commands to:

1. Create a new persistent keyring.
2. Create a new secret key to hold an encryption key.
3. Add the key to the keyring.
4. Verify whether the key has been successfully added to the keyring.

---

## Prerequisites

Ensure the following prerequisites are met before running the script:

1. **Linux Kernel Support**: The script requires `keyctl`, which is a Linux utility.
2. Install the `keyutils` package (if not already installed):
   ```bash
   sudo apt update
   sudo apt install keyutils

---

## Running Instructions

1. **Make the script executable**:
   ```bash
   chmod +x keyctl.sh
2. **Run the script**
   ```bash
   ./keyctl.sh
