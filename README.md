# OpenComputers Fabric Port (1.21.6) — bootstrap

This repository now contains the initial bootstrap for a **full-port effort** of OpenComputers to Fabric **1.21.6**.

## Status

This commit does **not** claim feature parity yet. It establishes:

- Fabric Loom project for Minecraft 1.21.6.
- Mod bootstrap + registries entrypoints.
- Core architecture contracts for components, network nodes, and VM abstraction.
- NBT-serializable node graph primitives.
- A concrete first block (`Case Block`) with a matching block entity.

## Goal

Reach full parity with original OpenComputers functionality (computers, components, robots, drones, rack servers, networking, storage/media, Lua runtime integration, power model, upgrades, rendering, and addon compatibility surface where feasible).

## Next milestones

1. Implement component bus and addressable network semantics.
2. Implement VM lifecycle/state machine and persistent machine state.
3. Port canonical blocks/items in dependency order (case/chassis -> screen/keyboard -> filesystem/media -> adapters).
4. Implement power and tick scheduling model.
5. Provide migration/test matrix for parity assertions.

