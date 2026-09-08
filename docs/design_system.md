# Mohid-NG Coastal Scientific Theme

This page defines the mandatory visual design system for the Block 0 documentation bootstrap. It is part of the repository contract: the web documentation must not depend only on informal design notes or external conversations.

The visual identity combines water, bathymetry, Voronoi geometry and scientific computing. The result must be modern, technical, clean and restrained. It must not look like generic software marketing, and it must not look like an old academic manual.

## Design stack

The documentation stack is:

- Sphinx;
- MyST Markdown;
- PyData Sphinx Theme;
- sphinx-design;
- sphinx-copybutton;
- sphinxcontrib-bibtex;
- custom CSS in `docs/_static/css/mohid-ng.css`.

## Light palette

| Token | Hex | Role |
|---|---:|---|
| Deep Ocean | `#073B4C` | Primary navigation, headings and strong accents |
| Harbour Blue | `#0B5C75` | Links and secondary structural elements |
| Tidal Teal | `#118AB2` | Interactive accents and scientific highlights |
| Estuary Cyan | `#47B5D1` | Secondary highlights and dark-mode links |
| Foam | `#F4FAFC` | Main light background |
| Mist | `#E8F1F5` | Secondary light surface |
| Sand | `#D8B26E` | Mild warnings and institutional accents |
| Sediment Amber | `#C9822B` | Warning, validation and attention colour |
| Graphite | `#1F2933` | Primary text colour |
| Slate | `#52616B` | Secondary text colour |

## Dark palette

| Token | Hex | Role |
|---|---:|---|
| Abyss | `#071923` | Main dark background |
| Deep Navy | `#0B2530` | Dark cards and surfaces |
| Blue Graphite | `#102F3D` | Dark structural background |
| Muted Cyan | `#5BC0D8` | Dark-mode links and highlights |
| Foam Text | `#DDEFF4` | Dark-mode text |
| Soft Slate | `#9DB3BD` | Dark-mode secondary text |
| Sediment Gold | `#D6A354` | Dark-mode warning accent |
| Error Coral | `#E07A5F` | Error and failure accent |

## Typography

The main documentation font is **Inter**. The code and terminal font is **JetBrains Mono**.

Fallback families are allowed and must remain in the CSS:

```css
--pst-font-family-base: Inter, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
--pst-font-family-monospace: "JetBrains Mono", "Cascadia Code", "Source Code Pro", monospace;
```

Font files must not be redistributed in the repository. The CSS declares the preferred families only.

## Layout

The standard page layout is:

- global navigation on the top bar;
- left sidebar for the documentation hierarchy;
- central content column;
- right sidebar for the local table of contents.

The home page shall use:

- a short hero section;
- cards for the main user paths;
- a horizontal workflow statement;
- a visible VoronoiMeshMaker responsibility notice;
- institutional attribution to UERJ through IPRJ/UERJ.

## Core components

The following CSS components are part of the Block 0 contract:

- `.mohidng-hero`, alias for the main hero area;
- `.mohidng-workflow`, alias for the workflow strip;
- `.mohid-card`, for documentation cards;
- `.logo-card`, for institutional logo cards;
- `.institution-logo`, for official institutional logos;
- `.admonition-voronoimeshmaker-responsibility`, for the boundary between Mohid-NG and VoronoiMeshMaker;
- `.admonition-validation-gate`, for block-completion conditions;
- `.admonition-conservation-requirement`, for conservation requirements.

Legacy aliases such as `.hero-mohid` and `.workflow-line` may remain during the bootstrap phase, but the Mohid-NG-specific names above are the preferred names.

## Custom admonitions

The documentation may use the following semantic admonitions:

- Numerical requirement;
- Conservation requirement;
- VoronoiMeshMaker responsibility;
- Validation gate;
- Performance note;
- Experimental feature.

The most important one is **VoronoiMeshMaker responsibility**. It must be used whenever a page discusses mesh generation, generator redistribution, remeshing, connectivity reconstruction, mesh-quality metrics or mesh hierarchy construction. These are VoronoiMeshMaker responsibilities. Mohid-NG consumes validated Voronoi mesh packages.

## Logo policy

The current participating institution is:

- Universidade do Estado do Rio de Janeiro (UERJ), through Instituto Politécnico (IPRJ/UERJ).

The documentation may contain UERJ and IPRJ/UERJ logo placeholders during Block 0. Real logos must be added only when the authorised files and usage conditions are available.

Rules:

- prefer SVG;
- do not distort proportions;
- do not recolour institutional logos;
- do not apply shadows or decorative effects;
- keep the Mohid-NG project identity visually distinct from institutional attribution.

## Validation

The design system is checked by:

```bash
python tools/check_design_system.py
```

The bilingual documentation entry points are checked by:

```bash
python tools/check_bilingual_docs.py
```

The Block 0 completion gate requires these scripts to pass, in addition to the C++ build, C++ tests and Sphinx documentation build.