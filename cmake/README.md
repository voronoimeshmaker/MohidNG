# MohidNG build modules

Only this directory was reorganized. The root `CMakeLists.txt`, presets, C++
sources, dependency selection and documentation output remain unchanged.

The three existing entry modules remain compatible with the root:

- `MohidNGCompilerOptions.cmake` loads options and private target settings from
  `project/`. Its deferred call configures the library after the unchanged root
  has created it.
- `MohidNGExamples.cmake` and `MohidNGTests.cmake` discover programs and preserve
  the input defaults of the existing examples/tests.
- `testing/MohidNGPrograms.cmake` shares program, CTest and runner setup.
- `testing/MohidNGScientificPrograms.cmake` adds enabled benchmarks/cases after
  the library exists, independently of the root's example/test switches.

## Optimization

| Option | Default | Effect |
| --- | --- | --- |
| `MOHIDNG_ENABLE_LTO` | `ON` | IPO/LTO in Release and RelWithDebInfo, after a compiler/linker support check. |
| `MOHIDNG_ENABLE_NATIVE_ARCH` | `OFF` | `-march=native` in optimized GCC/Clang configurations; local CPU only, not a portable binary. |
| `MOHIDNG_ENABLE_WARNINGS` | `ON` | Private warnings on the library and all project programs. |
| `MOHIDNG_ENABLE_SANITIZERS` | `OFF` | ASan/UBSan, O1, debug information and frame pointers; separate diagnostic build. |
| `MOHIDNG_ENABLE_COVERAGE` | `OFF` | Explicit coverage instrumentation, O0 and debug information; independent of whether gcovr is installed. |

Release and RelWithDebInfo use O3 with GCC/Clang or O2 with an MSVC-style
frontend. Debug is not silently changed to Release. IPO is configured per target,
including its final executable link, not through global flags. Instrumentation
disables IPO/native optimization; sanitizers and coverage cannot be combined.
Unsupported requested instrumentation/native options fail with a diagnostic.

No fast-math, Ofast, forced unrolling or PGO is enabled. Native optimization is
rejected for cross-compilation. Warnings and CPU flags are not public API usage
requirements. Instrumented static libraries expose the required diagnostic link
runtime only to build-tree consumers.

Commands below run from the MohidNG root and work with the existing root file.
Use separate build directories for each configuration.

```bash
# Portable optimized build, with supported LTO.
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel 2
ctest --test-dir build/release --output-on-failure

# Optimized for the CPU of this machine, not for distributing generic binaries.
cmake -S . -B build/release-native -DCMAKE_BUILD_TYPE=Release -DMOHIDNG_ENABLE_NATIVE_ARCH=ON
cmake --build build/release-native --parallel 2
ctest --test-dir build/release-native --output-on-failure

# Reference without LTO, for controlled comparisons.
cmake -S . -B build/release-reference -DCMAKE_BUILD_TYPE=Release -DMOHIDNG_ENABLE_LTO=OFF

# Diagnostics and coverage are different builds.
cmake -S . -B build/sanitizers -DCMAKE_BUILD_TYPE=Debug -DMOHIDNG_ENABLE_SANITIZERS=ON
cmake -S . -B build/coverage -DCMAKE_BUILD_TYPE=Debug -DMOHIDNG_ENABLE_COVERAGE=ON
```

Optimization flags are candidates for better performance, not a measured speedup.
Compare identical meshes, numerical settings and stopping criteria; record both
time and numerical error. Do not benchmark instrumented builds. These options do
not rebuild or optimize pre-installed HDF5, PETSc or other dependencies.

## Programs and runners

Four program groups are supported:

| Group | Folder and prefix | Purpose |
| --- | --- | --- |
| Tests | `tests/**/test_*.cpp` | Automated GoogleTest tests. |
| Examples | `examples/**/exe_*.cpp` | Small demonstrations of APIs and workflows. |
| Verification benchmarks | `benchmarks/**/bench_*.cpp` | Controlled preparatory problems, including analytic/manufactured solutions. |
| Scientific cases | `cases/**/case_*.cpp` | Complete scientific applications. |

Here, benchmark means scientific verification, not just performance timing.
New programs follow the `.cpp` convention above. Existing `tst_*` tests and `.cc`
files remain supported for compatibility. No scientific directories or C++
drivers are created by these modules.

Tests/examples retain their root switches. Enable the additional groups with
`-DMOHIDNG_BUILD_BENCHMARKS=ON` and `-DMOHIDNG_BUILD_CASES=ON` (both default OFF).
They can be built even with tests/examples disabled. Scientific programs have
`benchmarks`/`cases` build targets and `run_all_benchmarks`/`run_all_cases` runners.
They are not added to CTest unless `-DMOHIDNG_REGISTER_SCIENTIFIC_TESTS=ON`; this
keeps long scientific runs out of the default unit-test workflow.

New `test_*` programs use `find_package(GTest REQUIRED)`, `GTest::gtest_main` and
`gtest_discover_tests`, exposing individual GoogleTest cases to CTest. GTest must
be installed when such tests exist. The four existing standalone drivers remain
explicit compatibility exceptions; their C++ conversion is outside this change.
They are not falsely reported as GoogleTest tests. The infrastructure check also
needs GTest because it builds a real, small GoogleTest fixture.

Example entry points are `examples/**/exe_*.cc` or `exe_*.cpp`; test entry points
are `tests/**/test_*.cpp` (legacy `tst_*` and `.cc` are also accepted). One source creates one executable, including
when multiple programs share a directory. Duplicate executable names are errors.
A normal incremental build discovers additions/removals via `CONFIGURE_DEPENDS`;
build once to regenerate before requesting the runner of a newly added program.

Full runners always exist: `run_exe_name`, `run_test_name`, `run_bench_name` and
`run_case_name` (plus `run_tst_name` for legacy tests). Short `run_name`
aliases remain available when unambiguous. If an example and a test share the
same short name, use their full runners instead. Existing commands such as
`make run_gradient_reconstruction` continue to work; now
`make run_exe_gradient_reconstruction` works too.

Runners and CTest use the target executable path, including multi-configuration
subdirectories and platform suffixes. Working directories default to
`<build>/runs/<program>`, or `<build>/runs/<configuration>/<program>` for
multi-configuration generators, keeping generated case files away from sources.
Existing input files, including `RectangularDomainGradient.ini`, stay where they
are. Their default arguments use absolute source paths.

New programs default to no arguments and no additional libraries. Configure
exceptions without changing any CMake source, using uppercase executable names:

```bash
cmake -S . -B build/release '-DMOHIDNG_EXE_CASE1_ARGUMENTS=/path/input.ini;second argument'
cmake -S . -B build/release -DMOHIDNG_EXE_CASE1_LIBRARIES=yaml-cpp::yaml-cpp
cmake -S . -B build/release -DMOHIDNG_EXE_CASE1_WORKING_DIRECTORY=/path/to/results
```

The same variables work for `TEST_NAME`, `BENCH_NAME` and `CASE_NAME` (or legacy
`TST_NAME`). Arguments and libraries are CMake lists,
not shell command strings. Quoting preserves spaces within each argument. An
explicit empty argument override runs a program without arguments. Remove a
cached override with `cmake -U MOHIDNG_EXE_CASE1_ARGUMENTS ...` to restore defaults.
Shared defaults such as `MOHIDNG_BOOTSTRAP_PACKAGE` remain effective unless that
program has an explicit override. Additional dependency targets must already be
available; this mechanism does not download or implicitly discover libraries.

CTest labels `examples` and `tests` allow separate runs, for example
`ctest --test-dir build/release -L tests --output-on-failure`.

## Build infrastructure checks

```bash
cmake --build build/release --target check_cmake
```

The check generates tiny temporary projects under the selected build directory.
It verifies all four groups, individual GTest discovery, independent scientific
builds, `.cc`/`.cpp` discovery, new folders, removed files, one executable per
entry point, full/short runners, ambiguous names, argument quoting, refreshed
shared defaults and private/deferred settings. It never modifies the source tree.
The nine existing numerical/example tests remain separate from this check.

Validation on 2026-09-08 used GCC 15.2.0 and CMake 4.2.3 in Ubuntu-26.04-Test
under WSL. All nine existing tests passed in Debug, Release with LTO/native,
ASan/UBSan and coverage configurations. Infrastructure checks passed with Unix
Makefiles and Ninja Multi-Config, including a real GoogleTest fixture. Compile
commands confirmed that all 19 project translation units received the intended
profile, with no fast-math and no IPO/native flags in instrumented builds.
The old and full gradient runners both passed. The invalid sanitizer-plus-coverage
combination was rejected as intended. MSVC/Clang and runtime speedups were not
tested in this validation.

## Deliberate scope limits

The root still discovers library implementation files as `src/**/*.cc`. Supporting
`src/**/*.cpp`, changing dependency discovery, adding install/export rules or
editing root presets requires a later root change and is not hidden in these
modules. No VMM files, scientific methods, licences, web output, roadmap or
traceability records were changed by this build update.
