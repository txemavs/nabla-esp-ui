#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ ! -x .venv/bin/esphome ]]; then
  echo "Missing .venv. Follow simulator/README.md to install dependencies." >&2
  exit 1
fi
source .venv/bin/activate
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
case "${1:-regular}" in
  regular) config=simulator/hello-world.yaml ;;
  portrait) config=simulator/portrait.yaml ;;
  tft160) config=simulator/tft160.yaml ;;
  tiny) config=simulator/compact.yaml ;;
  readable) config=simulator/readable.yaml ;;
  composition) config=simulator/composition.yaml ;;
  information) config=simulator/information.yaml ;;
  password) config=simulator/password.yaml ;;
  control-panel) config=simulator/control-panel.yaml ;;
  *) echo "Usage: $0 [regular|portrait|tiny|readable|tft160|password|information|composition|control-panel]" >&2; exit 2 ;;
esac
exec esphome run "$config"
