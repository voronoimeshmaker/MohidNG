# Case file

The case file shall describe physics, fields, boundary patches, forcing, solver options, output and diagnostics in a user-readable format.

## Initial YAML-style input example

The first executable input example is connected to the rectangular-domain gradient reconstruction example:

```bash
make run_rectangular_domain_gradient
```

That target reads `examples/Ex05_RectangularDomainGradient/RectangularDomainGradient.ini`. The file keeps the example data beside the source code and uses YAML syntax to describe the rectangular domain, mesh resolution, linear field, and gradient options.
