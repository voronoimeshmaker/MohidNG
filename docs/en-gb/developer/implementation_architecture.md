# Implementation architecture

Mohid-NG shall be built from concrete components, composition, traits, concepts, registries and factories. Inheritance and virtual functions shall not be used in performance-critical numerical kernels.

Backends for solvers, I/O and GIS shall be isolated behind adapters. Physical model code shall remain backend-neutral.
