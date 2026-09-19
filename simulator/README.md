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
The desktop opens immediately and its toolbar logo spins once in place.
The 4-column, 2-row launcher contains Settings, Communications, Photos, Music,
Cameras, Weather, Lights and Sensors (translated by the selected UI locale).
Settings opens Connections (then Wi-Fi), Language, About and Appearance. Sensors
opens folders and sensor leaves. Breadcrumbs show the route; X returns to the parent and the triangle
returns to the parent; at the root it toggles tiles/list. Other applications remain placeholders.
These are navigation demonstrations, not implemented applications.
The UI currently uses Spanish labels; source documentation remains English.

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
use the separate tiny/readable profiles for 128x64.

- simulator/hello-world.yaml: entry point, dimensions, and package selection.
- simulator/hardware/sdl.yaml: host platform, virtual display, and mouse input.
- examples/hello-world/ui.yaml: splash, startup transition, and page shell.
- navigation/launcher.yaml: menu entries and grid positions.
- components/menu-tile.yaml: shared tile appearance and open action.
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

## Input responsiveness

The SDL driver in the pinned ESPHome version processes one queued event per
main loop. The simulator package sets the host loop interval to 1 ms and touch
polling to 5 ms to reduce mouse-event backlog and missed short presses.
These settings are host-only and must not be copied into device profiles blindly.
Mouse input and nested back navigation have been exercised under WSLg.
See navigation/README.md for the current tree and its limitations.

## Keyboard and touch

Focus the simulator window. Up/Down selects the previous/next item (wrapping
at the ends), Enter opens it, Escape goes back, Home returns to the desktop.
The selected border is white; other actionable borders are medium gray. Touch selects
and opens through the same action, after 120 ms of visual feedback.

The shared toolbar and focus behavior are described in navigation/README.md.
Set ui_language to es or en; see locales/README.md for CLI overrides.

Current presentation contract: focus changes only borders, never icon, text or
background colors. Text follows the dark/light foreground; launcher icon colors are independent.
The current toolbar title is bold and excluded from focus; ancestors remain regular clickable text.
The header has no divider. Startup spins the
same toolbar logo in place for one second; there is no separate splash page.
Set nabla_monochrome: "true" to hide unselected control borders. This is a
presentation option, not validation of physical OLED hardware.

Root triangle toggles tile/list. The circular-arrow button cycles 90-degree
rotation, reachable via Up/Down and Enter. The SDL window remains physically
480x320; its contents rotate as on a fixed physical panel. See navigation/README.md.

For the larger platform design and proposed keyboard/128x64 fixtures, see
[the platform roadmap](../docs/platform/ROADMAP.md). The compact profiles are separate compositions; do not create them by shrinking
the regular example. Compact forms use the M2 character selector and shared form controller.

## Password keyboard preview

Run the native LVGL keyboard fixture in a separate 480x320 window:

```sh
source .venv/bin/activate
SDL_VIDEODRIVER=x11 esphome run simulator/password.yaml
```

Click letters, shift/case, symbols, backspace and the native confirm/cancel keys.
Up/Down traverses the keyboard buttons; Enter activates the selected button.
Escape cancels and clears the field. Confirm also clears it and reports demo
completion. Neither path connects to Wi-Fi, stores input, or logs its contents.
This fixture previews the native keyboard; it is not yet the integrated settings
editor, a Wi-Fi credential validator, or the compact OLED character picker.
The stock keyboard has its built-in character repertoire; translated labels
do not imply an accented keyboard layout. Use a dummy password for the preview.

For English, use `esphome -s ui_language en run simulator/password.yaml`.
Source: examples/password/ui.yaml. Fonts and colors follow the shared theme.

## Integrated manual Wi-Fi form

Open the main demo with ./simulator/run.sh, then
Ajustes > Conexiones > Wi-Fi (Settings > Connections > Wi-Fi in English).
Enter an SSID and password manually. Test connection validates the draft locally;
it does not change the host network or persist credentials.
Cancel, X, triangle, breadcrumbs, Escape and Home discard the draft on departure.
Touch fields to switch the keyboard target, or use Up/Down and Enter.
The keyboard Ready key advances to password, then validates.
See [the form contract](../components/wifi-form/README.md) for supported inputs,
validation limits and pending real Wi-Fi/Nabla Net adapters.

## M1 profile windows

From the repository root:
```sh
./simulator/run.sh regular
./simulator/run.sh portrait
./simulator/run.sh tiny
./simulator/run.sh readable
```

Each command compiles and opens its own window. tiny/readable are native 128x64;
portrait is a native 320x480 window. They use the same menu catalog.
Up/Down/Enter and Escape navigate. At compact root, select the triangle after
the eight options (or touch it) to switch between fullscreen icon and text list.
Tiny has only two root views: fullscreen icon or text-only list (no icon glyphs).
There is no intermediate 2x2 tile grid on 128x64 displays. A parent triangle/Back row
remains reachable without a physical Escape key.
See [profiles](../profiles/README.md) and [M1 evidence](../docs/platform/M1-VERIFICATION.md).
Compact Wi-Fi editing now uses the M2 character selector; see ../components/forms/README.md.

## M2 Wi-Fi workflow

In regular, portrait, tiny or readable, open Settings > Connections > Wi-Fi.
Regular offers Scan beside Connect/Cancel, opening a normal/empty/error scenario
picker. Compact exposes all three fixtures in its list. Choose Invitados Demo to try success without
typing a password; choose Demo sin servicio and an 8-character dummy password
to try failure. Other protected fixtures accept any valid-length dummy password.
Both scan and connection are simulations; there are no network side effects.

In the compact field editor, use Up/Down/Enter for Done, Delete, Cancel, the
character group and individual characters. Escape cancels only the field edit;
Back from the form discards all data. See ../components/forms/README.md.

## M2 controls gallery

Open Settings > Controls (demo) in any profile. The fields are configured in
examples/hello-world/navigation.yaml under nabla_navigation.forms.
Edit numbers with Minus/Plus/Accept/Cancel, select an operating mode and toggle
notifications. These are local example values, not real device settings.

Use Up/Down/Enter throughout; Left/Right is an optional numeric/choice shortcut.
Save asks for confirmation with Cancel focused first. Cancelling a field restores
its previous value; cancelling the form discards all its changes. Confirmed
values remain during this process and reset when the program restarts.
The shared toolbar/ESC returns from an editor/dialog before leaving the form.

The integrated Wi-Fi keyboard includes ASCII and Spanish accents. Set
nabla_keyboard_latin: "false" to test the unmodified native keyboard.
Passwords stay masked at all times. The standalone password preview keeps its
original stock keyboard; it is an isolated earlier fixture.

See [forms](../components/forms/README.md) and
[M2 evidence](../docs/platform/M2-VERIFICATION.md) for coverage and limitations.

## Information preview

Run ./simulator/run.sh information, then Settings > Information.
Wi-Fi combines network and IP; open it for gateway, netmask and signal.
Model opens chip, display technology, resolution and color capability.
All preview network values are synthetic and labeled as a demo. The regular
simulator reports unavailable network data instead. No device upload is involved.

Settings > Appearance contains a dark-mode checkbox row and a font-family row.
Touch or ENTER toggles either in place; UP/DOWN moves between them. The text
family switches between Ubuntu Mono (default) and DejaVu Sans; icons stay unchanged.

Appearance also exposes toggle_borders: enabled by default, with idle borders
#404040 in dark mode and #BFBFBF in light mode. With borders disabled, controls
use inverse foreground/background for focus and retain sequential/touch access.
LVGL menus, toolbar controls, forms, Wi-Fi fields and keyboard use the shared
control_style.h helper. Compact profiles use binary fill inversion.
The setting changes decoration, not the display color capability or font.
Physical e-paper rendering/refresh remains unverified; this is a host UI preview.

## Device-owned composition

Run ./simulator/run.sh composition. Edit simulator/composition.yaml to change
its complete navigation tree and forms. This imports reusable components without
importing an example menu, matching the recommended private Builder structure.

## Control Panel demo

```sh
./simulator/run.sh control-panel         # 480×320 regular
./simulator/run.sh control-panel-core2   # 320×240 Core2-sized (dense)
```

Opens a dense grid control panel with two synthetic site sections (Villa/Nave).
Use UP/DOWN to navigate cells, LEFT/RIGHT to switch sections, ENTER to toggle
or press. Toggle cells show on/off state with color feedback; momentary cells
fire press events; sensor cells display read-only values in a bottom row.

The Core2 demo matches M5Stack Core2 320×240 form factor with dense mode:
smaller cells, tighter spacing, single-line text with ellipsis truncation.

All data is local mock state. Toggle/momentary callbacks log to the console.
See [Control Panel](../components/control-panel/README.md) for integration.

## T-Call OLED preview

Run ./simulator/run.sh tcall for a complete 128x64 monochrome composition with
local light toggles, Wi-Fi, appearance and device information. All light actions
and Wi-Fi operations are simulated. See [the fixture notes](../examples/tcall/README.md).
Normal-mode selection uses the same pointer in menu rows, Wi-Fi and forms;
inverse mode remains available. The preview raises the body font from 11 to 12 px; the header is 13 px high with an 11 px title; icon size stays unchanged.
