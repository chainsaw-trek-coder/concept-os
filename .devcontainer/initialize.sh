#!/bin/bash

sudo apt update -y

# Grub
sudo apt install grub2-common -y

# Grahpics Environment
sudo apt install x11vnc xvfb fluxbox supervisor novnc -y

# QEMU
sudo apt-get install qemu-system -y
sudo apt install qemu-kvm -y

# CMake
sudo apt install cmake -y

/workspaces/concept-os/.devcontainer/get-cross-compiler.sh