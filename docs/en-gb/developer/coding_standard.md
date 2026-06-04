# Coding standard

The Mohid-NG coding standard uses modern C++ conventions aligned with Google-style formatting where practical.

Initial rules:

```text
.h and .cc extensions
header guards
include order: related header, C/C++ library, external libraries, project headers
PascalCase for types
PascalCase for public functions when adopting Google style
snake_case for variables
class data members end with underscore
constants use kName
```

Project-specific rules override generic style rules when numerical performance or scientific clarity requires it.
