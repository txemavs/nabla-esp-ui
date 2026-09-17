# Nabla Camera Cache (experimental)

Optional Home Assistant custom integration. Produces small baseline JPEGs on
request, without polling when idle. No firmware changes are needed to install
the service; existing viewers continue using their original URL until migrated.

Copy custom_components/nabla_camera_cache to Home Assistant's
/config/custom_components/, add the following configuration (or a package), run
the Home Assistant configuration check, and restart Core in a maintenance window:

    nabla_camera_cache:
      cameras:
        - camera.example_front
        - camera.example_workbench
      refresh_interval: 1.0
      max_stale: 8.0
      quality: 70

GET /api/nabla_camera/camera.example_front?size=icon returns 64x64, with black
letterboxing. size=view (the default) fits inside 480x480 without distortion or
upscaling. Landscape 16:9 yields 480x270; portrait 9:16 yields 270x480.
One capture generates both variants.

Authentication is inherited from Home Assistant's CameraView: use an
Authorization: Bearer header, or the existing rotating camera image token in
the token query parameter. Camera tokens grant image access: keep them private,
use HTTPS, and never publish token-bearing links or put images in /config/www.
The entity must also be explicitly allowlisted in this integration.
This uses the same authorization semantics as HA's native camera proxy.

Each camera has one in-flight refresh shared across all readers. Cold readers
join it; warm readers receive the previous complete image while it updates.
Only complete immutable image pairs replace the cache. There is no frame
history or disk write. At most two cameras capture/render concurrently.
An idle service starts no capture. Existing camera integrations may separately
maintain streams or their own caches; this integration does not control those.

refresh_interval is the minimum shared cache lifetime, not a guaranteed frame
rate. It accepts 0.1–60 seconds; start at 1 second and measure the source and
device before trying 10 fps. Identical source frames can produce identical
ETags even after a refresh. X-Nabla-Age is time since the source was obtained,
not necessarily its exposure timestamp. X-Nabla-Stale identifies an older
served frame. Frames older than max_stale are not served; source failure
returns 503 when no usable frame remains, with a 2-second retry backoff.
Clients can revalidate with If-None-Match (304); authentication still runs first.

Limits: 32 configured cameras, 10 MiB source bytes, 16 million source pixels,
128 KiB maximum per encoded variant, JPEG quality 30–85. Pillow transforms run
in HA's executor, never in its UI/event loop. Memory holds only current outputs
plus bounded in-flight source/processing buffers; concurrent HTTP responses
can temporarily retain previous immutable output bytes. No arbitrary source
URLs, requested dimensions, or per-client image caches.

Cache engine tests cover concurrent readers, no idle work, stale/failed
sources, cancellation, dimensions and baseline JPEG output. The HA adapter
targets Core 2026.9.1; it uses CameraView, an internal API that must be checked
when upgrading HA. Live installation results are recorded privately, not in
public examples. A standalone HTTP/RTSP source adapter for other hosts remains
future work; the cache/resize engine is independent of Home Assistant.
