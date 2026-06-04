# Installation

This page is a placeholder for installation instructions.

The first public development target should support:

```text
C++20 or newer
CMake
Sphinx documentation build
VoronoiMeshMaker mesh package input
optional solver backends
HDF5 or NetCDF output, to be defined
```

For documentation only:

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements-docs.txt
sphinx-build -b html docs build/html
```
