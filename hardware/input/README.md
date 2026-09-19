# Input devices

Encoder, button and other input configurations for physical controls.

## Available configurations

| File | Type | Notes |
|------|------|-------|
| encoder-nodemcu-32s.yaml | Rotary encoder + buttons | A/B/push/K0 for compact shell |

## Usage

Import via the board composer:

```yaml
packages:
  input: !include input/encoder-nodemcu-32s.yaml
```

## Compact shell integration

Encoder inputs call the compact shell navigation methods directly:
- Clockwise: `nabla::compact_shell.move(1)`
- Anticlockwise: `nabla::compact_shell.move(-1)`
- Push: `nabla::compact_shell.activate()`
- K0/Back: `nabla::compact_shell.back()`

For devices with different pin assignments, create a new YAML file
with the appropriate GPIO numbers.
