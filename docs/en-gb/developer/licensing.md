# Dependency treatment

Mohid-NG uses and is expected to use a range of external libraries,
development tools and optional scientific/HPC backends.

The authoritative inventory of third-party software actually used,
distributed or required by the project is maintained in
`THIRD_PARTY_NOTICES.md` and shall evolve with the implementation.

Dependencies shall be classified, where applicable, as:

- direct runtime or build dependencies;
- optional scientific or HPC backends;
- solver, partitioning or numerical packages reached through another dependency;
- testing and development dependencies;
- documentation dependencies; and
- transitive dependencies relevant to redistribution or licence compliance.

Each third-party component remains governed by its own licence.

The licence of a parent dependency shall not be assumed to cover packages
that it downloads, builds, links or otherwise makes available. Relevant
transitive and optional dependencies shall therefore be reviewed separately
when they become part of a supported Mohid-NG configuration.

New dependencies shall be added to `THIRD_PARTY_NOTICES.md` when adopted,
including their role, licence, integration mode and any redistribution or
compatibility considerations relevant to Mohid-NG.