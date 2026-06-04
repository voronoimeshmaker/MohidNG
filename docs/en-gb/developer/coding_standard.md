# Coding standard

The Mohid-NG coding standard uses modern C++ conventions aligned with Google-style formatting where practical. Project-specific rules override generic style rules when numerical performance or scientific clarity requires it.

## Source files

```text
.h and .cc extensions
header guards or #pragma once in headers
PascalCase for types
PascalCase for public functions when adopting Google style
snake_case for variables
class data members end with underscore
constants use kName
```

## Class identifiers

Every class must register a stable class identifier for debug and error messages. Use one local declaration inside the public section of the class:

```cpp
class FieldSet {
 public:
  DefineIdentity("MohidNG.Fields.FieldSet")
};
```

Do not use central enums, RTTI or compiler-specific type names. Class-owned checks should use `RequireClass(...)` so that exceptions report the class identifier, source folder, source file, line and function.

## Include directives

Include directives are part of the project style and must follow this sequence:

1. C++ standard-library headers, alphabetically.
2. External-library headers, alphabetically, such as HDF5, PETSc, Trilinos, or other third-party dependencies.
3. Mohid-NG headers, alphabetically.

Separate the three include groups with one blank line. Mohid-NG headers must always use angle brackets, for example:

```cpp
#include <cmath>
#include <string>
#include <vector>

#include <hdf5.h>
#include <petsc.h>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>
```

Do not use quoted includes for Mohid-NG headers.

## Comments

Use `//` comments for source-code comments. Do not use `/* ... */` block comments in Mohid-NG source files. For long explanatory headers, such as example-file introductions, start and end the block with a `//` separator line no longer than 80 columns.
