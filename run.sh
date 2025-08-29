#!/bin/bash

nasm boot.nasm -f bin -o boot.bin
qemu-system-x86_64 boot.bin --no-reboot
