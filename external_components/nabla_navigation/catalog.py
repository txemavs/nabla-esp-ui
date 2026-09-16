import json
import re

INFO_FIELDS = {"model": 1, "wifi": 2, "ip": 3, "version": 4, "uptime": 5}

ACTIONS = {"open", "dark", "light", "wifi_demo", "forms_demo"}

def flatten(tree):
    result = []
    seen = set()
    def visit(node, parent, depth):
        if not isinstance(node, dict):
            raise ValueError("Each node must be a mapping")
        unknown = set(node) - {"key", "title", "icon", "detail", "action", "children", "bg_dark", "bg_light", "icon_dark", "icon_light", "info"}
        if unknown:
            raise ValueError("Unknown node fields: " + ", ".join(sorted(unknown)))
        key = node.get("key")
        if not isinstance(key, str) or not re.fullmatch(r"[a-z][a-z0-9_]*", key):
            raise ValueError("Node key must use lowercase letters, digits and underscores")
        if key in seen:
            raise ValueError("Duplicate node key: " + key)
        seen.add(key)
        if depth > 16 or len(result) >= 128:
            raise ValueError("Tree limit: 16 levels and 128 nodes")
        for field in ("title", "icon", "detail"):
            value = node.get(field, "")
            if not isinstance(value, str) or "\x00" in value:
                raise ValueError(field + " must be a string without NUL")
        if not node.get("title"):
            raise ValueError("Node requires a nonempty title: " + key)
        children = node.get("children", [])
        if not isinstance(children, list) or len(children) > 8:
            raise ValueError("Each node supports at most eight children")
        action = node.get("action", "open")
        if action not in ACTIONS:
            raise ValueError("Unknown action: " + str(action))
        if action != "open" and (parent < 0 or children):
            raise ValueError("Actions must be non-root leaves")
        info = node.get("info")
        if info is not None and (not isinstance(info, str) or info not in INFO_FIELDS):
            raise ValueError("Unknown information field")
        if info is not None and (children or parent < 0 or action != "open" or node.get("detail")):
            raise ValueError("Information fields must be read-only leaves without static detail")
        colors = {}
        for field, default in (("bg_dark", 0x000000), ("bg_light", 0xFFFFFF), ("icon_dark", 0xFFFFFF), ("icon_light", 0x000000)):
            value = node.get(field, default)
            if not isinstance(value, int) or isinstance(value, bool) or not 0 <= value <= 0xFFFFFF:
                raise ValueError(field + " must be a 24-bit RGB integer")
            colors[field] = value
        index = len(result)
        result.append(dict(key=key, title=node["title"], parent=parent,
                           icon=node.get("icon", ""), detail=node.get("detail", ""),
                           action=action, info=info, **colors))
        for child in children:
            if depth == 0 and (not isinstance(child, dict) or not child.get("icon")):
                raise ValueError("Desktop entries require an icon")
            visit(child, index, depth + 1)
    visit(tree, -1, 0)
    return result

def emit(tree):
    nodes = flatten(tree)
    rows = []
    for n in nodes:
        values = [json.dumps(n["title"], ensure_ascii=False), str(n["parent"]),
                  json.dumps(n["detail"], ensure_ascii=False),
                  json.dumps(n["icon"], ensure_ascii=False),
                  str({"open": 0, "dark": 1, "light": 2, "wifi_demo": 3, "forms_demo": 4}[n["action"]]),
                  str(n["bg_dark"]), str(n["bg_light"]), str(n["icon_dark"]), str(n["icon_light"]), str(INFO_FIELDS.get(n["info"], 0))]
        rows.append("{" + ", ".join(values) + "}")
    return ("namespace nabla { const Node nodes[] = {"
            + ", ".join(rows) + "}; const int count = "
            + str(len(nodes)) + "; }")
