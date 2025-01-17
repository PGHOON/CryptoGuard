# Base image
FROM ubuntu:22.04

# Install necessary packages
RUN apt-get update && apt-get install -y \
    pkg-config gcc clang llvm libelf-dev make git wget lsb-release \
    software-properties-common gnupg curl && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Install specific LLVM and Clang versions
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && \
    apt-get update && apt-get install -y clang-18 llvm-18 libclang-18-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Set environment variables for LLVM and Clang
ENV CC=clang-18 \
    LLVM=llvm-18 \
    CLANG=clang-18

# Clone and build eBPF_syscall project
RUN git clone https://github.com/PGHOON/eBPF_syscall.git /eBPF_syscall && \
    cd /eBPF_syscall/libbpf/src && make install && \
    git clone --recurse-submodules https://github.com/libbpf/bpftool.git /bpftool && \
    cd /bpftool/src && make install

# Compile the syscall tracing program
WORKDIR /eBPF_syscall/multiprocess
RUN make

# Set the default command to run the eBPF program
CMD ["./syscall_trace_all"]
