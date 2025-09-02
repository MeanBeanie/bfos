#!/bin/bash

if [[ "$1" == "debug" ]]; then
	qemu-system-x86_64 -cdrom ./bfOS.iso --no-shutdown --no-reboot -S -s &
	gdb ./bin/bfOS
else
	qemu-system-x86_64 -cdrom ./bfOS.iso --no-shutdown --no-reboot
fi
