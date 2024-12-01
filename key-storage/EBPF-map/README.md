# Prerequisites for Working with eBPF Maps

To work with eBPF maps, ensure you have the following prerequisites installed and configured:

## Compiler and Linker
- **clang**
- **llvm**

## Kernel Headers and Development Libraries
- `linux-headers-<kernel-version>`  
- `libelf-dev`

## eBPF Tools
- `bpftool`  
- `libbpf`

## Kernel Features
- **BPF Filesystem**: Enable the BPF filesystem (`/sys/fs/bpf`).  
- **Kernel Version**: Ensure your kernel supports eBPF (Kernel version ≥ 5.4 recommended).

# Steps to Set and Store Keys in eBPF Maps

Follow these steps to set and store keys in eBPF maps:

```bash
# 1. Enable the BPF Filesystem
sudo mount -t bpf bpf /sys/fs/bpf

# 2. Compile the Set Key Program
gcc set_key.c -o set_key -lbpf

# 3. Execute the Set Key Program
sudo ./set_key

# 4. Compile the eBPF Code
clang -O2 -target bpf -c ebpf_encrypt_decrypt.c -o ebpf_encrypt_decrypt.o

# 5. Load the eBPF Program
sudo bpftool prog load ebpf_encrypt_decrypt.o /sys/fs/bpf/ebpf_encrypt_decrypt

# 6. Pin the Key Map
sudo bpftool map pin /sys/fs/bpf/key_map
