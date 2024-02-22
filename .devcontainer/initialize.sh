#!/bin/bash

sudo apt update -y

# Grub
sudo apt install grub2-common -y

# QEMU
sudo apt-get install qemu-system -y
sudo apt install qemu-kvm -y

/workspaces/concept-os/.devcontainer/get-cross-compiler.sh