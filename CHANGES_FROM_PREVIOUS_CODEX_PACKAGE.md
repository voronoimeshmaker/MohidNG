# Changes from the previous Codex package

This repository skeleton supersedes the earlier Mohid-NG Codex package that already contained the C++20 core bootstrap, the provisional mesh reader, field containers, gradient reconstruction, and the separated Error, Exception and Logger/Trace subsystems.

Codex must treat this as an update, not as a greenfield replacement. Preserve the previous working C++ build and tests, then add the following Block 0 documentation requirements.

## New Block 0 requirement: Documentation Design System Bootstrap

The Block 0 completion gate now includes a web/documentation design bootstrap.

New or expanded deliverables:

- `docs/conf.py`, Sphinx configuration;
- `docs/index.md`, root language selector;
- `docs/en-gb/index.md`, British English entry point;
- `docs/pt-br/index.md`, Brazilian Portuguese entry point;
- `docs/pt-pt/index.md`, European Portuguese entry point;
- `docs/design_system.md`, formal design-system page;
- `docs/_static/css/mohid-ng.css`, official CSS tokens and components;
- `docs/_static/logo/`, Mohid-NG placeholder logo assets;
- `docs/_static/logos/`, institutional logo placeholder area;
- `requirements-docs.txt`, documentation dependencies;
- `.readthedocs.yaml`, future ReadTheDocs configuration;
- `tools/check_design_system.py`, dependency-free design-system validator;
- `tools/check_trilingual_docs.py`, dependency-free trilingual documentation validator;
- `.github/workflows/ci.yml`, now validates design tokens, trilingual entry points and Sphinx build.

## Design decisions added

- Visual theme: Mohid-NG Coastal Scientific Theme.
- Main text font: Inter.
- Code and terminal font: JetBrains Mono.
- Light palette includes Deep Ocean `#073B4C`, Tidal Teal `#118AB2`, Foam `#F4FAFC`, Mist `#E8F1F5`, Sediment Amber `#C9822B`, Graphite `#1F2933`.
- Dark palette includes Abyss `#071923`, Muted Cyan `#5BC0D8`, Foam Text `#DDEFF4`, Error Coral `#E07A5F`.
- Documentation is trilingual: `en-gb`, `pt-br`, `pt-pt`.
- The current participating institution is UERJ, through IPRJ/UERJ.
- Contact is `mohidng@hotmail`.
- GitHub organisation/account is `github.com/mohidng`.

## Constraints unchanged

- Mohid-NG uses only Voronoi mesh packages for simulation.
- All mesh generation, adaptation, remeshing, generator redistribution, connectivity reconstruction, mesh-quality control and mesh hierarchy construction belong to VoronoiMeshMaker.
- VoronoiMeshMaker may depend on CGAL.
- Mohid-NG must not depend on CGAL directly.
- Error, Exception and Logger/Trace remain separate subsystems.
- Error codes are flexible textual codes, not enums.
- Diagnostic messages are trilingual, with `pt-br` as the default language.
- Flow tracing is enabled in Debug and compiled as no-op in Release through `NDEBUG`.

## Required checks

After applying this package, the following must pass:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --test-dir build/debug --output-on-failure
python tools/check_design_system.py
python tools/check_trilingual_docs.py
sphinx-build -b html docs build/docs/html -W --keep-going
```
