#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 1024
char* print_attribute;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file.ossimp>\n", argv[0]);
        return 1;
    }
    
    char *filename = argv[1];
    if (strstr(filename, ".ossimp") == NULL) {
        printf("Error: file must have .ossimp extension\n");
        return 1;
    }
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: failed to open file %s\n", filename);
        return 1;
    }

    char* line[1024];
    while (fgets(line, 1024, file) != NULL ) {
            if (strstr(line, "create_bootloader") != NULL ) {
                FILE *bootloader_file = fopen("bootloader.asm", "w");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';
                fprintf(bootloader_file, "[org 0x7c00]\n");
                fprintf(bootloader_file, "kernel_adress equ %s\n", text);
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "mov [boot_disk], dl\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "xor ax, ax\n");
                fprintf(bootloader_file, "mov es, ax\n");
                fprintf(bootloader_file, "mov ds, ax\n");
                fprintf(bootloader_file, "mov bp, 0x8000\n");
                fprintf(bootloader_file, "mov sp, bp\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "mov bx, kernel_adress\n");
                fprintf(bootloader_file, "mov dh, 50\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "mov ah, 0x02\n");
                fprintf(bootloader_file, "mov al, dh\n");
                fprintf(bootloader_file, "mov ch, 0x00\n");
                fprintf(bootloader_file, "mov dh, 0x00\n");
                fprintf(bootloader_file, "mov cl, 0x02\n");
                fprintf(bootloader_file, "mov dl, [boot_disk]\n");
                fprintf(bootloader_file, "int 0x13\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "mov ah, 0x0\n");
                fprintf(bootloader_file, "mov al, 0x3\n");
                fprintf(bootloader_file, "int 0x10\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "code_segement equ gdt_code - gdt_start\n");
                fprintf(bootloader_file, "data_segement equ gdt_data - gdt_start\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "cli\n");
                fprintf(bootloader_file, "lgdt [gdt_descriptor]\n");
                fprintf(bootloader_file, "mov eax, cr0\n");
                fprintf(bootloader_file, "or eax, 1\n");
                fprintf(bootloader_file, "mov cr0, eax\n");
                fprintf(bootloader_file, "jmp code_segement:start_protected_mode\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "jmp $\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "boot_disk: db 0\n");
                fprintf(bootloader_file, "\n");
                fprintf(bootloader_file, "gdt_start:\n");
                fprintf(bootloader_file, "\tgdt_null:\n");
                fprintf(bootloader_file, "\t\tdd 0x0\n");
                fprintf(bootloader_file, "\t\tdd 0x0\n");
                fprintf(bootloader_file, "\tgdt_code:\n");
                fprintf(bootloader_file, "\t\tdw 0xffff\n");
                fprintf(bootloader_file, "\t\tdw 0x0\n");
                fprintf(bootloader_file, "\t\tdb 0x0\n");
                fprintf(bootloader_file, "\t\tdb 0b10011010\n");
                fprintf(bootloader_file, "\t\tdb 0b11001111\n");
                fprintf(bootloader_file, "\t\tdb 0x0\n");
                fprintf(bootloader_file, "\tgdt_data:\n");
                fprintf(bootloader_file, "\t\tdw 0xffff\n");
                fprintf(bootloader_file, "\t\tdw 0x0\n");
                fprintf(bootloader_file, "\t\tdb 0x0\n");
                fprintf(bootloader_file, "\t\tdb 0b10010010\n");
                fprintf(bootloader_file, "\t\tdb 0b11001111\n");
                fprintf(bootloader_file, "\t\tdb 0x0\n");
                fprintf(bootloader_file, "gdt_end:\n");
                fprintf(bootloader_file, "gdt_descriptor:\n");
                fprintf(bootloader_file, "\tdw gdt_end - gdt_start - 1\n");
                fprintf(bootloader_file, "\tdd gdt_start\n");
                fprintf(bootloader_file, "[bits 32]\n");
                fprintf(bootloader_file, "start_protected_mode:\n");
                fprintf(bootloader_file, "\tmov ax, data_segement\n");
                fprintf(bootloader_file, "\tmov ds, ax\n");
                fprintf(bootloader_file, "\tmov ss, ax\n");
                fprintf(bootloader_file, "\tmov es, ax\n");
                fprintf(bootloader_file, "\tmov fs, ax\n");
                fprintf(bootloader_file, "\tmov gs, ax\n");
                fprintf(bootloader_file, "\tmov ebp, 0x90000\n");
                fprintf(bootloader_file, "\tmov esp, ebp\n");
                fprintf(bootloader_file, "\tjmp kernel_adress\n");
                fprintf(bootloader_file, "times 510-($-$$) db 0\n");
                fprintf(bootloader_file, "dw 0xaa55\n");

                fclose(bootloader_file);
            }

            if (strstr(line, "create_entry") != NULL ) {
                FILE *kernel_entry_file = fopen("kernel_entry.asm", "w");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';

                fprintf(kernel_entry_file, "section .text\n");
                fprintf(kernel_entry_file, "\t[bits 32]\n");
                fprintf(kernel_entry_file, "\t[extern _start]\n");
                fprintf(kernel_entry_file, "\tcall _start\n");
                fprintf(kernel_entry_file, "\tjmp $\n");

                fclose(kernel_entry_file);
            }

            if (strstr(line, "create_kernel") != NULL ) {
                FILE *kernel_file = fopen("kernel.c", "w");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';
                fprintf(kernel_file, "#include \"function.c\"\n\n");
                fprintf(kernel_file, "void _start(){\n");

                fclose(kernel_file);
            }

            if (strstr(line, "print") != NULL) {
                FILE *source_file = fopen("kernel.c", "a+");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';

                char *token = strtok(text, ",");
                fprintf(source_file, "\tprint(\"%s\", ", token);
                token = strtok(NULL, ",");
                while (token != NULL) {
                    fprintf(source_file, "%s", token);
                    token = strtok(NULL, ",");
                    if (token != NULL)
                        fprintf(source_file, ", ");
                }
                fprintf(source_file, ");\n");
                fclose(source_file);
            }

            if (strstr(line, "close_kernel") != NULL ) {
                FILE *kernel_source_file = fopen("kernel.c", "a+");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';

                fprintf(kernel_source_file, "\treturn;\n");
                fprintf(kernel_source_file, "}\n");

                fclose(kernel_source_file);
            }

            if (strstr(line, "init_vga") != NULL ) {
                FILE *function_file = fopen("function.c", "w");

                char *start = strchr(line, '[');
                char *end = strchr(line, ']');
                int length = end - start - 1;
                char text[length + 1];
                memcpy(text, start + 1, length);
                text[length] = '\0';


                fprintf(function_file, "#include <stdint.h>\n");
                fprintf(function_file, "#include <stddef.h>\n");
                fprintf(function_file, "#include <stdarg.h>\n");
                fprintf(function_file, "#include <stdbool.h>\n");

                fprintf(function_file, "void print(const char *str, unsigned char attrib) {\n");
                fprintf(function_file, "\tstatic unsigned short *video_memory = (unsigned short *)0xB8000;\n");
                fprintf(function_file, "\tstatic unsigned int cur_pos = 0;\n");

                fprintf(function_file, "\tfor (int i = 0; str[i] != '\\0'; i++) {\n");
                fprintf(function_file, "\t\tswitch (str[i]) {\n");
                fprintf(function_file, "\t\t\tcase '\\n':\n");
                fprintf(function_file, "\t\t\t\tcur_pos = (cur_pos / 80 + 1) * 80;\n");
                fprintf(function_file, "\t\t\t\tbreak;\n");
                fprintf(function_file, "\t\t\tbreak;\n");
                fprintf(function_file, "\t\t\tdefault:\n");
                fprintf(function_file, "\t\t\t\tvideo_memory[cur_pos++] = (unsigned short)((attrib << 8) | str[i]);\n");
                fprintf(function_file, "\t\t\t\tbreak;\n");
                fprintf(function_file, "\t\t}\n");

                fprintf(function_file, "\tif (cur_pos >= 80 * 25) {\n");
                fprintf(function_file, "\t\tint j;\n");

                fprintf(function_file, "\t\tfor (j = 0; j < 80 * 24; j++) {\n");
                fprintf(function_file, "\t\t\tvideo_memory[j] = video_memory[j + 80];\n");
                fprintf(function_file, "\t\t}\n");

                fprintf(function_file, "\t\tfor (; j < 80 * 25; j++) {\n");
                fprintf(function_file, "\t\t\tvideo_memory[j] = (unsigned short)((attrib << 8) | ' ');\n");
                fprintf(function_file, "\t\t}\n");

                fprintf(function_file, "\t\t\tcur_pos = 80 * 24;\n");
                fprintf(function_file, "\t\t\t}\n");
                fprintf(function_file, "\t\t}\n");
                fprintf(function_file, "\t}\n");


                fclose(function_file);
            }
        }
    fclose(file);

    return 0;
}
