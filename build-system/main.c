#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "build") == 0) {
        system("nasm bootloader.asm -f bin -o bootloader.bin");
        system("nasm kernel_entry.asm -f elf -o kernel_entry.o");
        system("i686-elf-gcc -ffreestanding -m32 -g -c kernel.c -o kernel.o");
        system("i686-elf-ld -o full_kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o --oformat binary");
        system("cat bootloader.bin full_kernel.bin > launch.bin");
    } else if (strcmp(argv[1], "run") == 0) {
        system("qemu-system-i386 -m 128M -fda launch.bin");
    } else {
        printf("Unknow command\n");
    }

    return 0;
}
