# Mohid-NG

```{raw} html
<div class="hero-mohid">
  <h1>Mohid-NG</h1>
  <p>Modelação ambiental baseada em malhas Voronoi para hidrodinâmica, transporte e sistemas costeiros, fluviais e de bacias acoplados.</p>
  <p>Contacto actual: <a style="color:#F4FAFC" href="mailto:mohidng@hotmail">mohidng@hotmail</a>. GitHub: <a style="color:#F4FAFC" href="https://github.com/mohidng">github.com/mohidng</a>.</p>
</div>
```

::::{grid} 1 2 4 4
:gutter: 3

:::{grid-item-card} Executar um caso
:class-card: mohid-card
Instale as ferramentas, seleccione um pacote de malha Voronoi, configure o caso e execute o Mohid-NG.
:::

:::{grid-item-card} Compreender as equações
:class-card: mohid-card
Consulte a formulação por volumes finitos, os campos, as fronteiras e os diagnósticos.
:::

:::{grid-item-card} Preparar dados Voronoi
:class-card: mohid-card
Use o VoronoiMeshMaker para gerar, adaptar, validar e empacotar todos os dados de malha Voronoi.
:::

:::{grid-item-card} Desenvolver o Mohid-NG
:class-card: mohid-card
Siga a arquitectura de implementação, a norma de código, os testes e os critérios de validação.
:::

::::

```{raw} html
<div class="workflow-line">Dados GIS → VoronoiMeshMaker → Pacote de malha Voronoi → Mohid-NG → validação → saída</div>
```

```{admonition} Responsabilidade do VoronoiMeshMaker
:class: voronoimeshmaker
Toda a implementação relacionada com a malha pertence ao VoronoiMeshMaker: geração, redistribuição de pontos geradores, remeshing, reconstrução de conectividade, controlo de qualidade, hierarquias e adaptação de malha. O Mohid-NG consome pacotes de malha Voronoi validados.
```


## Instituição participante

O Mohid-NG é actualmente desenvolvido na Universidade do Estado do Rio de Janeiro (UERJ), através do Instituto Politécnico (IPRJ/UERJ).

```{toctree}
:maxdepth: 2
:caption: Visão geral

overview/what_is_mohid_ng
overview/institutions
overview/relation_to_voronoimeshmaker
```

```{toctree}
:maxdepth: 2
:caption: Primeiros passos

getting_started/installation
getting_started/first_case
```

```{toctree}
:maxdepth: 2
:caption: Teoria

theory/finite_volume_voronoi
theory/gradient_reconstruction
theory/remeshing_conservation
```

```{toctree}
:maxdepth: 2
:caption: Desenvolvimento

developer/architecture
developer/coding_standard
```
