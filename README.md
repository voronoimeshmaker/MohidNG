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

## Build

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
## Project contact

Email: mohidng@hotmail

GitHub organisation: https://github.com/mohidng

