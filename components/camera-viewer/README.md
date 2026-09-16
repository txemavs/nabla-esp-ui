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

Only the selected source is fetched, every two seconds. A single RGB565 image
(416x234 maximum, approximately 190 KiB) and a 32 KiB receive buffer are reused.
PSRAM/regular LCD targets only; compact/monochrome camera rendering is not supported.
Hosts need a CA certificate path and JPEGDEC's Linux build flag.

Loading, waiting, error and stale states use the normal footer.
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
