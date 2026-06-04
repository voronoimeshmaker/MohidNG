#!/usr/bin/env python3
"""Validate Mohid-NG Block 0 documentation design tokens.

This script is intentionally dependency-free. It does not render Sphinx; it
checks that the repository contains the mandatory design-system contract agreed
for Block 0.
"""
from __future__ import annotations

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
CSS = ROOT / "docs" / "_static" / "css" / "mohid-ng.css"
DESIGN = ROOT / "docs" / "design_system.md"
LOGOS = ROOT / "docs" / "_static" / "logos" / "README.md"

REQUIRED_CSS_TOKENS = {
    "Deep Ocean": "#073B4C",
    "Harbour Blue": "#0B5C75",
    "Tidal Teal": "#118AB2",
    "Estuary Cyan": "#47B5D1",
    "Foam": "#F4FAFC",
    "Mist": "#E8F1F5",
    "Sediment Amber": "#C9822B",
    "Graphite": "#1F2933",
    "Abyss": "#071923",
    "Muted Cyan": "#5BC0D8",
    "Foam Text": "#DDEFF4",
    "Error Coral": "#E07A5F",
    "Inter": "Inter",
    "JetBrains Mono": "JetBrains Mono",
}

REQUIRED_DESIGN_TERMS = [
    "Mohid-NG Coastal Scientific Theme",
    "Deep Ocean",
    "Tidal Teal",
    "Foam",
    "Abyss",
    "Inter",
    "JetBrains Mono",
    "VoronoiMeshMaker responsibility",
    "Validation gate",
    "UERJ",
    "IPRJ",
]

REQUIRED_CSS_CLASSES = [
    ".mohidng-hero",
    ".mohidng-workflow",
    ".institution-logo",
    ".logo-card",
    ".admonition-voronoimeshmaker-responsibility",
    ".admonition-validation-gate",
    ".admonition-conservation-requirement",
]


def fail(message: str) -> None:
    print(f"[design-system] ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


def require_file(path: Path) -> str:
    if not path.is_file():
        fail(f"missing required file: {path.relative_to(ROOT)}")
    return path.read_text(encoding="utf-8")


def main() -> int:
    css = require_file(CSS)
    design = require_file(DESIGN)
    require_file(LOGOS)

    for label, token in REQUIRED_CSS_TOKENS.items():
        if token not in css and token not in design:
            fail(f"missing design token {label!r} ({token!r})")

    for term in REQUIRED_DESIGN_TERMS:
        if term not in design:
            fail(f"missing term in docs/design_system.md: {term!r}")

    for class_name in REQUIRED_CSS_CLASSES:
        if class_name not in css:
            fail(f"missing CSS class: {class_name}")

    print("[design-system] OK: required design tokens, fonts, classes and documentation were found.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
