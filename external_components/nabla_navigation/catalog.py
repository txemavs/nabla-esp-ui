import json
import re

ACTIONS = {"open", "dark", "light"}

def flatten(tree):
    result = []
    seen = set()
    def visit(node, parent, depth):
        if not isinstance(node, dict):
            raise ValueError("Each node must be a mapping")
        unknown = set(node) - {"key", "title", "icon", "detail", "action", "children"}
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
            raise ValueError("Theme actions must be non-root leaves")
        index = len(result)
        result.append(dict(key=key, title=node["title"], parent=parent,
                           icon=node.get("icon", ""), detail=node.get("detail", ""),
                           action=action))
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
                  str({"open": 0, "dark": 1, "light": 2}[n["action"]])]
        rows.append("{" + ", ".join(values) + "}")
    return ("namespace nabla { const Node nodes[] = {"
            + ", ".join(rows) + "}; const int count = "
            + str(len(nodes)) + "; }")
