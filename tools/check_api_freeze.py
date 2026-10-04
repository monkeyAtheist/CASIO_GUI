#!/usr/bin/env python3
from pathlib import Path
import hashlib
import sys

ROOT = Path(__file__).resolve().parents[1]
BASELINE = ROOT / "docs" / "API_BASELINE_V1.sha256"

def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()

expected = {}
for raw in BASELINE.read_text(encoding="utf-8").splitlines():
    raw = raw.strip()
    if not raw:
        continue
    sha, rel = raw.split(None, 1)
    expected[rel.strip()] = sha

failed = False

for rel, sha in expected.items():
    path = ROOT / rel
    if not path.exists():
        print(f"MISSING  {rel}")
        failed = True
        continue

    actual = digest(path)

    if actual != sha:
        print(f"CHANGED  {rel}")
        print(f"  expected {sha}")
        print(f"  actual   {actual}")
        failed = True
    else:
        print(f"OK       {rel}")

if failed:
    print("\nPublic API baseline drift detected.")
    print("Review the change under the v1 compatibility policy before release.")
    sys.exit(1)

print("\nCASIO GUI v1 public API baseline: OK")
