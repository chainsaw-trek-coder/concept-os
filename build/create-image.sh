#!/bin/bash

# This is a hack to get around the restrictions in Codespaces.

# Download minimal Ubuntu image
UBUNTU_IMAGE_URL="https://cloud-images.ubuntu.com/minimal/releases/focal/release/ubuntu-20.04-minimal-cloudimg-amd64.img" # TODO: Find an image that is more compatible with Codespaces default kernel
UBUNTU_IMAGE="ubuntu-20.04-minimal-cloudimg-amd64.img"

# Check if the image already exists
if [ ! -f $UBUNTU_IMAGE ]; then
    echo "Downloading Ubuntu image..."
    wget $UBUNTU_IMAGE_URL -O $UBUNTU_IMAGE

    sudo virt-customize -a $UBUNTU_IMAGE \
        --install grub-pc-bin,grub-efi-amd64-bin,parted,udev \
        --run-command 'apt-get update && apt-get install -y grub-pc-bin grub-efi-amd64-bin parted udev' \
        --run-command 'ssh-keygen -A' \
        --run-command 'systemctl enable ssh' \
        --run-command 'mkdir -p /root/.ssh && chmod 700 /root/.ssh' \
        --run-command 'echo "PermitRootLogin yes" >> /etc/ssh/sshd_config' \
        --run-command 'echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config' \
        --run-command 'echo "PermitEmptyPasswords yes" >> /etc/ssh/sshd_config' \
        --root-password password:ubuntu
else
    echo "Ubuntu image already exists."
fi

# Create a new image for the bootable OS
BOOTABLE_IMAGE="bootable-os.img"
qemu-img create -f qcow2 $BOOTABLE_IMAGE 5M

# Run the Ubuntu image in QEMU
qemu-system-x86_64 -m 512 -hda $UBUNTU_IMAGE -hdb $BOOTABLE_IMAGE -net nic -net user,hostfwd=tcp::5000-:22 -nographic &

# Wait for the VM to boot
running=0
while [ $running -eq 0 ]; do
    sleep 1
    if ssh -q -o "BatchMode=yes" -o "ConnectTimeout=5" -p 5000 localhost "exit"; then
        running=1
    else
        echo "Waiting for SSH server to start..."
    fi
done

# Install GRUB utilities in the VM
# ssh -p 2222 -o "StrictHostKeyChecking=no" ubuntu@localhost << EOF
# sudo apt-get update
# sudo apt-get install -y grub-pc-bin grub-efi-amd64-bin
# sudo apt install parted -y
# sudo apt install udev -y
# EOF

# Copy the kernel to the VM
scp -P 5000 -o "StrictHostKeyChecking=no" ./kernel-loader/bin/kernel_loader root@localhost:/home/ubuntu/kernel_loader

# Create a bootable image
ssh -p 5000 -o "StrictHostKeyChecking=no" root@localhost << EOF
sudo mkdir -p /mnt/boot
sudo mount /dev/sdb1 /mnt
sudo grub-install --target=i386-pc --boot-directory=/mnt/boot /dev/sdb
sudo cp /home/ubuntu/kernel_loader /mnt/boot/

# Create GRUB configuration
cat << GRUB_CFG | sudo tee /mnt/boot/grub/grub.cfg
set timeout=5
set default=0

menuentry "Concept OS" {
    multiboot /boot/kernel_loader
    boot
}
GRUB_CFG

sudo umount /mnt
EOF

# Download the bootable image
scp -P 5000 -o "StrictHostKeyChecking=no" root@localhost:/home/ubuntu/$BOOTABLE_IMAGE ./

# Cleanup
ssh -p 5000 -o "StrictHostKeyChecking=no" root@localhost "sudo poweroff"

# BOOT_FS=$(mktemp -d)
# mkdir -p $BOOT_FS/boot/grub
# cp ./kernel-loader/bin/kernel_loader $BOOT_FS/boot/

# # Create GRUB configuration

# cat << EOF | sudo tee $BOOT_FS/boot/grub/grub.cfg
# set timeout=5
# set default=0

# menuentry "Concept OS" {
#     multiboot /boot/kernel_loader
#     boot
# }
# EOF

# dd if=/dev/zero of=./image.img bs=512 count=8024

# parted --script ./image.img mklabel msdos
# parted --script ./image.img mkpart primary 1MiB 100%

# mkfs.ext4 ./image.img -d $BOOT_FS
# grub-install --target=i386-pc --boot-directory=$BOOT_FS/boot --modules="part_msdos ext2" ./image.img

# Create a loop device
# LOOP_DEVICE=$(sudo losetup --show -fP ./image.img)

# Format the partition

# Mount the partition
# MOUNT_POINT=$(mktemp -d)
# sudo mount ${LOOP_DEVICE}p1 $MOUNT_POINT

# Create directories
# sudo grub-install --target=i386-pc --boot-directory=$MOUNT_POINT/boot --modules="part_msdos ext2" $LOOP_DEVICE
# sudo cp ./kernel-loader/bin/kernel_loader $MOUNT_POINT/boot/

# Create GRUB configuration

# cat << EOF | sudo tee $MOUNT_POINT/boot/grub/grub.cfg
# set timeout=5
# set default=0

# menuentry "My OS" {
#     multiboot /boot/kernel_loader
#     boot
# }
# EOF

# # Cleanup
# sudo umount $MOUNT_POINT
# sudo losetup -d $LOOP_DEVICE
# rm -rf $MOUNT_POINT