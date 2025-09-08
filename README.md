# CryptoGuard: Lightweight Hybrid Detection and Response to Host-based Cryptojackers in Linux Cloud Environments

## How to Use
### - Installation
Environment setup:</br>
<code style="color : color">./setup.sh</code>

### - Example
You may start with our [eBPF_syscall](https://github.com/PGHOON/eBPF_syscall) repo to get a better grasp of the monitoring workflow.

#### - API
Please refer to our [eBPF_API](https://github.com/PGHOON/eBPF_API) repo.

## Dataset
The main syscall dataset is located in:<br>
<code style="color : color">CryptoGuard/dataset/drive/</code></br>

There are split files of compressed archive.</br>
//├── CryptoGuard_gd.tar.xz.part_aa</br>
//└── CryptoGuard_gd.tar.xz.part_ab</br>

To extract:</br>
<code style="color : color">cd CryptoGuard/dataset/drive</code></br>
<code style="color : color">cat CryptoGuard_gd.tar.xz.part_* > CryptoGuard_gd.tar.xz</code></br>
<code style="color : color">tar -xvf CryptoGuard_gd.tar.xz</code></br>


## Export Citation
Any use of our repository that results in academic or other publications with a bibliography must include citations as listed below:</br>

@inproceedings{10.1145/3708821.3736186,</br>
author = {Park, Gyeonghoon and Kim, Jaehan and Choi, Jinu and Kim, Jinwoo},</br>
title = {CryptoGuard: Lightweight Hybrid Detection and Response to Host-based Cryptojackers in Linux Cloud Environments},</br>
year = {2025},</br>
isbn = {9798400714108},</br>
publisher = {Association for Computing Machinery},</br>
address = {New York, NY, USA},</br>
url = {[https://doi.org/10.1145/3708821.3736186](https://doi.org/10.1145/3708821.3736186)},</br>
doi = {10.1145/3708821.3736186},</br>
abstract = {Host-based cryptomining malware, commonly known as cryptojackers, have gained notoriety for their stealth and the significant financial losses they cause in Linux-based cloud environments. Existing solutions often struggle with scalability due to high monitoring overhead, low detection accuracy against obfuscated behavior, and lack of integrated remediation. We present CryptoGuard, a lightweight hybrid solution that combines detection and remediation strategies to counter cryptojackers. To ensure scalability, CryptoGuard uses sketch- and sliding window-based syscall monitoring to collect behavior patterns with minimal overhead. It decomposes the classification task into a two-phase process, leveraging deep learning models to identify suspicious activity with high precision. To counter evasion techniques such as entry point poisoning and PID manipulation, CryptoGuard integrates targeted remediation mechanisms based on eBPF, a modern Linux kernel feature deployable on any compatible host. Evaluated on 123 real-world cryptojacker samples, it achieves average F1-scores of 96.12\% and 92.26\% across the two phases, and outperforms state-of-the-art baselines in terms of true and false positive rates, while incurring only 0.06\% CPU overhead per host.},
booktitle = {Proceedings of the 20th ACM Asia Conference on Computer and Communications Security},</br>
pages = {1617–1631},</br>
numpages = {15},</br>
keywords = {Cryptojacking, Cloud Security, eBPF, System Call Monitoring},</br>
location = {</br>
},</br>
series = {ASIA CCS '25}</br>
}</br>
