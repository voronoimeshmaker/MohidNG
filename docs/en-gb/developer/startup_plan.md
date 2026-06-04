# Startup plan

## Block 0, project foundation

Goal: create a buildable, testable and documented repository.

Gate: CMake configures, examples build, tests run, documentation skeleton exists.

## Block 1, Voronoi mesh contract

Goal: read a minimal Voronoi mesh package and validate topology and geometry.

Gate: a simple 2-D Voronoi mesh is read, cell-face-cell connectivity is validated, boundary patches are recognised.

## Block 2, fields and operators

Goal: allocate fields and compute basic finite-volume operators.

Gate: constant fields have zero gradient, linear fields have exact gradients on the bootstrap mesh, conservation tests pass.

## Block 3, first physical equation

Goal: run a minimal scalar transport problem on a fixed Voronoi mesh.

Gate: case reaches final time, mass conservation is reported, output is visualisable.
