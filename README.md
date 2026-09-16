# ∇ nabla-esp-ui

A modular UI library for **ESPHome + LVGL**, with a shared visual style, reusable components, and navigation adapted to different displays and input methods.

**Define what you want to control; reuse how it looks and works.**

> **Status: initial design.** This README describes the planned architecture and initial scope. The packages, profiles, and applications below are implementation goals. There is no installable release or validated hardware configuration yet.

## The idea

Build panels without redesigning buttons, bars, menus, and controls for every device.

A menu containing eight applications should appear as eight icons on a touchscreen or eight list entries on a small display with a rotary encoder. Navigation structure and functionality are shared; each profile provides a suitable presentation.

The same lighting application should work across rooms, buildings, and Home Assistant installations by changing entity mappings rather than copying its implementation.

## What each panel configures

- **Hardware:** board, display, touchscreen, encoder, and backlight.
- **UI profile:** format, density, and input method.
- **Navigation:** available applications, ordering, and favorite shortcuts.
- **Entities:** lights, sensors, and actions for that installation.
- **Theme:** shared styling with optional overrides.

Configuration will initially use **native ESPHome packages, includes, and variables**. A custom generator is not required. The public configuration syntax will be established through the first compilable examples.

## Library layers

### Theme

Shared colors, typography, icons, spacing, dimensions, and visual states. Compact, regular, and large variants should maintain a common identity.

### Visual components

Building blocks such as icon buttons, bars, indicators, cards, headers, and menu rows.

Each component is defined once, accepts parameters, and avoids assumptions about a particular display or entity. A bar should look and behave consistently wherever it is used.

### Applications

Functional modules built from library components: lighting, sensors, scenes, climate, media, and settings.

Each application declares its dependencies and groups its data connections, actions, and presentations. Installation-specific entity mappings remain outside the module.

### Navigation

A shared contract for opening applications, going back, returning home, selecting, and confirming.

The initial touch profile will provide a desktop with up to eight shortcuts and a global menu revealed from a corner, with a visible alternative entry point. The menu will offer access to the desktop, settings, and favorite applications.

The encoder profile will use structured menus: rotate to select, press to enter, and a configurable back action.

### Profiles

Presentation depends on resolution, orientation, available space, and input method—not just physical screen size.

A lighting application may use cards and detail controls on a touchscreen, and a list with sequential adjustments on a small display. It will share functionality and configuration wherever practical.

Each format will have an appropriate composition rather than shrinking a full desktop to fit every display.

### Hardware

Physical drivers and connections remain separate from the UI.

The **JC3248W535CN** is the first target device. Its exact configuration and compatibility will be verified during implementation.

## Planned structure

Directories will be added as their modules are implemented:

```text
theme/          Colors, styles, fonts, and icons
components/     Parameterized visual building blocks
apps/           Applications and their presentations
navigation/     Shared menus and actions
profiles/       Composition by format and input method
hardware/       Board and peripheral configurations
examples/       Complete panels and entity mappings
simulator/      Host configurations with SDL2 and demo data
docs/catalog/   Module documentation and examples
```

## Module contract

- Each instance has unique identifiers resolved during configuration.
- Adding another instance must not require copying scripts or editing module internals.
- Each module declares its dependencies and required parameters.
- Removing a module should remove its associated logic without leaving dangling references.
- Visual components must not hardcode installation-specific Home Assistant entities.
- Applications define how unknown, unavailable, and pending states are handled.
- Profiles share navigation actions even when their presentation differs.
- Native ESPHome and LVGL actions are preferred; additional C++ must be limited in scope and documented.
- Examples keep credentials in secret files excluded from the repository.

ESPHome resolves IDs and composition at compile time. The library will build on that model without relying on runtime widget discovery.

## Component catalog

Each component or application will have an entry documenting:

- Name, purpose, and status: experimental or validated.
- Parameters, defaults, and identifiers.
- Dependencies and compatible profiles.
- A minimal inclusion example.
- A screenshot or demonstration when available.
- Completed checks and known limitations.

The catalog will distinguish available modules from planned work.

## Desktop development

The planned development environment is **ESPHome host + SDL2**: a desktop window will run the same UI includes used on the device.

This will allow us to:

- Review styling and layouts at different resolutions.
- Test touch interaction with a mouse.
- Map keyboard keys to navigation actions for menu testing.
- Use mock data to exercise states and transitions.
- Run component examples without flashing a board.

Windows development is expected to use WSL with graphical support. Reproducible setup instructions and an ESPHome version will be established when the first example is validated.

YAML changes require recompilation. Desktop execution does not validate ESP memory usage, performance, or physical drivers; builds and tests on real hardware remain necessary.

## Initial scope

- [ ] Pin an ESPHome version and provide a reproducible host + SDL2 example.
- [ ] Define the initial theme and basic visual components.
- [ ] Define navigation and application registration contracts.
- [ ] Present the same menu as an icon desktop and an encoder-driven list.
- [ ] Implement lighting and settings applications.
- [ ] Prepare and validate the JC3248W535CN hardware profile.
- [ ] Document the first modules in the catalog.

Eight desktop positions do not imply eight completed applications. Shortcuts will reflect the configured modules.

### First milestone: Hello World

Start with a minimal desktop example before building the application library:

- Open an SDL2 window with configurable dimensions.
- Render a shared LVGL view containing “Hello, nabla!”.
- Include a button that changes a label to verify mouse input and UI updates.
- Run without a physical board, Home Assistant, or credentials.
- Keep desktop display/input configuration separate from reusable UI.
- Document setup and a single command to build and run the example.

This milestone is planned, not yet implemented.

### Success criterion

Add, repeat, or remove a control and reuse an application in another profile without editing component internals. Expected changes should be concentrated in panel composition, hardware, and entity mappings.

## Repository language

Documentation, code comments, identifiers, and development instructions use English. User-facing interface localization can be added separately.

## References

- [ESPHome LVGL](https://esphome.io/components/lvgl/)
- [LVGL widgets](https://esphome.io/components/lvgl/widgets/)
- [LVGL layouts](https://esphome.io/components/lvgl/layouts/)
- [ESPHome packages](https://esphome.io/components/packages/)
- [LVGL cookbook](https://esphome.io/cookbook/lvgl/)
- [SDL2 host display](https://esphome.io/components/display/sdl/)
- [ESPHome UI Kit](https://github.com/mplogas/esphome-ui-kit)
- [ESPHome Modular LVGL Buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)

These projects serve as design references. Any reused code must preserve its applicable license and attribution.
