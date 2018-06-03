#!/bin/sh
mkdir -p build/sys/boot/grub
cp image.bin build/sys/boot/image.bin
cp build/grub.cfg build/sys/boot/grub/grub.cfg
grub-mkrescue -o ./rhino.iso build/sys --verbose