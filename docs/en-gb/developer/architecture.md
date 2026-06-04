# Architecture

Mohid-NG shall expose a simple public API and hide internal implementation detail. Physical models shall depend on fields, operators and mesh views, not on mesh-generator internals or solver-library objects.

Hot numerical code shall use data-oriented structures and avoid virtual dispatch, heap allocation inside loops and string lookup inside loops.
