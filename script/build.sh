#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

if command -v clang++ >/dev/null 2>&1 && \
	 command -v ld.lld >/dev/null 2>&1 && \
	 command -v llvm-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=clang
	CXX=clang++
	LD=ld.lld
	OBJCOPY=llvm-objcopy
elif command -v i686-elf-g++ >/dev/null 2>&1 && \
		 command -v i686-elf-ld >/dev/null 2>&1 && \
		 command -v i686-elf-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=gcc
	CXX=i686-elf-g++
	LD=i686-elf-ld
	OBJCOPY=i686-elf-objcopy
elif command -v i386-elf-g++ >/dev/null 2>&1 && \
		 command -v i386-elf-ld >/dev/null 2>&1 && \
		 command -v i386-elf-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=gcc
	CXX=i386-elf-g++
	LD=i386-elf-ld
	OBJCOPY=i386-elf-objcopy
else
	echo "[build] Missing compiler tools."
	echo "[build] Need either:"
	echo "[build]   clang++, ld.lld, llvm-objcopy"
	echo "[build] or"
	echo "[build]   i686-elf (or i386-elf) g++, ld, objcopy"
	echo "[build] In MSYS2 MINGW64, search available package names with:"
	echo "[build]   pacman -Ss 'clang|lld|llvm-objcopy'"
	echo "[build]   pacman -Ss 'elf-gcc|cross-gcc|i686-elf|i386-elf'"
	echo "[build]   pacman -Ss 'elf-binutils|cross-binutils|i686-elf|i386-elf'"
	echo "[build] Then install one matching toolchain and retry."
	exit 1
fi

echo "[build] Assembling kernel entry"
nasm -f elf32 source/kernel/kernel_entry.asm -o kernel_entry.o

echo "[build] Compiling kernel.cpp"
if [[ "$TOOLCHAIN" == "clang" ]]; then
	"$CXX" --target=i686-elf -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -nostdlib -nodefaultlibs -I source/type -c source/kernel/kernel.cpp -o kernel.o
else
	"$CXX" -m32 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -nostdlib -nodefaultlibs -I source/type -c source/kernel/kernel.cpp -o kernel.o
fi

echo "[build] Linking kernel.elf"
"$LD" -m elf_i386 -T source/link/linker.ld -nostdlib -o kernel.elf kernel_entry.o kernel.o

echo "[build] Converting kernel.elf -> kernel.bin"
"$OBJCOPY" -O binary kernel.elf kernel.bin

KERNEL_SIZE=$(wc -c < kernel.bin | tr -d '[:space:]')
KERNEL_SECTORS=$(( (KERNEL_SIZE + 511) / 512 ))

echo "[build] Assembling bootloader (kernel sectors: ${KERNEL_SECTORS})"
nasm -f bin source/boot/boot.asm -o boot.bin -DKERNEL_SECTORS="${KERNEL_SECTORS}"

echo "[build] Creating os-image.bin"
cat boot.bin kernel.bin > os-image.bin

IMAGE_SIZE=$(wc -c < os-image.bin | tr -d '[:space:]')
PAD=$(( (512 - (IMAGE_SIZE % 512)) % 512 ))
if [[ "$PAD" -gt 0 ]]; then
	dd if=/dev/zero bs=1 count="$PAD" >> os-image.bin 2>/dev/null
fi

echo "[build] Done: os-image.bin"
