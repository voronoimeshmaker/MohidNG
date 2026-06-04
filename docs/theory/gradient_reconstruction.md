# Gradient reconstruction

The bootstrap implementation now contains Mohid-NG-owned weighted least-squares and classical Green-Gauss cell-centred gradient operators. Its role is to establish a first numerical service, a cached workspace pattern and a testing pattern.

Gradient methods are extensible through a registry/factory model, not through a central enum. New methods should register their own workspace builder under a stable textual name.

Gradient weightings are policies. Built-in policies currently cover uniform, inverse-distance and inverse-distance-squared weighting; custom policies can be supplied without changing the core implementation.

The production Mohid-NG gradient module shall later support several methods, including Green-Gauss, weighted least-squares, QR-based least-squares, curvilinear reconstruction, limited gradients and validation diagnostics.
The workspace exposes diagnostics with method name, weighting name, cell count, well-conditioned stencil count, ill-conditioned stencil count and cached coefficient count.
