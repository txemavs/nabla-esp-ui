# Communication buses

SPI, I2C and other bus configurations for hardware components.

## Available configurations

| File | Bus | Notes |
|------|-----|-------|
| jc3248w535cn-qspi.yaml | QSPI | Quad SPI for display |
| jc3248w535cn-i2c.yaml | I2C | Touch controller |

## Usage

Import via the board composer:

```yaml
packages:
  qspi_bus: !include bus/jc3248w535cn-qspi.yaml
  i2c_bus: !include bus/jc3248w535cn-i2c.yaml
```

## Board-specific pins

Bus configurations are often board-specific due to GPIO assignments.
Create new files for boards with different pin mappings rather than
parameterizing heavily. Name them by board (e.g. `nodemcu-32s-spi.yaml`).
