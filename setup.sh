#!/bin/bash

# system update & necessary packages
sudo apt-get update
sudo apt-get install -y \
    pkg-config \
    gcc \
    clang \
    llvm \
    libelf-dev \
    make \
    git \
    wget \
    lsb-release \
    software-properties-common \
    gnupg

# clang & llvm
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh

sudo apt-get update
sudo apt-get install -y clang-18 llvm-18 libclang-18-dev

# Environment variable
export CC=clang-18
export LLVM=llvm-18
export CLANG=clang-18

# libbpf
git clone https://github.com/PGHOON/eBPF_syscall.git ~/eBPF_syscall
cd ~/eBPF_syscall/libbpf/src
sudo make install

# bpftool
git clone --recurse-submodules https://github.com/libbpf/bpftool.git ~/bpftool
cd ~/bpftool/src
sudo make install
