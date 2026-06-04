# Startup plan

## Block 0, project foundation

Status: completed on 2026-06-04.

Goal: create a buildable, testable and documented repository.

Gate: CMake configures, examples build, tests run, documentation skeleton exists, the documentation design system is validated, trilingual documentation entry points are present, the Sphinx site builds without warnings, and the repository has an explicit licence and third-party dependency policy.

Completion record:

- CMake Debug and Release presets configure successfully.
- Debug and Release builds complete successfully.
- CTest passes in Debug and Release.
- Examples follow the exe_* convention and can be run from the build directory through make run_* targets.
- Tests follow the tst_* convention and can be run from the build directory through make run_* targets.
- The documentation design system check passes.
- The trilingual documentation check passes.
- The Sphinx HTML documentation builds with -W --keep-going.
- Apache-2.0 is the selected project licence.
- THIRD_PARTY_NOTICES.md records the dependency policy.

## Block 1, Voronoi mesh contract

Status: completed on 2026-06-04.

Goal: read a minimal Voronoi mesh package and validate topology and geometry.

Gate: a simple 2-D Voronoi mesh is read, cell-face-cell connectivity is validated, boundary patches are recognised, mesh metadata is checked, and invalid packages produce clear diagnostics without turning Mohid-NG into a mesh generator.

Completion record:

- HDF5 is the current Block 1 mesh-package dependency and is linked through CMake.
- A minimal 2-D Voronoi mesh package fixture is stored in data/meshes/square_2x2.mngpkg.h5.
- Mohid-NG reads schema version, producer, dimension, CRS note, nodes, cells, faces and boundary patches from the package.
- The reader validates topology, geometry, boundary patch consistency and metadata before constructing MeshView.
- Invalid permanent fixtures cover negative cell area and out-of-range face owner diagnostics.
- The plain-text bootstrap mesh is rejected by the HDF5 package reader.
- Debug and Release CTest runs pass with the Block 1 reader test.
- The new example follows the exe_* convention and runs through make run_read_voronoi_package.
- The new test follows the tst_* convention and runs through make run_voronoi_mesh_package.
- Mohid-NG remains a package consumer and does not implement Voronoi generation, remeshing or connectivity reconstruction.

## Block 2, fields and operators

Status: planned.

Goal: allocate fields and compute basic finite-volume operators.

Gate: constant fields have zero gradient, linear fields have exact gradients on the bootstrap mesh, conservative operator tests pass, and field/operator APIs remain independent of PETSc, Trilinos, HDF5, GDAL, CGAL and VoronoiMeshMaker implementation types.

## Block 3, first physical equation

Status: planned.

Goal: run a minimal scalar transport problem on a fixed Voronoi mesh.

Gate: case reaches final time, mass conservation is reported, output is visualisable, and the case can be reproduced from documented input files.
