# Build-time localization

Set ui_language to es or en in simulator/hello-world.yaml.
Locale packages provide matching tr_* substitutions. They are resolved in the
navigation YAML before the external component emits safely escaped C++ data.
Quotes, backslashes and newlines are supported. Runtime switching is pending.

```sh
source .venv/bin/activate
esphome -s ui_language en run simulator/hello-world.yaml
```

Run ./simulator/run.sh for the configured default. Add the same translation key
to both files and reference ${tr_key} in a node title or detail.
Glyph coverage is configured in theme/fonts.yaml. Unsupported locales fail
configuration rather than silently falling back.
