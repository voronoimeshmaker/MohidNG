# Finite volumes on Voronoi meshes

The generic cell-integrated conservation statement is

$$
\frac{\mathrm{d}}{\mathrm{d}t}\int_{V_P}\phi\,\mathrm{d}V + \sum_{f\in\partial P} F_f A_f = \int_{V_P} S_\phi\,\mathrm{d}V.
$$

Mohid-NG shall operate on Voronoi cells, faces, face normals, cell volumes, face areas, boundary patches and field locations supplied by VoronoiMeshMaker.
