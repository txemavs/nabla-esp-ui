"""Bounded compile-time form metadata."""
import json
import re

def validate(fields):
    if not isinstance(fields, list) or not 1 <= len(fields) <= 8:
        raise ValueError("forms requires 1..8 fields")
    seen = set()
    for f in fields:
        if not isinstance(f, dict):
            raise ValueError("A form field must be a mapping")
        kind = f.get("type")
        allowed = {"key", "label", "type", "initial"} | (
            {"min", "max", "step"} if kind == "number" else {"options"} if kind == "choice" else set())
        if kind not in {"number", "choice", "toggle"} or set(f) - allowed:
            raise ValueError("Unknown form type or field")
        key = f.get("key")
        if not isinstance(key, str) or not re.fullmatch(r"[a-z][a-z0-9_]*", key) or key in seen:
            raise ValueError("Form keys must be unique lowercase identifiers")
        seen.add(key)
        label = f.get("label")
        if not isinstance(label, str) or not label or len(label.encode()) > 96 or "\x00" in label:
            raise ValueError("Form labels require 1..96 UTF-8 bytes, without NUL")
        initial = f.get("initial")
        if kind == "toggle":
            if type(initial) is not bool:
                raise ValueError("Toggle initial must be boolean")
        elif kind == "number":
            values = [f.get("min"), f.get("max"), f.get("step", 1), initial]
            if any(not isinstance(x,int) or isinstance(x,bool) or not -(2**31) <= x < 2**31 for x in values):
                raise ValueError("Number bounds and initial must be int32")
            low, high, step, initial = values
            if step <= 0 or not low <= initial <= high or (initial-low)%step or (high-low)%step:
                raise ValueError("Invalid range, step or initial alignment")
        else:
            options = f.get("options")
            if not isinstance(options, list) or not 1 <= len(options) <= 8:
                raise ValueError("Choice requires 1..8 options")
            if any(not isinstance(x,str) or not x or len(x.encode())>96 or "\x00" in x for x in options):
                raise ValueError("Invalid choice label")
            if len(set(options)) != len(options) or not isinstance(initial,int) or isinstance(initial,bool) or not 0 <= initial < len(options):
                raise ValueError("Duplicate option or invalid initial index")
    return fields

def emit(fields):
    if fields:
        validate(fields)
    rows=[]
    for f in fields:
        kind=f["type"]
        low, high, step=(f["min"],f["max"],f.get("step",1)) if kind=="number" else (0,len(f["options"])-1 if kind=="choice" else 1,1)
        opts=", ".join(json.dumps(x,ensure_ascii=False) for x in f.get("options",[]))
        rows.append("{"+", ".join([json.dumps(f["key"]), json.dumps(f["label"],ensure_ascii=False),
            "Kind::"+kind.upper(),str(low),str(high),str(step),str(int(f["initial"])), "{"+opts+"}"])+"}")
    return 'namespace nabla_forms { const Field fields[] = {' + (", ".join(rows) or "{}") + '}; const int field_count = '+str(len(fields))+'; }'
