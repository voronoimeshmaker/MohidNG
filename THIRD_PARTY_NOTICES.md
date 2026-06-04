# Third-party notices

Mohid-NG is licensed under the Apache License, Version 2.0. This file records the intended policy for external software used by the project.

Mohid-NG should prefer external dependencies as system packages, CMake packages, package-manager dependencies, or optional backends. Third-party source code should not be copied into this repository unless there is a clear technical reason and the upstream licence, copyright notices and attribution requirements are preserved.

## Current documentation dependencies

| Component | Use | Licence notes |
| --- | --- | --- |
| Sphinx | Documentation build | BSD-style licence |
| MyST Parser | Markdown support for Sphinx | MIT licence |
| PyData Sphinx Theme | Documentation HTML theme | BSD-style licence |
| sphinx-design | Documentation UI components | MIT licence |
| sphinx-copybutton | Documentation code-copy buttons | MIT licence |
| sphinxcontrib-bibtex | Bibliography support | BSD-style licence |
| Breathe | Future API documentation bridge | BSD-style licence |
| Exhale | Future API documentation generation | BSD-style licence |

The Python documentation dependencies are listed in `requirements-docs.txt` and installed into a local virtual environment when building documentation locally.

## Current scientific/HPC dependencies

| Component | Current role | Licence notes | Mohid-NG policy |
| --- | --- | --- | --- |
| HDF5 | Block 1 Voronoi mesh package reader | BSD-style HDF5 licence | External system dependency; do not vendor; do not expose HDF5 types in public physical-model APIs. |

## Expected future scientific/HPC dependencies

| Component | Expected role | Licence notes | Mohid-NG policy |
| --- | --- | --- | --- |
| PETSc | Optional linear/nonlinear solver backend | BSD-2-Clause | Optional backend; do not expose PETSc types in public physical-model APIs. |
| Trilinos | Optional solver/preconditioner backend | Mixed, commonly BSD-3-Clause and some LGPL components | Optional backend; package selection must be reviewed before binary distribution. |
| NetCDF | Scientific data I/O | Permissive/BSD-style licence | External dependency; do not vendor. |
| VoronoiMeshMaker | Voronoi mesh-package producer | Project-specific; to be reviewed | Separate producer tool; Mohid-NG consumes validated packages. |
| CGAL | VoronoiMeshMaker implementation dependency | Mixed open-source/commercial options | Must remain outside Mohid-NG core and public APIs. |
| GDAL | GIS workflows | MIT/X-style licence | Use through external tools/backends only; avoid coupling in core model APIs. |

## Compatibility rules

- Keep the Mohid-NG core under Apache-2.0.
- Keep third-party dependencies external unless vendoring is explicitly approved.
- Preserve upstream copyright notices and licence files for any copied or adapted code.
- Isolate optional solver/I/O integrations behind backend targets such as `MohidNG::PetscBackend` or `MohidNG::TrilinosBackend` when they are added.
- Avoid GPL-only dependencies in the Mohid-NG core.
- Review LGPL dependencies case by case, especially for binary redistribution and linking obligations.
- Do not expose PETSc, Trilinos, HDF5, GDAL, CGAL or VoronoiMeshMaker implementation types in public physical-model APIs.

This file is informational and should be updated whenever a new external dependency is added.
