# Gradient reconstruction

Gradient reconstruction is a first-class numerical service of Mohid-NG. It is required for diffusion, reconstruction, source terms, diagnostics and adaptation indicators.

The initial operator families should include:

```text
Green-Gauss
least squares
weighted least squares
QR-based weighted least squares
limited gradients
```

Adaptation indicators shall not depend exclusively on a single raw gradient norm.
