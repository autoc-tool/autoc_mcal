#!/usr/bin/env python3
"""Remove GCC-only flags that break MSVC cl/link from Ninja files."""
import pathlib, re, shutil, sys
ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".")
PATTERNS = [
    r"-Wno-[A-Za-z0-9_=+-]+",
    r"-Werror(?:=[A-Za-z0-9_=+-]+)?",
    r"-f[A-Za-z0-9_=+-]+",
    r"-rdynamic",
    r"-m32",
    r"-m64",
]
rx = re.compile("|".join(f"(?:{p})" for p in PATTERNS))
changed = 0
for name in ("build.ninja", "rules.ninja"):
    path = ROOT / name
    if not path.exists():
        continue
    bak = ROOT / f"{name}.bak_before_msvc_sanitize"
    if not bak.exists():
        shutil.copy2(path, bak)
    text = path.read_text(encoding="utf-8", errors="surrogateescape")
    new = rx.sub("", text)
    new = re.sub(r"[ \t]{2,}", " ", new)
    if new != text:
        path.write_text(new, encoding="utf-8", errors="surrogateescape")
        changed += 1
        print(f"sanitized {path}")
    else:
        print(f"unchanged {path}")
print(f"done files_changed={changed}")