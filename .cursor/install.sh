#!/usr/bin/env bash
# Idempotent Cloud Agent bootstrap for the nabla-esp-ui desktop simulator.
# Installs the SDL2 build toolchain, creates the pinned Python environment and
# warms the ESPHome build cache so the first simulator run is fast.
set -euo pipefail
cd "$(dirname "$0")/.."

export DEBIAN_FRONTEND=noninteractive

# System dependencies required to compile and run the SDL host simulator.
sudo apt-get update -qq
sudo apt-get install -y --no-install-recommends \
  libsdl2-dev \
  build-essential \
  python3-venv \
  python3-dev

# Pinned Python environment (ESPHome is pinned in requirements.txt).
if [[ ! -x .venv/bin/python ]]; then
  python3 -m venv .venv
fi
# shellcheck disable=SC1091
source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt

# Warm the ESPHome/PlatformIO build cache for the default simulator profile.
# This is durable state captured by the environment snapshot; later builds are
# incremental. Failure here should not block dependency setup.
esphome compile simulator/hello-world.yaml || \
  echo "WARN: simulator warm-compile failed; the agent can recompile on demand." >&2
