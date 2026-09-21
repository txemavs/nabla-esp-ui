# Display and Interaction Catalog

Compare display formats and input capabilities when choosing a Nabla hardware
profile. A resolution may appear more than once because shape and controls vary.
The reference matrix is not a claim of support; tested targets are recorded
separately. Confirm the exact variant against its manufacturer documentation
before choosing drivers or pins. Follow the [adoption workflow](DEVICE-ADOPTION.md)
to add a target; this register owns the summary of verification evidence.

## Displays and Controls


Dimensions are in pixels; width and height swap when the display rotates. Input capabilities depend on the exact device variant. See Tested Devices below for public compositions and recorded verification results.

| Resolution | Display type | Built-in interaction | Examples / reference |
| --- | --- | --- | --- |
| 128×32 | Rectangular monochrome OLED | None on the display module | [SSD1306 OLED](https://learn.adafruit.com/monochrome-oled-breakouts) |
| 128×64 | Rectangular monochrome OLED | None on the display module | [SSD1306 OLED](https://learn.adafruit.com/monochrome-oled-breakouts) |
| 128×128 | Square color LCD, 0.85″ | Button beneath the screen; not a touchscreen | [M5Stack AtomS3](https://docs.m5stack.com/en/core/AtomS3) |
| 160×80 | Rectangular color LCD, wristband | Single touch input on the original model; not a touchscreen | [Original LILYGO T-Wristband input example](https://github.com/Xinyuan-LilyGO/T-Wristband/blob/master/examples/T-Wristband-DRV2605/T-Wristband-DRV2605.ino) |
| 135×240 | Rectangular color LCD | Onboard buttons on the original model | [Original LILYGO T-Display](https://lilygo.cc/products/t-display) |
| 170×320 | Rectangular color LCD, 1.9″ | User and BOOT buttons; no touchscreen on the basic variant | [Basic LILYGO T-Display-S3](https://wiki.lilygo.cc/products/t-display-series/t-display-s3/) |
| 172×320 | Rectangular color LCD, 1.47″ | Module-dependent; touch variants exist | Exact module reference pending verification |
| 240×240 | Square color LCD, 1.54″ | Touchscreen | [LILYGO T-Watch S3](https://wiki.lilygo.cc/products/t-watch-series/t-watch-s3/) |
| 240×240 | Round color LCD, 1.28″ | None on the non-touch LCD module | GC9A01 family; exact module reference pending verification |
| 240×280 | Rectangular color LCD, 1.69″ | None on the reference LCD module | [Waveshare 1.69inch LCD Module](https://www.waveshare.com/1.69inch-LCD-Module.htm) |
| 390×390 | Color AMOLED, 2.04″ | Touchscreen and rotary encoder with push button | [LILYGO T-Encoder Pro](https://wiki.lilygo.cc/products/t-encoder-series/t-encoder-pro/) |
| 480×320 | Rectangular color LCD, 3.5″ | Depends on the module and touch controller | [Waveshare resistive-touch example](https://www.waveshare.com/3.5inch-HDMI-LCD.htm); HDMI format reference, not a direct ESPHome connection example |
| 480×480 | Square color LCD, 4″ | Touchscreen | [Seeed SenseCAP Indicator](https://wiki.seeedstudio.com/Sensor/SenseCAP/SenseCAP_Indicator/Get_started_with_SenseCAP_Indicator/) |


## Tested Devices

Public records of physical targets and host fixtures. Link reusable hardware,
example YAML and evidence here; keep actual installation menus, deployment pins,
credentials and issue details that identify a site in the private installation.

| Device / profile | Display and input | Verified scope | Remaining checks / issues | Public YAML and evidence |
| --- | --- | --- | --- | --- |
| JC3248W535CN | 3.5-inch IPS; native 320×480, logical 480×320; touch | Initial physical launcher/touch navigation, USB installation, OTA/API and initial primary Wi-Fi scan/save/rollback checks documented | M3 usability, power-cycle/recovery and 24-hour reconnect/navigation soak remain open; see watchdog investigation for rotation follow-up | [Device](../devices/jc3248w535cn.yaml), [editable consumer example](../examples/github/panel.yaml), [hardware notes](../hardware/jc3248w535cn.md), [watchdog investigation](platform/M3-WATCHDOG.md) |
| NodeMCU-32S + ST7735 | Native 128×160, logical 160×128; encoder, push and K0 Back | Physical color-launcher photo, OTA installation, encrypted API reconnection and MQTT state reception documented | Physical legibility, encoder direction, brightness/Cancel, captive-AP recovery and extended reconnect/soak checks remain open | [Hardware](../hardware/nodemcu-32s-st7735.yaml), [compact example](../examples/mqtt-lights/compact.yaml), [test evidence](../hardware/nodemcu-32s-st7735.md) |
| T-Call + external SSD1309 OLED | 2.42-inch 128×64 monochrome SPI; encoder/push | OTA 0.7.0-ui and encrypted API reconnection verified; owner confirmed operation and overall appearance on 2026-09-19 | Individual MQTT actuation, Wi-Fi credential rollback, power-cycle and soak checks remain open | [Device](../devices/tcall-ssd1309.yaml), [hardware and evidence](../hardware/tcall-ssd1309.md) |
| Tiny / readable OLED profiles | 128×64 monochrome host viewport; simulated sequential input | Host rendering and shared navigation/form baseline | Physical OLED validation pending; a host build does not validate panel contrast, wiring or input hardware | [Tiny fixture](../simulator/tiny.yaml), [readable fixture](../simulator/readable.yaml), [profile notes](../profiles/README.md) |

The compact MQTT example is an illustrative fragment, not a standalone firmware.
An online Device Builder entry proves availability only, not display/navigation
correctness. Record test dates, exact library revision and tested behavior when
adding evidence; retain detailed private YAML links in the installation's own
test register. See [private installations](PRIVATE-INSTALLATIONS.md).

T-Watch 2020 has a [public device composition](../devices/lilygo-twatch-2020.yaml)
and a 240x240 profile. Its mirror firmware was installed by OTA and a real 240x240 RGB332
frame was inspected on 2026-09-21; remote touch input remains unavailable. T-Wristband is a different target:
confirm its exact revision before assigning drivers or pins. Other reference
rows are not verified Nabla targets merely because their resolution matches.

## Interpreting Input Capabilities

- **Touchscreen:** reports positions on the visible surface.
- **Touch button:** reports one input, not screen coordinates.
- **Rotary encoder with push button:** provides rotation and activation. It is
  one option for a non-touch display; buttons are another.
- **Buttons:** the profile must define selection, activation, Back and Cancel.
  A boot/reset control is not automatically a general-purpose navigation input.
- **No local input:** requires passive operation or explicitly supported remote
  input. Do not expose actions that require missing controls.

Resolution alone determines neither navigation nor legibility. Physical size,
visible area, orientation and shape matter. Round and square 240×240 displays
need different content bounds. Profiles own this adaptation; applications keep
their meaning and obey shared appearance rules.

## Future Columns

Add physical diagonal/visible area, controller and bus, profile coverage,
supported size choices, or measured resource/refresh limits when they help
select hardware. Keep examples last and split rows for materially different
input variants. See the [shared UI contract](UI-CONSISTENCY.md).

## Optional mirror evidence (updated 2026-09-21)

| Target | Mirror evidence | Remaining qualification |
| --- | --- | --- |
| T-Call 128×64 | Physical mono1 frame and remote encoder actions checked | Recovery and soak |
| Kit1 160×128 | USB write verified; real RGB332 frame inspected | Multi-client heap and latency |
| Large panel 480×320 | OTA succeeded; real RGB332 frame inspected | Runtime rotation and sustained load |
| T-Watch 240×240 | OTA and real RGB332 frame verified on 2026-09-21 (0429cdf) | Remote touch, runtime rotation and soak |

See the [mirror component](../external_components/nabla_display_mirror/README.md)
for memory costs, color quantization and explicit limits. This table does not
claim that every device currently runs the latest repository revision.
