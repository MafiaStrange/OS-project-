# eBPF File Re-Direction Tool

# Project Description

This project is an **eBPF-based tool** designed to provide **file encryption and decryption** capabilities for files within a specific directory (`/home/vboxuser/encrypted/`). It intercepts system calls such as `openat`, `read`, and `write` to dynamically **encrypt data during writes** and **decrypt data during reads**, ensuring that files are stored securely in an encrypted format.

The implementation uses **eBPF** programs to operate in kernel space and employs the **eunomia-bpf toolkit** for efficient compilation, kernel attachment, and runtime management. By leveraging the flexibility of eBPF, this tool achieves transparent encryption and decryption without requiring modifications to user-space applications.


---

## Prerequisites

Before running the tool, ensure the following are installed and configured on your system:

1. **clang**: For compiling the eBPF code.
2. **llvm**: Used alongside clang for compiling and linking.
3. **Linux Headers**: Install kernel headers matching your system's kernel version:
   ```bash
   sudo apt install linux-headers-$(uname -r)
4. **Eunomia-BPF**: Install eunomia-bpf for compiling and managing eBPF programs, follow the official eunomia-bpf installation guide for setup instructions.
   - ecc: Eunomia's compiler for building eBPF programs.
   - ecli: Eunomia's CLI tool for attaching and running eBPF programs.
---

## Running Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/MafiaStrange/bpf-file-encryption-/tree/main/file-encryption-eunomia-bpf
   cd file-encryption-eunomia-bpf
2. **Make sure ecc and ecli are part of the working directory**
   ```bash
   ls flle-encryption-eunomia-bpf | grep ecc
   ls flle-encryption-eunomia-bpf | grep ecli
   chmod +x ecc
   chmod +x ecli
3. **Compile the EBPF Program**
   ```bash
   ./ecc file-rerouting.bpf.c
4. **Run the EBPF program by attacking it to the kernel**
   ```bash
   sudo ./ecli run package.json
5. **Monitor the trace_pipe**
   ```bash
   sudo cat sys/kernel/debug/tracing/trace_pipe
6. **Create, read, and write files in the Encrypted directory, by default is "/home/vboxuser/Encrypted"**
