#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

if [[ ! -f os-image.bin ]]; then
	echo "[run] os-image.bin not found. Running build first..."
	"${SCRIPT_DIR}/build.sh"
fi

echo "[run] Launching QEMU"
qemu-system-i386 -drive format=raw,file=os-image.bin
