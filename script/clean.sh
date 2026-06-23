#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

# Generic artifact patterns for small systems projects.
FILE_PATTERNS=(
	'*.bin'
	'*.img'
	'*.iso'
	'*.o'
	'*.obj'
	'*.elf'
	'*.out'
	'*.lst'
	'*.map'
	'*.sym'
	'*.log'
)

DIR_PATTERNS=(
	'build'
	'out'
	'dist'
)

echo "[clean] Removing generated files..."
for pattern in "${FILE_PATTERNS[@]}"; do
	find . -type f -name "$pattern" -delete
done

echo "[clean] Removing common build directories..."
for dirname in "${DIR_PATTERNS[@]}"; do
	find . -type d -name "$dirname" -prune -exec rm -rf {} +
done

echo "[clean] Done."
