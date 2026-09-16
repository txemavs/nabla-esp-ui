#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ ! -x .venv/bin/esphome ]]; then
  echo "Missing .venv. Follow simulator/README.md to install dependencies." >&2
  exit 1
fi
source .venv/bin/activate
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
exec esphome run simulator/hello-world.yaml
