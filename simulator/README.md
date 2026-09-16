# Desktop simulator

The simulator compiles ESPHome for your computer and opens an SDL2 window.
It runs the actual LVGL YAML, including the shared theme and view.
No board, Home Assistant connection, or credentials are required.

## Setup on Ubuntu / Windows WSL

Use Ubuntu 24.04 with Python 3.12 and a graphical desktop.
On Windows, run these commands inside Ubuntu on WSL with WSLg enabled.

From the repository root:

```sh
sudo apt-get update
sudo apt-get install -y libsdl2-dev build-essential python3-venv
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

ESPHome is pinned in requirements.txt. The first build downloads additional
build dependencies and takes longer than subsequent incremental builds.

## Open the window

From the repository root:

```sh
./simulator/run.sh
```

The script activates the project environment, compiles, and launches the
480 x 320 landscape window. It defaults to the X11 SDL backend for WSLg.
The screen contains the original logo, **nabla.net ESP UI**, and **Iniciar**.

Click **Iniciar** to test touch input with the mouse. It logs
**Start requested** in the terminal; application navigation is not implemented yet.
Close the window to exit, or press Ctrl+C in the launching terminal.

From Windows PowerShell, for this checkout:

```powershell
wsl -d Ubuntu-24.04 -- bash -lc 'cd /home/txema/projects/nabla-esp-ui && ./simulator/run.sh'
```

Adjust the distribution and checkout path for your machine.

## Compile without opening a window

```sh
source .venv/bin/activate
esphome compile simulator/hello-world.yaml
```

## Reopen an already compiled window

From the repository root:

```sh
SDL_VIDEODRIVER=x11 simulator/.esphome/build/nabla-simulator/.pioenvs/nabla-simulator/program
```

This skips compilation and runs the last build. Run it from another terminal
to open another instance. Each instance has independent UI state.
Close running windows before rebuilding; launch them again to load changes.

## Change the screen

Edit screen_width and screen_height in simulator/hello-world.yaml, then run
the launcher again. The current view is designed for 480 x 320 landscape;
automatic small-screen layouts are future work.

- simulator/hello-world.yaml: entry point, dimensions, and package selection.
- simulator/hardware/sdl.yaml: host platform, virtual display, and mouse input.
- examples/hello-world/ui.yaml: reusable LVGL launch screen.
- theme/nabla.yaml: shared colors and logo resource.
- assets/nabla.jpg: unmodified brand artwork, resized at compile time.

Changing dimensions here changes the virtual screen's shape; physical display
rotation and touch transforms will belong to the real hardware profile.

## Troubleshooting

- **No window / display error:** run inside a graphical Linux session or WSLg.
  Check `echo "$DISPLAY"`. A headless SSH shell needs graphical forwarding.
- **SDL2 not found:** check `sdl2-config --version` and install libsdl2-dev.
- **Old content:** rebuild with the launcher; the compiled binary alone cannot
  pick up YAML changes.
- **Missing Python packages:** activate .venv and install requirements.txt.

Desktop execution checks UI configuration and behavior. It does not validate
ESP memory limits, performance, physical display drivers, or touch calibration.
