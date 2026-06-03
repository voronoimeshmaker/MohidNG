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
./build/debug/examples/Ex01_ReadMesh/Ex01_ReadMesh data/meshes/square_2x2.mngmesh
./build/debug/examples/Ex02_CellField/Ex02_CellField data/meshes/square_2x2.mngmesh
./build/debug/examples/Ex03_GradientReconstruction/Ex03_GradientReconstruction data/meshes/square_2x2.mngmesh
```

## Project contact

Email: mohidng@hotmail

GitHub organisation: https://github.com/mohidng
