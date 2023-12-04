#!/bin/bash
sudo apt-get update

sudo apt-get install -y apt-transport-https ca-certificates curl clang llvm jq
sudo apt-get install -y libelf-dev libpcap-dev libbfd-dev binutils-dev build-essential make
sudo apt-get install -y linux-tools-common linux-tools-$(uname -r)
sudo apt-get install -y bpfcc-tools
sudo apt-get install -y python3-pip

#+ build libbpf, bpftool
