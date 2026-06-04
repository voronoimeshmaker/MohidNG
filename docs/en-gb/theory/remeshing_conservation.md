# Remeshing and conservation

Remeshing and mesh adaptation are implemented in VoronoiMeshMaker. Mohid-NG shall, however, define the conservation requirements that must be satisfied when fields are transferred between mesh packages.

For a conserved scalar, the transfer should satisfy

$$
\sum_Q \phi_Q^{\mathrm{new}} V_Q^{\mathrm{new}}
= \sum_P \phi_P^{\mathrm{old}} V_P^{\mathrm{old}}.
$$

```{admonition} Validation gate
:class: validation-gate
No remeshing workflow shall be accepted without conservation diagnostics for every conserved field involved in the transfer.
```
