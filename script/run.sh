#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

BUILD_DIR="build"
IMAGE_PATH="$BUILD_DIR/os-image.bin"

if [[ ! -f "$IMAGE_PATH" ]]; then
	echo "[run] $IMAGE_PATH not found. Running build first..."
	"${SCRIPT_DIR}/build.sh"
fi

echo "[run] Launching QEMU"
qemu-system-i386 -drive format=raw,file="$IMAGE_PATH"
