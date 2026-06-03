# Contributing to Mohid-NG

This bootstrap repository is intended to establish the initial project structure. Contributions shall preserve the following rules:

1. Keep Mohid-NG independent from VoronoiMeshMaker internals.
2. Do not implement mesh generation, remeshing or mesh adaptation in Mohid-NG.
3. Keep public headers free from external solver, GIS and mesh-generator implementation types.
4. Use C++20.
5. Follow the Google-based `.clang-format` configuration.
6. Add tests for every new numerical operator.
7. Add an example for every new user-facing feature.
8. Keep examples executable through CTest.
