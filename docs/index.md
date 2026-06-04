# Mohid-NG documentation

```{raw} html
<section class="mohidng-home">
  <div class="mohidng-hero mohidng-hero-home">
    <div class="mohidng-hero-copy">
      <span class="mohidng-kicker">Voronoi finite volumes for environmental modelling</span>
      <h1>Mohid-NG</h1>
      <p class="mohidng-lead">A new-generation environmental modelling platform for hydrodynamics, transport and coupled coastal, riverine and watershed systems.</p>
      <div class="mohidng-hero-actions" aria-label="Documentation entry points">
        <a class="mohidng-button primary" href="en-gb/index.html">English</a>
        <a class="mohidng-button" href="pt-br/index.html">Português do Brasil</a>
        <a class="mohidng-button" href="pt-pt/index.html">Português de Portugal</a>
      </div>
    </div>
    <div class="mohidng-hero-visual" aria-label="Mohid-NG identity and workflow">
      <img src="_static/logo/mohid-ng-logo.png" alt="Mohid-NG logo" />
      <div class="mohidng-mesh-signal">
        <span></span><span></span><span></span><span></span><span></span><span></span>
      </div>
    </div>
  </div>

  <div class="mohidng-split-band">
    <div>
      <h2>Project Boundary</h2>
      <p>Mohid-NG consumes validated Voronoi mesh packages and focuses on physical models, finite-volume operators, diagnostics and reproducible simulation workflows.</p>
    </div>
    <div>
      <h2>Mesh Responsibility</h2>
      <p>VoronoiMeshMaker owns mesh generation, generator redistribution, remeshing, connectivity reconstruction, mesh-quality metrics and mesh hierarchy construction.</p>
    </div>
  </div>

  <div class="mohidng-workflow" aria-label="Mohid-NG workflow">
    <div class="mohidng-workflow-step"><strong>GIS data</strong><span>domain and forcing context</span></div>
    <div class="mohidng-workflow-step"><strong>VoronoiMeshMaker</strong><span>validated mesh package</span></div>
    <div class="mohidng-workflow-step"><strong>Mohid-NG</strong><span>fields, operators and models</span></div>
    <div class="mohidng-workflow-step"><strong>Validation</strong><span>conservation diagnostics</span></div>
    <div class="mohidng-workflow-step"><strong>Output</strong><span>analysis and visualisation</span></div>
  </div>
</section>
```

::::{grid} 1 1 3 3
:gutter: 3

:::{grid-item-card} British English
:class-card: mohid-card
Start from the English documentation, including overview, workflow, theory, models and developer notes.

```{button-ref} en-gb/index
:ref-type: doc
:color: primary
:expand:
Open English documentation
```
:::

:::{grid-item-card} Português do Brasil
:class-card: mohid-card
Acesse a documentação brasileira com visão geral, primeiros passos, teoria e desenvolvimento.

```{button-ref} pt-br/index
:ref-type: doc
:color: primary
:expand:
Abrir documentação brasileira
```
:::

:::{grid-item-card} Português de Portugal
:class-card: mohid-card
Aceda à documentação portuguesa com enquadramento, primeiros passos, teoria e desenvolvimento.

```{button-ref} pt-pt/index
:ref-type: doc
:color: primary
:expand:
Abrir documentação portuguesa
```
:::

::::

```{admonition} VoronoiMeshMaker responsibility
:class: voronoimeshmaker
All mesh-related implementation belongs to VoronoiMeshMaker: Voronoi generation, generator-point redistribution, remeshing, mesh adaptation, connectivity reconstruction, mesh-quality metrics, mesh hierarchies and GIS-driven mesh preparation. Mohid-NG consumes validated Voronoi mesh packages.
```

```{admonition} Validation gate
:class: validation-gate
The bootstrap gate is intentionally concrete: configure, build, run CTest, validate the documentation design system, validate trilingual entry points and render the Sphinx site without warnings.
```

## Participating Institution

```{raw} html
<div class="logo-strip mohidng-institution-strip">
  <div class="logo-card">
    <img class="institution-logo" src="_static/logos/uerj-placeholder.svg" alt="UERJ logo placeholder" />
    <strong>Universidade do Estado do Rio de Janeiro</strong>
  </div>
  <div class="logo-card">
    <img class="institution-logo" src="_static/logos/iprj-placeholder.svg" alt="IPRJ logo placeholder" />
    <strong>Instituto Politécnico, IPRJ/UERJ</strong>
  </div>
</div>
```

Mohid-NG is currently developed at the Instituto Politécnico, a unit of the Universidade do Estado do Rio de Janeiro, Brazil. Contact: [mohidng@hotmail](mailto:mohidng@hotmail). GitHub: [github.com/mohidng](https://github.com/mohidng).

```{toctree}
:hidden:
:maxdepth: 2

English <en-gb/index>
Português do Brasil <pt-br/index>
Português de Portugal <pt-pt/index>
design_system
developer/startup_plan
examples/examples_policy
overview/relation_to_voronoimeshmaker
theory/gradient_reconstruction
en-gb/developer/error_exception_logging
en-gb/developer/startup_plan
```
