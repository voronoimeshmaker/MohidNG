# Mohid-NG

Mohid-NG is a proposed new-generation environmental modelling platform based on Voronoi finite volumes. The first implementation target is deliberately narrow: read a validated Voronoi mesh package, allocate fields, compute basic finite-volume operators, run minimal examples, and build a disciplined testable codebase.

## Current bootstrap scope

This repository skeleton intentionally implements only a small bootstrap core:

- project structure;
- CMake build;
- Google-style formatting configuration;
- typed mesh indices;
- a temporary text mesh reader;
- cell fields;
- weighted least-squares gradient reconstruction;
- three runnable examples;
- unit and integration tests.

The temporary `.mngmesh` format is not the final mesh contract. The final mesh package shall be produced by VoronoiMeshMaker, most likely through a documented HDF5-based contract.

## Architectural boundary

VoronoiMeshMaker owns every mesh-related implementation concern:

- Voronoi generation;
- generator-point redistribution;
- remeshing;
- mesh adaptation;
- connectivity reconstruction;
- mesh-quality metrics;
- mesh hierarchy construction;
- GIS-driven mesh preparation.

Mohid-NG consumes validated Voronoi mesh packages and uses them for physical modelling, finite-volume operators, fields, solvers, diagnostics, I/O and validation.

## Licence

Mohid-NG is licensed under the Mozilla Public License 2.0 (MPL-2.0). See the `LICENSE` file for details. Third-party dependency policy and notices are tracked in `THIRD_PARTY_NOTICES.md`.

## Build

HDF5 development libraries are required for the Block 1 Voronoi mesh package reader. On Ubuntu, install `libhdf5-dev`.

```bash
cmake --preset debug
cmake --build --preset debug
ctest --test-dir build/debug --output-on-failure
```

## Examples

```bash
./build/debug/exe_read_mesh data/meshes/square_2x2.mngmesh
./build/debug/exe_cell_field data/meshes/square_2x2.mngmesh
./build/debug/exe_gradient_reconstruction data/meshes/square_2x2.mngmesh

# Or from build/debug:
make run_read_mesh
make run_cell_field
make run_gradient_reconstruction
```

## Adding examples and tests

CMake discovers examples and tests automatically. Add new examples as `examples/.../exe_name.cc` and new tests as `tests/.../tst_name.cc`.

From `build/debug`, `exe_case1.cc` can be built and run with `make run_case1`. Test files follow the same rule: `tst_gradient.cc` creates `make run_gradient`.

## Documentation

```bash
python3 tools/check_design_system.py
python3 tools/check_trilingual_docs.py
build/docs-venv/bin/sphinx-build -b html docs docs/html -W --keep-going
```

## Error, Exception and Trace Policy

Mohid-NG has its own error, exception and flow-trace subsystem. Errors use flexible textual codes, not enums, and support localised messages in `pt-br`, `pt-pt` and `en-gb`. The default language is `pt-br`. `Error` and `MohidNgException` are separate concepts. Debug builds capture the execution-flow trace at the throw site; release builds compile this trace mechanism as a no-op through `NDEBUG`.

## Web Publication Options

The documentation site is generated with Sphinx under docs/html. Publication options are:

- ReadTheDocs: best for versioned documentation and automatic builds from GitHub.
- GitHub Pages: simple static hosting directly from a repository branch or Actions artifact.
- Institutional server: useful when UERJ/IPRJ wants to host an official project page.
- Local HTML only: acceptable during Block 0 while the public home and logos are still being refined.

## Project contact

Email: mohidng@hotmail

GitHub organisation: https://github.com/mohidng

