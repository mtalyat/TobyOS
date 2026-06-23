#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

if ! command -v pacman >/dev/null 2>&1; then
  echo "[setup] pacman not found. Run this script from an MSYS2 shell (MINGW64)."
  exit 1
fi

if [[ -z "${MSYSTEM:-}" || "${MSYSTEM}" != "MINGW64" ]]; then
  echo "[setup] Warning: this script is intended for MSYS2 MINGW64."
  echo "[setup] Current MSYSTEM='${MSYSTEM:-unset}'."
  echo "[setup] Continue anyway..."
fi

echo "[setup] Installing build and run dependencies..."
pacman -S --needed \
  mingw-w64-x86_64-clang \
  mingw-w64-x86_64-lld \
  mingw-w64-x86_64-llvm \
  nasm \
  mingw-w64-x86_64-qemu

# Ensure current shell can resolve mingw tools.
export PATH="/mingw64/bin:$PATH"

if [[ -f "$HOME/.bashrc" ]]; then
  if ! grep -Fq 'export PATH="/mingw64/bin:$PATH"' "$HOME/.bashrc"; then
    echo "[setup] Adding /mingw64/bin to ~/.bashrc"
    echo 'export PATH="/mingw64/bin:$PATH"' >> "$HOME/.bashrc"
  fi
else
  echo "[setup] Creating ~/.bashrc and adding /mingw64/bin"
  echo 'export PATH="/mingw64/bin:$PATH"' > "$HOME/.bashrc"
fi

echo "[setup] Verifying required tools..."
REQUIRED_TOOLS=(
  clang++
  ld.lld
  llvm-objcopy
  nasm
  qemu-system-i386
)

MISSING=0
for tool in "${REQUIRED_TOOLS[@]}"; do
  if command -v "$tool" >/dev/null 2>&1; then
    echo "[setup]   ok: $tool -> $(command -v "$tool")"
  else
    echo "[setup]   missing: $tool"
    MISSING=1
  fi
done

if [[ "$MISSING" -ne 0 ]]; then
  echo "[setup] One or more tools are still missing."
  echo "[setup] Open a new MINGW64 shell and re-run this script."
  exit 1
fi

echo "[setup] Environment is ready."
echo "[setup] Next steps:"
echo "[setup]   bash script/all.sh"
