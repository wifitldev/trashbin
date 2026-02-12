# OpenComputers Fabric Port (1.21.6)

This repository contains a **modem/network-first** MVP of an OpenComputers port for Fabric **1.21.6**.

## Implemented MVP

- Fabric mod bootstrap and registrations.
- `Case` block + block entity, available in creative via `BlockItem`.
- Stable machine address (UUID-based 32-hex string), persisted in NBT.
- Minimal machine runtime with state (`OFF`, `RUNNING`, `CRASHED`) and server-side signal queue drain.
- Minimal component bus semantics with addressable `modem` component.
- Modem methods:
  - `open(port)`
  - `close(port)`
  - `closeAll()`
  - `isOpen(port)`
  - `broadcast(port, payload)`
  - `send(toAddress, port, payload)`
- Delivery rules:
  - Server-authoritative logic only.
  - Same dimension only.
  - Wireless range: 32 blocks (default, persisted).
  - Port validation: `1..65535`.
  - Payload MVP rule: one string, max 256 chars (truncated).
  - Outgoing delivery cap: max 20 delivered packets per source modem per tick.
- Incoming packet produces machine signal:
  - `modem_message(_, localAddress, remoteAddress, port, distance, payload)`
  - Signal handling is visible in server log.

## Commands for runtime testing

- `/oc_addr <x> <y> <z>` — print Case address.
- `/oc_open <x> <y> <z> <port>` — open modem port.
- `/oc_broadcast <x> <y> <z> <port> <msg...>` — send broadcast.
- `/oc_send <x> <y> <z> <toAddress> <port> <msg...>` — send directed packet.

Also, right-clicking a Case prints address + currently open ports.

## Manual verification scenario

1. Start dedicated server or singleplayer integrated server.
2. Place two `Case` blocks within 20 blocks in same dimension.
3. Read both addresses with `/oc_addr`.
4. Open port on both:
   - `/oc_open <case1> 1234`
   - `/oc_open <case2> 1234`
5. Broadcast from Case 1:
   - `/oc_broadcast <case1> 1234 hello`
   - Observe `modem_message` log on receiver machine.
6. Directed send:
   - `/oc_send <case1> <address_case2> 1234 ping`
   - Observe receiver log.
7. Negative checks:
   - Close target port → no signal delivered.
   - Move beyond 32 blocks → no signal delivered.

## Next milestones

- Expand component set beyond modem.
- Proper VM execution bridge (Lua/OpenOS phase).
- Add persistent node graph routing features and richer OC compatibility layers.
