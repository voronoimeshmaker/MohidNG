# Voronoi mesh package

The Voronoi mesh package is the contract between VoronoiMeshMaker and Mohid-NG. VoronoiMeshMaker produces and validates the mesh package; Mohid-NG reads it, checks internal consistency and uses it for finite-volume modelling.

```{admonition} VoronoiMeshMaker responsibility
:class: voronoimeshmaker
Voronoi generation, generator redistribution, remeshing, mesh adaptation, connectivity reconstruction, mesh-quality metrics, mesh hierarchy construction and GIS-driven mesh preparation belong to VoronoiMeshMaker. Mohid-NG must not implement these operations.
```

## Physical container

Block 1 introduces a minimal HDF5 package with this schema identifier:

```text
mohidng.voronoi_mesh_package/0.1
```

The current fixture is:

```text
data/meshes/square_2x2.mngpkg.h5
```

Two permanent invalid fixtures exercise reader diagnostics:

```text
data/meshes/square_2x2_negative_area.mngpkg.h5
data/meshes/square_2x2_bad_owner.mngpkg.h5
```

The bootstrap text file `square_2x2.mngmesh` remains only as a temporary test/readability aid. It is not the production package format.

## Root attributes

| Attribute | Type | Meaning |
| --- | --- | --- |
| `schema_version` | string | Must be `mohidng.voronoi_mesh_package/0.1` for the Block 1 reader. |
| `producer` | string | Must be `VoronoiMeshMaker`. |
| `dimension` | integer | Currently only `2` is supported. |
| `coordinate_reference_system` | string | CRS description or local-coordinate statement. |
| `note` | string | Human-readable package note. |

## Datasets

### `/nodes`

| Dataset | Shape | Type | Meaning |
| --- | --- | --- | --- |
| `/nodes/id` | `(num_nodes)` | int64 | Zero-based contiguous node ids. |
| `/nodes/xy` | `(num_nodes, 2)` | float64 | Node coordinates. |

### `/cells`

| Dataset | Shape | Type | Meaning |
| --- | --- | --- | --- |
| `/cells/id` | `(num_cells)` | int64 | Zero-based contiguous cell ids. |
| `/cells/centre` | `(num_cells, 2)` | float64 | Cell-centre coordinates. |
| `/cells/area` | `(num_cells)` | float64 | Positive 2-D cell areas. |

### `/faces`

| Dataset | Shape | Type | Meaning |
| --- | --- | --- | --- |
| `/faces/id` | `(num_faces)` | int64 | Face ids. |
| `/faces/owner` | `(num_faces)` | int64 | Owner cell id; must be in range. |
| `/faces/neighbour` | `(num_faces)` | int64 | Neighbour cell id, or `-1` for boundary faces. |
| `/faces/centre` | `(num_faces, 2)` | float64 | Face-centre coordinates. |
| `/faces/unit_normal` | `(num_faces, 2)` | float64 | Unit normal associated with the owner orientation. |
| `/faces/length` | `(num_faces)` | float64 | Positive face length. |
| `/faces/patch_id` | `(num_faces)` | int32 | Boundary patch id, or `-1` for interior faces. |

### `/boundary_patches`

| Dataset | Shape | Type | Meaning |
| --- | --- | --- | --- |
| `/boundary_patches/id` | `(num_patches)` | int32 | Boundary patch ids. |
| `/boundary_patches/name` | `(num_patches)` | fixed string | Patch names such as `south`, `open_sea` or `river_inflow`. |
| `/boundary_patches/type` | `(num_patches)` | fixed string | Patch type/classification. |

## Validation rules

Mohid-NG currently validates:

- schema version;
- producer name;
- 2-D dimension;
- required attributes and datasets;
- dataset ranks and sizes;
- contiguous zero-based node and cell ids;
- positive cell areas;
- face owners in range;
- face neighbours in range or `-1` for boundary;
- boundary faces reference existing patches;
- interior faces do not reference boundary patches;
- positive face lengths;
- unit-length face normals.

```{admonition} Validation gate
:class: validation-gate
Block 1 is complete when Mohid-NG reads a minimal HDF5 Voronoi mesh package, recognises boundary patches and metadata, rejects malformed packages with clear diagnostics and still contains no mesh-generation or remeshing functionality.
```

## Current reader API

The public C++ API intentionally hides HDF5 types:

```cpp
mohidng::MeshView mesh = mohidng::ReadVoronoiMeshPackage("mesh.mngpkg.h5");
```

HDF5 is an external implementation dependency of the reader, not a public physical-model API type.

