# Camera viewer (experimental)

Optional regular-profile LVGL JPEG snapshots with the shared header/footer.
Touch the image or video icon to toggle fullscreen. The Wi-Fi icon refreshes;
it is not a connectivity meter. Back first leaves fullscreen. No RTSP decoder.

Import components/camera-viewer/package.yaml AFTER packages/regular.yaml.
Set nabla_camera_count (default 3) and nabla_camera_route: a C++ expression
returning a zero-based slot or -1 outside camera routes. Use generated
nabla::routes::node_YOUR_KEY constants rather than translated titles or numeric
tree indices. Keep this mapping and your navigation in the private device YAML.

Import homeassistant.yaml once per camera with vars: binding_id (unique ID),
entity (camera entity), origin (trusted HTTPS HA origin without trailing slash)
and index (slot). The adapter checks the exact camera_proxy path for that entity
and receives its rotating image token through the encrypted native API.
Never publish these private bindings or tokens. TLS verification stays enabled;
sensitive HTTP and text sensor logs are muted.

For servers with the optional [Home Assistant camera cache](../../services/homeassistant/README.md),
set nabla_camera_use_cache: "true" in the device substitutions. All HA camera
bindings then use /api/nabla_camera/{entity}?token=...&size=view instead of the
full-size proxy, retaining the same rotating camera token. Enable only when
the service is installed and every bound camera is allowlisted. The default
is false, preserving compatibility with unmodified Home Assistant servers.

Only the selected source is fetched. nabla_camera_refresh_interval defaults to 1s. A single RGB565 image
(416x234 maximum, approximately 190 KiB) and a 32 KiB receive buffer are reused.
PSRAM/regular LCD targets only; compact/monochrome camera rendering is not supported.
Hosts need a CA certificate path and JPEGDEC's Linux build flag.

The footer shows small FPS text (12 px regular / 13 px Ubuntu Mono), measured
from completed image downloads/decodes, not LVGL redraws or requested refreshes.
The first frame shows -- FPS; subsequent inter-frame times use 25% exponential
smoothing. The rate falls when arrivals stop; after eight seconds the stale
message includes 0.0 FPS. Reopening a camera resets the measurement. Repeated
snapshots count as deliveries, not necessarily distinct camera exposures.
Waiting, error and stale states remain visible in the normal footer.
The module owns the nabla_view_refresh and nabla_back_intercept shell hooks.
Multiple views require an explicit root-level refresh and Back hook composition; see the light-card panel integration. The viewer contains
no installation address or entity; homeassistant.yaml is an optional adapter.

Actual images from three sources and fullscreen touch passed in SDL. The previous
private equivalent compiled and was flashed to JC3248W535CN; physical image
confirmation and a long running soak remain pending. Downloads can briefly delay
touch. Footer controls currently require touch; full U/D/ENTER delegation remains
pending. This experimental package does not complete the camera milestone.

## GitHub composition fragment

Merge into an existing private device YAML; preserve hardware, navigation,
Wi-Fi, encrypted API and OTA. The sample identifiers below are fictional.

    substitutions:
      nabla_ui_ref: REPLACE_WITH_TESTED_COMMIT
      nabla_camera_count: "1"
      nabla_camera_route: "id(nav_current)==nabla::routes::node_front_camera?0:-1"

    packages:
      library:
        url: https://github.com/txemavs/nabla-esp-ui
        ref: ${nabla_ui_ref}
        refresh: never
        files:
          - packages/regular.yaml
          - components/camera-viewer/package.yaml
          - path: components/camera-viewer/homeassistant.yaml
            vars:
              binding_id: front_camera_path
              entity: camera.example_front
              origin: https://home.example.invalid
              index: "0"

Declare a leaf with key front_camera in nabla_navigation.tree. Additional cameras
use distinct binding_id values and indices, with corresponding route mappings.

## Optional ESP32 background transport

nabla_camera_fetch is an opt-in ESP32/ESP-IDF component, requiring PSRAM.
It takes one or two runtime image IDs and exposes select(slot) and
request(slot, https_url). select(-1) invalidates pending results; request returns
false while busy or offline instead of building a queue. A single worker owns
the HTTP client and at most one request/result. HTTPS certificate verification
remains enabled, redirects are rejected, and completed connections are reused.
Compressed responses are capped at 128 KiB and allocated in PSRAM; network I/O
never calls LVGL. The main loop decodes a complete JPEG and invokes on_result
with slot and success. Tokens/URLs are not logged. Errors close the connection;
the next scheduled request may retry. This transport is for reduced snapshots,
not arbitrary full-resolution camera streams.

Configure images: [camera_image, camera_thumbnail] and on_result to execute
camera_finish/camera_error for slot 0 and camera_thumbnail_finish/
camera_thumbnail_error for slot 1. Override nabla_camera_download with a
request(0, selected_url) call. Select the current slot before camera_render.
The fallback online_image transport remains available to host simulators.
An idle persistent connection consumes memory until its next request or reboot.

## Passive thumbnail card

Import thumbnail.yaml after package.yaml. It requires the cache service and
uses size=icon from nabla_thumbnail_source (camera index, default 0).
nabla_thumbnail_active is a visibility expression; call camera_thumbnail_render
from the composed view hook. Override nabla_thumbnail_download to request slot
1 on the background transport, or retain the synchronous host fallback.
The source is 64x64 RGB565 (8 KiB) with a 4 KiB download buffer; the card draws
it at 32x32 in the icon area. No title or click action is supplied by the widget.
The consumer can bind a no-op command for the card. Error/stale images hide.

The light grid exposes nabla_light_card_override, a bool expression evaluated
before light state access. Return true after custom rendering to skip the
light renderer; i, node, rows, labels, w, h, gap and columns are in scope.
nabla_camera::thumbnail draws the passive image using that row's dimensions.
This prototype supports one thumbnail object and the regular color profile.
Compact/monochrome and multiple simultaneous thumbnail cards are not implemented.
