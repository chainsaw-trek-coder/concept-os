BOOT_FS=$(mktemp -d)
mkdir -p $BOOT_FS/boot/grub
cp ./kernel-loader/bin/kernel_loader $BOOT_FS/boot/

# Create GRUB configuration

cat << EOF | sudo tee $BOOT_FS/boot/grub/grub.cfg
set timeout=5
set default=0

menuentry "Concept OS" {
    multiboot /boot/kernel_loader
    boot
}
EOF

# Create Bootable CD-ROM
grub-mkrescue -o ./image.iso $BOOT_FS

# Create Hard Drive
dd if=/dev/zero of=./image.img bs=512 count=8024
parted --script ./image.img mklabel msdos
parted --script ./image.img mkpart primary 1MiB 100%

# mkfs.ext4 ./image.img -d $BOOT_FS
# grub-install --target=i386-pc --boot-directory=$BOOT_FS/boot --modules="part_msdos ext2" ./image.img

# This stuff requires extra permissions.
# # Create a loop device
# LOOP_DEVICE=$(sudo losetup --show -fP ./image.img)

# # Format the partition

# # Mount the partition
# MOUNT_POINT=$(mktemp -d)
# sudo mount ${LOOP_DEVICE}p1 $MOUNT_POINT

# # Create directories
# sudo grub-install --target=i386-pc --boot-directory=$MOUNT_POINT/boot --modules="part_msdos ext2" $LOOP_DEVICE
# sudo cp ./kernel-loader/bin/kernel_loader $MOUNT_POINT/boot/

# # Create GRUB configuration

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