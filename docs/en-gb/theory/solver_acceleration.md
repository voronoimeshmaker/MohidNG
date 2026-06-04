# Solver acceleration

Mohid-NG shall support a hierarchy of solver-acceleration techniques:

```text
algebraic multigrid
geometric multigrid, planned
block preconditioners
vertical line or block smoothers
domain decomposition
matrix-free kernels, planned
preconditioner reuse policies
```

The solver layer shall report setup time, solve time, iteration counts, residual reduction, memory usage and parallel efficiency.
