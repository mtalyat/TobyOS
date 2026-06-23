#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$ROOT_DIR"

echo "[all] Cleaning..."
bash "${SCRIPT_DIR}/clean.sh"

echo "[all] Building..."
bash "${SCRIPT_DIR}/build.sh"

echo "[all] Running..."
bash "${SCRIPT_DIR}/run.sh"