cmd_/root/little-pinguin/07/id.ko := ld -r -m elf_x86_64 -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /root/little-pinguin/07/id.ko /root/little-pinguin/07/id.o /root/little-pinguin/07/id.mod.o ;  true