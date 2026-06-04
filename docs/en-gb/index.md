# Mohid-NG

```{raw} html
<div class="hero-mohid">
  <h1>Mohid-NG</h1>
  <p>Voronoi-based environmental modelling for hydrodynamics, transport and coupled coastal, riverine and watershed systems.</p>
  <p>Current contact: <a style="color:#F4FAFC" href="mailto:mohidng@hotmail">mohidng@hotmail</a>. GitHub: <a style="color:#F4FAFC" href="https://github.com/mohidng">github.com/mohidng</a>.</p>
</div>
```

::::{grid} 1 2 4 4
:gutter: 3

:::{grid-item-card} Run a case
:class-card: mohid-card
Install the tools, select a Voronoi mesh package, configure a case and execute Mohid-NG.
:::

:::{grid-item-card} Understand the equations
:class-card: mohid-card
Read the finite-volume formulation, field definitions, boundary conditions and diagnostics.
:::

:::{grid-item-card} Prepare Voronoi data
:class-card: mohid-card
Use VoronoiMeshMaker to generate, adapt, validate and package all Voronoi mesh data.
:::

:::{grid-item-card} Develop Mohid-NG
:class-card: mohid-card
Follow the implementation architecture, coding standard, tests and validation gates.
:::

::::

```{raw} html
<div class="workflow-line">GIS data → VoronoiMeshMaker → Voronoi mesh package → Mohid-NG → validation → output</div>
```

```{admonition} VoronoiMeshMaker responsibility
:class: voronoimeshmaker
All mesh-related implementation belongs to VoronoiMeshMaker: mesh generation, generator redistribution, remeshing, connectivity reconstruction, mesh-quality control, mesh hierarchies and mesh adaptation. Mohid-NG consumes validated Voronoi mesh packages.
```

## Documentation structure


## Participating institution

Mohid-NG is currently developed at the Universidade do Estado do Rio de Janeiro (UERJ), through Instituto Politécnico (IPRJ/UERJ).

```{toctree}
:maxdepth: 2
:caption: Overview

overview/what_is_mohid_ng
overview/institutions
overview/relation_to_mohid_classic
overview/relation_to_voronoimeshmaker
```

```{toctree}
:maxdepth: 2
:caption: Getting started

getting_started/installation
getting_started/first_case
getting_started/running_examples
```

```{toctree}
:maxdepth: 2
:caption: Workflow

workflow/complete_workflow
workflow/gis_data
workflow/voronoi_mesh_package
workflow/forcing
workflow/postprocessing
```

```{toctree}
:maxdepth: 2
:caption: Theory

theory/governing_equations
theory/finite_volume_voronoi
theory/gradient_reconstruction
theory/free_surface
theory/wetting_and_drying
theory/remeshing_conservation
theory/parallel_partitioning
theory/solver_acceleration
```

```{toctree}
:maxdepth: 2
:caption: Models

models/hydrodynamics
models/transport
models/water_quality
models/sediments
models/lagrangian_particles
models/land_river_coupling
```

```{toctree}
:maxdepth: 2
:caption: Examples and validation

examples/index
validation/index
```

```{toctree}
:maxdepth: 2
:caption: Developer guide

developer/architecture
developer/coding_standard
developer/implementation_architecture
developer/testing
developer/performance
```

```{toctree}
:maxdepth: 2
:caption: File formats and API

file_formats/case_file
file_formats/mesh_package
file_formats/restart_file
file_formats/output_files
api/index
```
