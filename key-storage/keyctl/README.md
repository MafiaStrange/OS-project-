Description: keyctl is a command-line utility used to manage the Linux kernel's key management facility. 
             The code consists of keyctl commands that create a new persistent keyring, create a 
             new secret key holding our encryption key and add the key to the keyring. The code also 
             verifies whether the key we created has been added to the keyring we created.

Prerequisites: Linux kernel support (keyctl is a linux utility)
               sudo apt update
               sudo apt install keyutils (keyctl is a part of the keyutils package)

Running Instructions: Ensure that the file is executable (using chmod +x)
                      and directly run it using ./
