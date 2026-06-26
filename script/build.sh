#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

if command -v clang++ >/dev/null 2>&1 && \
	 command -v ld.lld >/dev/null 2>&1 && \
	 command -v llvm-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=clang
	CXX=clang++
	LD=ld.lld
	OBJCOPY=llvm-objcopy
    TARGET_FLAGS=(--target=i686-elf)
elif command -v i686-elf-g++ >/dev/null 2>&1 && \
		 command -v i686-elf-ld >/dev/null 2>&1 && \
		 command -v i686-elf-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=gcc
	CXX=i686-elf-g++
	LD=i686-elf-ld
	OBJCOPY=i686-elf-objcopy
    TARGET_FLAGS=()
elif command -v i386-elf-g++ >/dev/null 2>&1 && \
		 command -v i386-elf-ld >/dev/null 2>&1 && \
		 command -v i386-elf-objcopy >/dev/null 2>&1; then
	TOOLCHAIN=gcc
	CXX=i386-elf-g++
	LD=i386-elf-ld
	OBJCOPY=i386-elf-objcopy
    TARGET_FLAGS=()
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

INPUTS_CPP_SOURCES=(
	source/input/keyboard.cpp
)

INTERRUPTS_CPP_SOURCES=(
	source/interrupts/idt.cpp
	source/interrupts/interrupt_handlers.cpp
)

KERNEL_CPP_SOURCES=(
	source/kernel/kernel_main.cpp
)

STD_CPP_SOURCES=(
	source/std/memcpy.cpp
)

CPP_SOURCES=(
	"${INPUTS_CPP_SOURCES[@]}"
	"${INTERRUPTS_CPP_SOURCES[@]}"
	"${KERNEL_CPP_SOURCES[@]}"
	"${STD_CPP_SOURCES[@]}"
)

CXXFLAGS=(
	"${TARGET_FLAGS[@]}"
	-ffreestanding
	-fno-exceptions
	-fno-rtti
	-fno-stack-protector
	-nostdlib
	-nodefaultlibs
	-I source
)

echo "[build] Assembling kernel entry"
nasm -f elf32 source/kernel/kernel_entry.asm -o "$BUILD_DIR/kernel_entry.o"

echo "[build] Assembling ISR stubs"
nasm -f elf32 source/interrupts/isr.asm -o "$BUILD_DIR/isr.o"

CPP_OBJECTS=()

echo "[build] Compiling C++ sources"
for src in "${CPP_SOURCES[@]}"; do
	if [[ ! -f "$src" ]]; then
		echo "[build] Missing source file: $src"
		exit 1
	fi

	rel="${src#source/}"
	obj="${rel//\//_}"
	obj="${obj%.cpp}.o"
	obj_path="$BUILD_DIR/$obj"

	echo "[build]   $src -> $obj_path"
	"$CXX" "${CXXFLAGS[@]}" -c "$src" -o "$obj_path"
	CPP_OBJECTS+=("$obj_path")
done

echo "[build] Linking kernel"
"$LD" -m elf_i386 -T source/link/linker.ld -nostdlib -o "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/isr.o" "${CPP_OBJECTS[@]}"

echo "[build] Creating kernel binary"
"$OBJCOPY" -O binary "$BUILD_DIR/kernel.elf" "$BUILD_DIR/kernel.bin"

KERNEL_SIZE=$(wc -c < "$BUILD_DIR/kernel.bin" | tr -d '[:space:]')
KERNEL_SECTORS=$(( (KERNEL_SIZE + 511) / 512 ))

echo "[build] Assembling bootloader (kernel sectors: ${KERNEL_SECTORS})"
nasm -f bin source/boot/boot.asm -o "$BUILD_DIR/boot.bin" -DKERNEL_SECTORS="${KERNEL_SECTORS}"

echo "[build] Creating os-image.bin"
cat "$BUILD_DIR/boot.bin" "$BUILD_DIR/kernel.bin" > "$BUILD_DIR/os-image.bin"

IMAGE_SIZE=$(wc -c < "$BUILD_DIR/os-image.bin" | tr -d '[:space:]')
PAD=$(( (512 - (IMAGE_SIZE % 512)) % 512 ))
if [[ "$PAD" -gt 0 ]]; then
	dd if=/dev/zero bs=1 count="$PAD" >> "$BUILD_DIR/os-image.bin" 2>/dev/null
fi

echo "[build] Done: $BUILD_DIR/os-image.bin"
