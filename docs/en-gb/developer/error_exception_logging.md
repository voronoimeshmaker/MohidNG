# Error, exception and flow-trace policy

Mohid-NG shall maintain separate subsystems for registered errors, exceptions and
execution-flow tracing.

## Error

An `Error` is a registered value. It has a textual code, optional detail and
localised messages. Error codes are strings, not enums, so new modules can add
new errors without modifying a central enumeration.

The supported language keys are:

- `pt-br`, default;
- `pt-pt`;
- `en-gb`.

Messages are emitted in the selected language. Unsupported languages fall back to
`pt-br`.

## Exception

An exception is the interruption mechanism. `MohidNgException` carries an
`Error` and, in debug builds, the flow trace captured at the throw site.

Errors and exceptions are intentionally separate: a module may create, register
and report an error without necessarily throwing an exception.

## Logger and trace

`TraceScope` records the active execution path by RAII. Use
`MOHIDNG_TRACE_SCOPE("scope-name")` at public API boundaries, important
algorithmic functions and backend adapters.

The trace stack is active in debug builds and compiled as a no-op in release
builds through `NDEBUG`. This keeps diagnostics rich during development and avoids
runtime cost in production builds.

## Mandatory rule

Public functions shall raise registered errors whenever possible. Unregistered
error codes are allowed during experimentation, but official modules shall add
their codes and messages to the error catalogue or to a module-level registry.
