# Codex prompt, Mohid-NG project start

You are working on the initial implementation of **Mohid-NG**, a new-generation environmental modelling platform based exclusively on Voronoi finite volumes.

Your task is to take the provided repository skeleton and turn it into a clean, buildable, testable first commit. Work conservatively. Do not over-engineer. The first goal is not a full environmental model. The first goal is a disciplined starting point.

## Project identity

Project name: Mohid-NG

GitHub organisation: `mohidng`

Contact email: `mohidng@hotmail`

Primary institutional context: Universidade do Estado do Rio de Janeiro, through Instituto Politécnico, IPRJ/UERJ.

## Non-negotiable architectural boundary

Mohid-NG shall use only Voronoi-based computational meshes.

All mesh-generation functionality belongs to **VoronoiMeshMaker**, not Mohid-NG. Do not implement the following inside Mohid-NG:

- Voronoi generation;
- point-generator redistribution;
- remeshing;
- adaptive mesh generation;
- mesh connectivity reconstruction;
- mesh-quality metric computation;
- mesh hierarchy construction;
- GIS-driven mesh preparation.

Mohid-NG consumes validated Voronoi mesh packages produced by VoronoiMeshMaker. Mohid-NG may validate that a package is internally consistent, but it must not become a mesh generator.

## Initial implementation scope

Implement and stabilise only the first bootstrap core:

1. CMake project structure;
2. C++20 build;
3. Google-style formatting files;
4. typed mesh indices;
5. 2-D Voronoi mesh view;
6. temporary bootstrap mesh reader;
7. cell-centred scalar fields;
8. weighted least-squares cell-gradient reconstruction;
9. examples;
10. tests;
11. minimal documentation.

The temporary text mesh format is only for bootstrapping. It is not the final Mohid-NG mesh package format. The production mesh package shall later be specified as a stable VoronoiMeshMaker-to-Mohid-NG contract, probably HDF5-based.

## Coding rules

Use C++20.

Follow the repository `.clang-format`, based on Google C++ style.

Use:

- namespace `mohidng`;
- `PascalCase` for types;
- `PascalCase` for functions where already used in the skeleton;
- `snake_case` for local variables;
- trailing underscore for private data members;
- `.h` and `.cc` file extensions;
- `#pragma once` only if already used consistently in the skeleton;
- clear public/private separation.

Avoid:

- inheritance and virtual functions in the numerical core;
- one-object-per-cell designs;
- pointer-linked mesh graphs;
- heap allocation inside hot loops;
- exposing PETSc, Trilinos, CGAL, GDAL, HDF5 or VoronoiMeshMaker internal types in public physical-model APIs;
- adding dependencies unless essential.

## What to do first

1. Inspect the repository tree.
2. Configure the project with CMake.
3. Build it.
4. Run CTest.
5. Fix any compile, warning or test failures.
6. Keep changes minimal and coherent.
7. Update documentation only where needed to match the actual build and examples.

Suggested commands:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --test-dir build/debug --output-on-failure
```

If presets fail in your environment, use an equivalent CMake invocation and document it.

## Acceptance criteria for this first task

The work is complete only if all of the following are true:

1. `cmake --preset debug` configures successfully.
2. `cmake --build --preset debug` builds successfully.
3. `ctest --test-dir build/debug --output-on-failure` passes.
4. `Ex01_ReadMesh` reads the bootstrap mesh and prints the correct entity counts.
5. `Ex02_CellField` creates a cell field and prints meaningful min/max values.
6. `Ex03_GradientReconstruction` reconstructs the gradient of a linear field.
7. The code remains dependency-light.
8. No mesh generation or remeshing is implemented in Mohid-NG.
9. README instructions match the actual commands.
10. The final response includes a concise list of files changed, tests run and any known limitations.

## Expected numerical behaviour

The bootstrap mesh is a simple 2-D four-cell Voronoi-like control-volume mesh. For the field

```text
phi(x, y) = x + 2 y
```

the cell-centred gradient should be approximately

```text
grad(phi) = (1, 2)
```

for all cells on the bootstrap mesh.

## Out of scope for this first task

Do not implement:

- hydrodynamics;
- free-surface equations;
- wetting and drying;
- scalar transport time integration;
- remeshing;
- PETSc or Trilinos integration;
- HDF5 mesh package reading;
- NetCDF output;
- VTU output;
- GIS integration;
- particles;
- water quality;
- sediments;
- parallel MPI partitioning.

These features are planned, but they are not part of the first bootstrap task.

## Important design direction for later

The first real milestone after this bootstrap is:

> Mohid-NG reads a validated Voronoi mesh package, allocates fields, computes finite-volume operators, validates conservation diagnostics and writes visualisable output.

The code should make that future direction easier, but it should not implement the full system now.
