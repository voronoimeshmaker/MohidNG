# Third-party notices

Mohid-NG is licensed under the Mozilla Public License 2.0 (MPL-2.0).
The complete licence text for Mohid-NG is provided in the `LICENSE` file
at the repository root.

This document records third-party software relevant to the development,
build, documentation, execution and distribution of Mohid-NG, together with
the policy used to manage software provenance and licensing obligations.

It also records the licensing boundary between Mohid-NG and independent
projects that form part of the wider Mohid-NG scientific and numerical
ecosystem.

Third-party components remain subject to their own licences. The MPL-2.0
licence of Mohid-NG does not replace, modify or supersede the licence terms
of any external component.

## 1. General policy

Mohid-NG should prefer external dependencies supplied through system
packages, CMake packages, package managers or clearly isolated optional
backends.

Third-party source code should not be copied into the Mohid-NG repository
unless there is a documented technical reason for doing so and the
applicable licence permits the intended use, modification and distribution.

When third-party source code is incorporated, copied, translated or adapted,
all applicable copyright notices, licence texts, attribution requirements
and other obligations shall be preserved.

A dependency shall not be considered suitable for Mohid-NG solely because
it is open source. Technical suitability, licence compatibility,
redistribution obligations, provenance, maintenance implications and
relevant transitive dependencies shall be considered when the dependency
becomes part of a supported or distributed Mohid-NG configuration.

The licence of a parent package shall not be assumed to cover libraries,
solvers, partitioners or other components that the package can optionally
download, build, link, load or invoke.

## 2. Current core and build dependencies

The following third-party components are currently required directly by the
Mohid-NG build configuration.

| Component | Current role | Licence notes | Mohid-NG treatment |
| --- | --- | --- | --- |
| HDF5 | Voronoi mesh-package I/O | HDF5 licence, BSD-style | External dependency. Do not vendor without explicit review. Avoid exposing HDF5 implementation types in public physical-model APIs. |
| yaml-cpp | YAML configuration support | MIT licence | External dependency. Do not vendor without explicit review. Keep configuration parsing separated from physical-model interfaces. |

The project's supported CMake configuration is the authoritative source for
direct build dependencies.

This table shall be updated whenever a direct build or runtime dependency is
added, removed or replaced.

## 3. Current documentation dependencies

The documentation toolchain is defined by `requirements-docs.txt`.

| Component | Current role | Licence notes |
| --- | --- | --- |
| Sphinx | Documentation build | BSD-style licence |
| MyST Parser | Markdown support for Sphinx | MIT licence |
| PyData Sphinx Theme | Documentation HTML theme | BSD-style licence |
| sphinx-design | Documentation UI components | MIT licence |
| sphinx-copybutton | Documentation code-copy support | MIT licence |
| sphinxcontrib-bibtex | Bibliography support | BSD-style licence |
| Breathe | API-documentation bridge | BSD-style licence |
| Exhale | API-documentation generation support | BSD-style licence |

These components are development and documentation dependencies and are not
part of the Mohid-NG numerical core.

## 4. Planned and candidate dependencies

The Mohid-NG architecture is expected to integrate additional scientific,
numerical, testing and HPC software as implementation progresses.

The components listed in this section are planned, anticipated or under
consideration. Their presence here does not mean that they are currently
required by the Mohid-NG build, nor does it constitute final approval of
their licence or integration model.

| Component or family | Anticipated role | Mohid-NG treatment |
| --- | --- | --- |
| PETSc | Linear and nonlinear solvers, scalable numerical infrastructure and parallel execution support | Intended as the principal external numerical backend. Avoid unnecessary propagation of PETSc implementation types into public physical-model APIs. |
| MUMPS | Sparse direct solution, normally accessed through PETSc | Review independently when included in a supported configuration, including licence and redistribution obligations. |
| SuperLU and related solver packages | Sparse linear-system solution, potentially accessed through PETSc | Review individually when adopted. |
| METIS | Graph partitioning and ordering | Treat as an external component. Record the actual version, licence and integration mode when adopted. |
| ParMETIS | Parallel graph partitioning | Requires explicit licence and redistribution review before inclusion in a supported configuration. |
| Boost | General-purpose C++ facilities where justified | Use only where it provides a clear technical benefit over the C++ standard library or other established project facilities. |
| GoogleTest | Unit, component and API-level testing | Development and testing dependency. Record the actual integration when adopted by the supported build. |
| NetCDF | Scientific data I/O where required | External dependency if adopted. Do not vendor without explicit review. |
| VoroGradToolkit | Gradient reconstruction and related Voronoi finite-volume numerical operations | Independent numerical library expected to provide specialised gradient functionality to Mohid-NG. Its licence, version, integration mode and transitive dependencies shall be recorded when direct integration is adopted. |

This list is not exhaustive and shall not be treated as a frozen dependency
set.

New libraries shall be evaluated according to technical merit,
architectural fit, numerical requirements, maintenance cost, licence
compatibility, portability and reproducibility requirements.

Once a planned component becomes part of a supported Mohid-NG configuration,
it shall be moved to the appropriate current-dependency section and its
specific licence information shall be verified.

## 5. Optional, indirect and transitive dependencies

Mohid-NG may use software packages that themselves provide access to
additional libraries, solvers, partitioners or optional backends.

Such components shall not automatically be treated as covered by the
licence or compatibility assessment of the parent package.

Relevant optional and transitive dependencies shall be reviewed individually
when they become part of a supported Mohid-NG configuration, particularly
when they affect:

- source or binary redistribution;
- static or dynamic linking obligations;
- attribution requirements;
- reciprocal or copyleft obligations;
- commercial or institutional redistribution;
- supported operating systems;
- reproducibility of scientific results; or
- release packaging.

For each material third-party component, the project should record, where
applicable:

- component name and version;
- upstream project or source;
- role in Mohid-NG;
- upstream licence;
- whether it is a direct, optional or transitive dependency;
- whether it is linked, dynamically loaded, invoked externally or used only
  during development;
- whether it is redistributed with Mohid-NG; and
- relevant attribution, compatibility or redistribution obligations.

## 6. Independent projects in the Mohid-NG ecosystem

Some software systems interact closely with Mohid-NG while remaining
independent projects.

Interaction with Mohid-NG does not by itself make their source code or
internal dependencies part of the Mohid-NG core.

The architectural, licensing and provenance boundaries between these
projects shall remain explicit.

### 6.1 VoronoiMeshMaker

VoronoiMeshMaker is an independent mesh-generation project responsible for
producing Voronoi mesh packages that can be consumed by Mohid-NG.

Activities belonging to VoronoiMeshMaker include, as applicable:

- Voronoi mesh generation;
- generator-point redistribution;
- remeshing;
- mesh adaptation;
- connectivity construction;
- mesh-quality analysis;
- mesh hierarchy construction; and
- GIS-driven mesh preparation.

Dependencies used exclusively for these activities belong to
VoronoiMeshMaker or to its external workflow and shall not be recorded as
Mohid-NG dependencies unless Mohid-NG also uses them directly.

The data and interface contract between VoronoiMeshMaker and Mohid-NG shall
be documented independently of the internal implementation and dependency
choices of either project.

A change in the internal implementation of VoronoiMeshMaker shall not, by
itself, require a change in the Mohid-NG numerical core provided that the
agreed mesh-data contract remains satisfied.

### 6.2 VoroGradToolkit

VoroGradToolkit is an independent numerical software project dedicated to
gradient reconstruction and related numerical operations for
Voronoi-based finite-volume discretisations.

Mohid-NG may use VoroGradToolkit to provide specialised gradient and
reconstruction capabilities rather than duplicating those implementations
inside the Mohid-NG core.

VoroGradToolkit remains an independent project with its own source code,
release cycle, architecture, dependencies and licensing terms.

Unlike an external preprocessing or validation tool, VoroGradToolkit may
become a direct software dependency of Mohid-NG if Mohid-NG links against,
loads or otherwise invokes the VoroGradToolkit library during execution.

When such integration becomes part of a supported configuration, this
document shall record:

- the VoroGradToolkit version or compatible version range;
- its applicable licence;
- whether the dependency is required or optional;
- the integration mechanism;
- whether it is redistributed with Mohid-NG;
- relevant transitive dependencies; and
- applicable attribution, compatibility or redistribution obligations.

The architectural responsibility of each project shall remain explicit.

Mohid-NG owns the system-level policy that determines which gradient or
reconstruction capability is required by its numerical and physical models.

VoroGradToolkit owns the implementation of gradient algorithms and related
numerical functionality that fall within its defined scope.

Numerical functionality deliberately assigned to VoroGradToolkit should not
be independently duplicated inside the Mohid-NG core unless a documented
architectural decision establishes a technical reason for doing so.

VoroGradToolkit implementation types should not propagate unnecessarily
through public Mohid-NG physical-model APIs.

### 6.3 CRIVO

CRIVO is an independent verification and validation project used to assess
the scientific and numerical behaviour of Mohid-NG.

CRIVO is not part of the Mohid-NG numerical core and is not a conventional
runtime library dependency.

Its internal source code and software dependencies shall not be treated as
Mohid-NG dependencies solely because CRIVO is used for verification,
validation or release assessment.

The relationship between Mohid-NG and CRIVO is primarily one of independent
evaluation and evidence exchange.

Interfaces, datasets, test cases, analytical or reference solutions,
acceptance criteria, reports and other artefacts exchanged between Mohid-NG
and CRIVO shall be documented sufficiently to support reproducible
verification and validation.

A Mohid-NG release may depend procedurally on successful CRIVO evaluation
without creating a software-library dependency between Mohid-NG and CRIVO.

If CRIVO is ever linked, embedded, bundled or distributed together with
Mohid-NG, the resulting licensing, provenance and redistribution implications
shall be reviewed explicitly before that distribution model is adopted.

### 6.4 Other independent projects

The same boundary principle applies to other independent preprocessing,
validation, scientific-support or specialised numerical projects.

Interaction through documented files, data contracts, external processes,
libraries or validation workflows does not automatically make one project's
internal dependencies dependencies of another project.

If the integration model changes materially, the corresponding architectural,
dependency and licensing classification shall be reviewed.

## 7. Provenance rules

Mohid-NG shall distinguish explicitly between:

- independently developed Mohid-NG source code;
- external libraries used as dependencies;
- third-party source code incorporated into the repository;
- generated source code;
- code translated from another programming language;
- code adapted or derived from another software project; and
- scientific literature, equations, numerical methods and algorithms used
  as implementation references.

Code copied, translated or adapted from another software project shall not
be treated as independently developed Mohid-NG source code.

Its provenance and applicable licence shall be identified before
incorporation into the project.

Published equations, numerical methods, algorithms and scientific concepts
may serve as implementation references subject to applicable
intellectual-property conditions, but this does not by itself authorise the
copying, translation or close adaptation of another project's source-code
expression.

Legacy scientific software may be studied as a behavioural, scientific or
numerical reference, but source-code provenance shall remain explicit.

## 8. Public API and architectural boundary

Third-party implementation details should remain outside public
physical-model APIs whenever technically practical.

This principle applies to implementation-specific types associated with:

- solver libraries;
- parallel-runtime infrastructure;
- I/O packages;
- graph partitioners;
- mesh generators;
- gradient libraries; and
- other external numerical components.

The objective is to prevent unnecessary architectural coupling and to allow
external implementations to evolve or be replaced without requiring
unnecessary changes to Mohid-NG scientific interfaces.

This rule does not prohibit deliberate exposure of a third-party API where
such exposure is justified and explicitly accepted as an architectural
decision.

## 9. Compatibility rules

The following rules apply to third-party software used by Mohid-NG:

- Project-authored Mohid-NG source code is licensed under MPL-2.0 unless a
  file explicitly states otherwise.
- Third-party components retain their own licences.
- Keep third-party dependencies external whenever practical.
- Do not vendor third-party source code without explicit technical and
  licensing review.
- Preserve applicable upstream copyright notices, licence files and
  attribution requirements.
- Review reciprocal, copyleft or otherwise material licensing obligations
  before adopting affected components into supported configurations.
- Review relevant optional and transitive dependencies independently.
- Do not assume licence compatibility solely from the licence of a parent
  framework or package.
- Avoid unnecessary propagation of third-party implementation types into
  public Mohid-NG physical-model APIs.
- Record the provenance of copied, translated or adapted source code before
  incorporation.
- Reassess dependency and licensing implications whenever the integration or
  distribution model materially changes.

## 10. Maintenance and release review

This document represents the dependency, external-project and licensing
state of Mohid-NG and shall evolve with the implementation.

Current dependencies shall reflect the actual supported build and execution
configuration.

Planned or candidate dependencies shall remain clearly distinguished from
components already adopted.

Before each public release, the project shall review:

- direct build and runtime dependencies;
- supported optional backends;
- material transitive dependencies;
- documentation and development dependencies relevant to distributed
  artefacts;
- vendored or incorporated third-party material;
- applicable licence and attribution requirements;
- provenance records for incorporated, translated or adapted code;
- the integration status of VoroGradToolkit and other external numerical
  libraries;
- the interface boundary with VoronoiMeshMaker; and
- release-specific verification and validation requirements involving CRIVO.

CRIVO evaluation and other scientific release gates shall be governed by the
MOHID-NG verification, validation and release policy rather than by this
document alone.

This file shall be updated whenever a change in dependencies, ecosystem
boundaries, integration architecture or distribution practice materially
changes the third-party software obligations of Mohid-NG.