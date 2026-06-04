#!/usr/bin/env python3
"""Validate the Mohid-NG trilingual documentation entry points."""
from __future__ import annotations

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
LANGS = {
    "en-gb": ["Mohid-NG", "VoronoiMeshMaker", "UERJ", "IPRJ", "mohidng@hotmail", "github.com/mohidng"],
    "pt-br": ["Mohid-NG", "VoronoiMeshMaker", "UERJ", "IPRJ", "mohidng@hotmail", "github.com/mohidng"],
    "pt-pt": ["Mohid-NG", "VoronoiMeshMaker", "UERJ", "IPRJ", "mohidng@hotmail", "github.com/mohidng"],
}


def fail(message: str) -> None:
    print(f"[trilingual-docs] ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


def main() -> int:
    for lang, terms in LANGS.items():
        index = ROOT / "docs" / lang / "index.md"
        if not index.is_file():
            fail(f"missing language entry point: {index.relative_to(ROOT)}")
        text = index.read_text(encoding="utf-8")
        if not text.lstrip().startswith("#"):
            fail(f"language entry point has no Markdown title: {index.relative_to(ROOT)}")
        for term in terms:
            if term not in text:
                fail(f"missing term {term!r} in {index.relative_to(ROOT)}")

    print("[trilingual-docs] OK: en-gb, pt-br and pt-pt entry points are present and contain mandatory terms.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
