# Licensing and third-party dependencies

Mohid-NG is distributed under the Apache License, Version 2.0. The SPDX identifier is `Apache-2.0`.

This choice is intended to keep the core usable in academic, public-sector and industrial workflows while preserving an explicit patent grant. The project should remain dependency-light in the core and should avoid embedding third-party source code.

## Policy

- The Mohid-NG core is Apache-2.0.
- Documentation and small project-authored examples follow the repository licence unless a file states otherwise.
- External dependencies should be discovered through CMake, package managers or system installations.
- Optional HPC/scientific integrations should be isolated behind backend targets.
- GPL-only dependencies should not enter the core.
- LGPL dependencies require case-by-case review before binary redistribution.
- Upstream copyright notices and licence texts must be preserved for copied or adapted third-party code.

## Expected dependency treatment

PETSc, HDF5 and NetCDF are expected to fit the project as external dependencies with permissive licences. Trilinos requires package-level review because the Trilinos project contains packages under different licences, including BSD-style and LGPL components.

VoronoiMeshMaker remains a separate mesh-package producer. Any dependency needed for mesh generation, remeshing, generator redistribution, mesh-quality metrics, mesh hierarchy construction or GIS-driven mesh preparation belongs to VoronoiMeshMaker or external workflow tools, not to the Mohid-NG core.

See `THIRD_PARTY_NOTICES.md` at the repository root for the current third-party dependency policy and notice table.
