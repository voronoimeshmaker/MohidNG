# First case

The first operational case should be deliberately small:

```text
fixed 2D Voronoi mesh
one cell-centred scalar field
one conservative flux operator
one gradient reconstruction operator
one boundary patch
one output file
one conservation diagnostic
```

The objective is not realism. The objective is to prove that the mesh package, field registry, operator layer, case file, output and validation diagnostics work together.
