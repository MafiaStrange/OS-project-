# Opensnoop-replica (Tool for enrypted File Redirection)
## Description
Traces syscalls at kernel level and safely prompts the user to manually redirect to the encrypted file location.

---

## Prerequisites

Before running the tool, ensure the following are installed and configured on your system:

1. **clang**: For compiling the eBPF code.
2. **llvm**: Used alongside clang for compiling and linking.
3. **Linux Headers**: Install kernel headers matching your system's kernel version:
   ```bash
   sudo apt install linux-headers-$(uname -r)
4. **python-bcc** : For API calls to attach EBPF bytecode to the kernel
   ```bash
   sudo apt install python3-bcc

---

## Running Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/MafiaStrange/bpf-file-encryption-/tree/main/opensnoop-replica
   cd opensnoop-replica

2. **Run the python script**
   ```bash
   sudo python3 snoop_replica.py

3. **Create, read, and write files in the Encrypted directory, by default is "/home/vboxuser/Encrypted"**
