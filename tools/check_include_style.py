#!/usr/bin/env python3
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIRS = ("include", "src", "tests", "examples")
SOURCE_SUFFIXES = {".h", ".hpp", ".cc", ".cpp"}
STD_HEADERS = {
    "algorithm", "array", "bitset", "cassert", "cctype", "cerrno", "cfenv", "cfloat", "charconv",
    "chrono", "cinttypes", "climits", "clocale", "cmath", "codecvt", "compare", "complex", "concepts",
    "condition_variable", "coroutine", "csetjmp", "csignal", "cstdarg", "cstddef", "cstdint",
    "cstdio", "cstdlib", "cstring", "ctime", "deque", "exception", "execution", "filesystem",
    "format", "forward_list", "fstream", "functional", "future", "initializer_list", "iomanip", "ios",
    "iosfwd", "iostream", "istream", "iterator", "latch", "limits", "list", "locale", "map",
    "memory", "memory_resource", "mutex", "new", "numbers", "numeric", "optional", "ostream",
    "queue", "random", "ranges", "ratio", "regex", "scoped_allocator", "semaphore", "set", "shared_mutex",
    "source_location", "span", "sstream", "stack", "stdexcept", "stop_token", "streambuf", "string",
    "string_view", "strstream", "syncstream", "system_error", "thread", "tuple", "type_traits",
    "typeindex", "typeinfo", "unordered_map", "unordered_set", "utility", "valarray", "variant", "vector",
    "version",
}
INCLUDE_RE = re.compile(r"^#include\s+([<\"])([^>\"]+)([>\"])")
EXPECTED_GROUPS = ("standard", "external", "mohidng")


def classify(header: str) -> str:
    if header.startswith("MohidNG/"):
        return "mohidng"
    first = header.split("/", 1)[0]
    if first in STD_HEADERS:
        return "standard"
    return "external"


def include_blocks(lines: list[str]) -> list[list[tuple[int, str, str, str]]]:
    blocks: list[list[tuple[int, str, str, str]]] = []
    current: list[tuple[int, str, str, str]] = []
    for lineno, line in enumerate(lines, 1):
        match = INCLUDE_RE.match(line)
        if match:
            current.append((lineno, match.group(1), match.group(2), line.rstrip("\n")))
            continue
        if current:
            blocks.append(current)
            current = []
    if current:
        blocks.append(current)
    return blocks


def check_file(path: Path) -> list[str]:
    errors: list[str] = []
    text = path.read_text(encoding="utf-8")
    if '#include "MohidNG/' in text:
        errors.append(f"{path}: MohidNG includes must use angle brackets")
    for block in include_blocks(text.splitlines(keepends=True)):
        grouped: list[tuple[int, str, str, str, str]] = [
            (lineno, opener, header, raw, classify(header)) for lineno, opener, header, raw in block
        ]
        group_order = [group for *_prefix, group in grouped]
        positions = [EXPECTED_GROUPS.index(group) for group in group_order]
        if positions != sorted(positions):
            first_line = block[0][0]
            errors.append(f"{path}:{first_line}: include groups must be standard, external, MohidNG")
        for group in EXPECTED_GROUPS:
            headers = [header for _lineno, _opener, header, _raw, item_group in grouped if item_group == group]
            if headers != sorted(headers, key=str.lower):
                first = next(lineno for lineno, _opener, _header, _raw, item_group in grouped if item_group == group)
                errors.append(f"{path}:{first}: {group} includes must be alphabetical")
        for lineno, opener, header, _raw, group in grouped:
            if group == "mohidng" and opener != "<":
                errors.append(f"{path}:{lineno}: use #include <{header}> for MohidNG headers")
    return errors


def main() -> int:
    errors: list[str] = []
    for dirname in SOURCE_DIRS:
        for path in sorted((ROOT / dirname).rglob("*")):
            if path.suffix in SOURCE_SUFFIXES:
                errors.extend(check_file(path))
    if errors:
        print("\n".join(errors))
        return 1
    print("[include-style] OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
