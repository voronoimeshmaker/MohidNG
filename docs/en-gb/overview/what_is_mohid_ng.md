# What is Mohid-NG?

Mohid-NG is a new-generation environmental modelling platform designed around Voronoi finite-volume discretisation. Its purpose is to support hydrodynamics, transport, water-quality processes, sediments, Lagrangian particles and coupled coastal, riverine and watershed applications.

Mohid-NG is not a direct translation of the original MOHID source code. The guiding rule is:

```text
Port equations, not indices.
```

The project shall preserve the physical and operational ambition of the original MOHID ecosystem while replacing the structured grid contract with a Voronoi-native finite-volume contract based on cells, faces, fields, patches, layers and particles.
