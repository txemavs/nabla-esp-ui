# Build-time localization

Set ui_language to es or en in simulator/hello-world.yaml.
The locale package supplies matching tr_* substitutions for tiles, node titles,
breadcrumbs and descriptions. Runtime language switching is not implemented.

To test English from the repository root:
```sh
source .venv/bin/activate
esphome -s ui_language en run simulator/hello-world.yaml
```

Run ./simulator/run.sh to return to the configured default.

Add the same key to both locale files, then reference ${tr_key} in YAML.
Translations are currently bound into the static node catalog by
navigation/translations.yaml. Keep strings safe for their C++ lambda context:
quotes, backslashes and newlines require escaping. A validated declarative
catalog/code-generation layer remains planned to remove this restriction.

Text glyphs are defined in theme/fonts.yaml. Additional languages may require
additional glyphs. Unsupported locale names fail configuration rather than
silently falling back.
