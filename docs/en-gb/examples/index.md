# Examples

The examples are executable documentation. Each entry below states what the program does, how it is run from the build directory, and exposes the source file used by the web documentation.

## Current executable examples

| Example | Purpose | Run target |
| --- | --- | --- |
| `exe_read_mesh` | Read a bootstrap text mesh and print mesh counts. | `make run_read_mesh` |
| `exe_cell_field` | Create and inspect scalar fields attached to cells, faces and nodes. | `make run_cell_field` |
| `exe_gradient_reconstruction` | Compute the gradient of a linear cell field on the bootstrap mesh. | `make run_gradient_reconstruction` |
| `exe_read_voronoi_package` | Read and validate the HDF5 Voronoi mesh package contract. | `make run_read_voronoi_package` |
| `exe_rectangular_domain_gradient` | Read `RectangularDomainGradient.ini`, build a rectangular 2-D case and compute gradients. | `make run_rectangular_domain_gradient` |

## Ex01, read mesh

Purpose:
: Reads the bootstrap mesh format used during the initial blocks and reports the number of nodes, cells, faces and boundary patches.

Command from the build directory:

```bash
make run_read_mesh
```

Source file:
: {download}`exe_read_mesh.cc <../../../examples/Ex01_ReadMesh/exe_read_mesh.cc>`

```{literalinclude} ../../../examples/Ex01_ReadMesh/exe_read_mesh.cc
:language: cpp
:linenos:
```

## Ex02, cell field

Purpose:
: Creates scalar fields at different mesh locations and checks that sizes, locations and initial values are consistent with the mesh.

Command from the build directory:

```bash
make run_cell_field
```

Source file:
: {download}`exe_cell_field.cc <../../../examples/Ex02_CellField/exe_cell_field.cc>`

```{literalinclude} ../../../examples/Ex02_CellField/exe_cell_field.cc
:language: cpp
:linenos:
```

## Ex03, gradient reconstruction

Purpose:
: Builds a linear scalar field on the bootstrap mesh and reconstructs its cell gradients with the default gradient method.

Command from the build directory:

```bash
make run_gradient_reconstruction
```

Source file:
: {download}`exe_gradient_reconstruction.cc <../../../examples/Ex03_GradientReconstruction/exe_gradient_reconstruction.cc>`

```{literalinclude} ../../../examples/Ex03_GradientReconstruction/exe_gradient_reconstruction.cc
:language: cpp
:linenos:
```

## Ex04, read Voronoi package

Purpose:
: Opens a VoronoiMeshMaker HDF5 package, validates the Mohid-NG mesh package contract and prints mesh metadata.

Command from the build directory:

```bash
make run_read_voronoi_package
```

Source file:
: {download}`exe_read_voronoi_package.cc <../../../examples/Ex04_ReadVoronoiPackage/exe_read_voronoi_package.cc>`

```{literalinclude} ../../../examples/Ex04_ReadVoronoiPackage/exe_read_voronoi_package.cc
:language: cpp
:linenos:
```

## Ex05, rectangular-domain gradient

Purpose:
: Reads a YAML-formatted case file named `RectangularDomainGradient.ini`, creates a simple rectangular 2-D domain, builds an associated structured bootstrap mesh, defines a linear scalar field and computes its gradient.

Command from the build directory:

```bash
make run_rectangular_domain_gradient
```

Source file:
: {download}`exe_rectangular_domain_gradient.cc <../../../examples/Ex05_RectangularDomainGradient/exe_rectangular_domain_gradient.cc>`

Input file:
: {download}`RectangularDomainGradient.ini <../../../examples/Ex05_RectangularDomainGradient/RectangularDomainGradient.ini>`

```{literalinclude} ../../../examples/Ex05_RectangularDomainGradient/exe_rectangular_domain_gradient.cc
:language: cpp
:linenos:
```

### RectangularDomainGradient.ini

```{literalinclude} ../../../examples/Ex05_RectangularDomainGradient/RectangularDomainGradient.ini
:language: yaml
:linenos:
```
