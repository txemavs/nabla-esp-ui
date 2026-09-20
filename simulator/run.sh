#!/usr/bin/env bash
# Builds and launches a selected SDL fixture so shared UI behavior can be inspected locally.
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
  encoder-list) config=simulator/encoder-list.yaml ;;
  tft160) config=simulator/tft160.yaml ;;
  tft240) config=simulator/tft240.yaml ;;
  tcall) config=simulator/tcall.yaml ;;
  tiny) config=simulator/tiny.yaml ;;
  readable) config=simulator/readable.yaml ;;
  composition) config=simulator/composition.yaml ;;
  information) config=simulator/information.yaml ;;
  password) config=simulator/password.yaml ;;
  control-panel) config=simulator/control-panel.yaml ;;
  control-panel-core2) config=simulator/control-panel-core2.yaml ;;
  *) echo "Usage: $0 [regular|portrait|tft240|tcall|tiny|readable|tft160|password|information|composition|control-panel|control-panel-core2]" >&2; exit 2 ;;
esac
exec esphome run "$config"
