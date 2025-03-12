#!/bin/bash
sudo apt update -y

# Disk Utilities
sudo apt install grub2-common -y
sudo apt-get install grub-pc-bin -y
sudo apt install parted -y
sudo apt install udev -y

# Grahpics Environment
sudo apt install x11vnc xvfb fluxbox supervisor novnc -y

# QEMU
sudo apt-get install qemu-system -y
sudo apt install qemu-kvm -y
sudo apt install libguestfs-tools -y

# Development Tools
sudo apt-get install gcc-multilib g++-multilib -y
sudo apt install gdb -y
sudo apt install cmake -y

# Build cross-compiler
/workspaces/concept-os/.devcontainer/get-cross-compiler.sh

# Install Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
rustup target add i686-unknown-linux-gnu