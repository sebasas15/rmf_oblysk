# rmf_oblysk (overlay) — Component Architecture

Repo-level architecture for the **oblysk-authored overlay** on this fork of `open-rmf/rmf`.
Strategic context lives in the superproject's `architecture_v3/` (`@ref control`, `@ref rmf_eval`);
group-altitude design lives in `design/pages/04-control-rmf-core.md`; component-altitude design lives
in [`docs/design/`](docs/design/README.md).

A fact lives at exactly one altitude. This page owns the **overlay's boundary** — what oblysk builds
on top of the vendor core, what it consumes from it, and where the line between them runs. It does
not restate component interfaces (those are each page's §2) and it does not restate capability levels
(those are each page's §12.1).

## 0. Why this page is under `oblysk/`

`services/rmf_oblysk` is a mirror of `open-rmf/rmf`. Its root `README.md`, `docs/`, `media/`,
`scripts/` and `rmf.repos` are **upstream files**, and `git merge upstream/main` runs against them —
`docs/` already holds upstream's `Project-Roles.md` and `Development-and-Release.md`. Placing an
oblysk design tree at `docs/design/` would put authored content directly on that merge surface, where
every upstream sync becomes a conflict to resolve in a directory nobody here owns.

Everything oblysk writes therefore lives under `oblysk/`, which exists for exactly this. Nothing
outside it is modified by this repo's own cycles.

`tools/check_stage_vector.py`'s `collect_seams` walks the whole repository root rather than a fixed
glob, so a `§12.1` manifest under `oblysk/docs/design/` is collected exactly as one in a
conventionally-shaped repo. That was verified rather than assumed before this tree was created.

## 1. Repo boundary

### Component overview

This overlay builds **the commitment relation between robots and work** — which robot is bound to
which task, and whether that binding can be honoured. Its controlled process is that relation, not
the auction that produces it.

The framing matters because it is what makes the component have invariants at all. Read as a
*scheduler*, a dispatcher is about throughput and fairness and carries none; every row in
[`docs/design/invariants/control.md`](docs/design/invariants/control.md) is a way a commitment can be
made that the floor cannot honour, or an accepted artifact can come to sit in neither of its two
legitimate end states.

The repo owns two census seams (`architecture_v3/pages/51-staging-ladder.md §0`):

| Seam | Status | Page |
|---|---|---|
| `control.dispatch` | catalog authored 2026-09-21; the `§12.1` manifest lands with the component page in the same PR | [`docs/design/control/dispatch.md`](docs/design/control/dispatch.md) |
| `control.deconflict` | **unmanifested** — carried in every stage vector because it co-admits HAZ-1 with `fastpath.sev` | — |

**The vendor line.** `rmf_task`, `rmf_traffic`, the API server and `rmf-web` are upstream; this
overlay does not fork their behaviour. What it owns is the set of obligations the dispatcher must
meet regardless of which implementation sits behind it — which is why the mechanism-swap check
([`invariants/control.md §6`](docs/design/invariants/control.md#6-mechanism-swap-self-check))
replaces the bid/award auction with a static assignment table and finds that no row moves. If a row
ever moves under that swap, it was describing upstream rather than constraining it, and it belongs in
the design page's §6.

### Owned components

| Component | Realizes | Designed? |
|---|---|---|
| **Commitment authority** — accepts, auctions, filters, awards, releases, refuses | CTL-6, CTL-15 | **yes** — reduced page |
| **Refusal surface** — the declared, enumerable reason set a refusal draws from | CTL-15 | **yes** — same page, §2 |

**On the refusal surface being its own component.** It is an enum in every implementation sense. It
is a designed component here because `INV-RMF-5` is a predicate over *it* rather than over the
dispatcher's code: the row is decided by enumerating the declared reason set against the
operationally distinguishable outcomes, with nothing running. A design that leaves refusal reasons to
whatever strings the implementation happens to emit puts that row somewhere with no page.

### Consumed contracts (referenced, not owned)

| Contract | Owner | Consumed by |
|---|---|---|
| `ValidatedTaskRequest` (`oblysk_envelope`) | **superproject** — `design/pages/00-component-catalog.md:87` ratifies it as the SEV's output and the Dispatcher's only legal input | §2 — cited, never restated |
| the movement-permission verdict | `fastpath.sev` (`edge_oblysk`), `INV-FP-1` | §2 — consumed; this overlay asserts nothing about whether a verdict is sound |
| duplicate-delivery absorption at this boundary | `ingest`/`fast-path`, `INV-FP-14` | the obligation lands here while the row lives there — see [`invariants/README.md §5`](docs/design/invariants/README.md#5-cross-group-ties) |
| the schedule of record and its single-writer discipline | `ha.*` (`edge_oblysk`), HAZ-19 | §2 — commitments become reservations; who may write the schedule is not this overlay's |
| the command regime | `ha.*` (`edge_oblysk`), CTL-8/CTL-10 | consumed as context; no row here is gated on a regime change |

**The `INV-FP-14` row is the one worth reading twice.** It is the only consumed contract that places
an obligation *on this component while being owned elsewhere*. `INV-FP-14` requires a duplicate
delivery of the same request to be absorbed **at the receiving boundary**, and the receiving boundary
is the dispatcher. The overlay's own row, `INV-RMF-3`, covers the strictly different case of
committing twice on its own initiative after a timeout — a failure that arrives through no delivery
at all.

### Requirements this overlay satisfies vs. consumes

| | Requirements |
|---|---|
| **Satisfies** | CTL-15 — capability matching as a hard filter, bounded progress to an end state, and a refusal that names an actionable reason |
| **Satisfies in part, jointly** | CTL-6 — the *dispatch* half: a validated request becomes one commitment. The *gate* half (that nothing reaches here un-validated, RMF-API path included) is `INV-FP-1`'s, and duplicating it was rejected on clause 5 |
| **Consumes** | CTL-1 (the schedule), CTL-8/CTL-10 (the regime), EFP-2 (the verdict's meaning) |
| **Out of scope here** | CTL-11 (nav graph and PTP), CTL-13 (control-plane observability — `adapters_oblysk`), and the traffic negotiation `rmf_traffic` performs, which `control.deconflict` will own when it is manifested |

## 2. In-process vs. out-of-process boundaries

**In-process with the commitment authority:** acceptance, the auction, the eligibility filter, award
and release. These are in-process because `INV-RMF-3` is a statement about *at most one live
commitment at every instant*, and an out-of-process award step introduces exactly the window between
"decided" and "recorded" that the row's T0 interleaving describes.

**Out-of-process, same zone (OT):** the SEV upstream, the fleet adapters downstream, the schedule of
record. Each is a test-double boundary; nothing here needs a real one, which is why no row in this
group tiers T2.

**Adversarial, and it is not an adversary:** the fleet. `INV-RMF-3`'s hazard is a robot that is
silent for a reason the dispatcher cannot distinguish from unavailability, and `INV-RMF-4`'s is a
robot whose capability changed after it bid. Neither is malice; both are a process model going stale
against a floor that moved. That is where this group's hazards live — §1 of the catalog says so in
the process-model table, which is the section most of the timing rows trace back to.

## 3. Build target graph

**None yet — design only.** The overlay carries `oblysk/README.md`, this page and `docs/design/`.
Upstream's own build (`colcon`, `rmf.repos`, the `ros:jazzy` devcontainer) is unchanged and is not
this overlay's to describe.

When targets land, §2's boundary dictates the shape: the commitment authority and its filter are one
unit; the refusal reason set is a declarative artifact readable by a test independently of the
dispatcher, because `INV-RMF-5` is checked against it rather than against running code; and the fleet
interface crosses a package boundary so `INV-RMF-3` and `INV-RMF-4` can inject a silent robot and a
mid-auction capability change respectively.

## 4. Interfaces & contracts

Per-component interfaces are each design page's §2 and are **not** restated here. The
interface-diagram convention is
[`docs/design/README.md §0`](docs/design/README.md#0-interface-diagram-convention).

### Commitment authority

Provided: `accept`, and the refusal surface. Required: the `ValidatedTaskRequest` contract, the
fleet's bid/capability reporting, and the schedule. Seam: the allocation strategy (strategy) —
deliberately a seam, because §6's mechanism swap confirms no row moves when the auction is replaced.
Full diagrams and the owner table:
[`docs/design/control/dispatch.md §2`](docs/design/control/dispatch.md#2-interfaces).

### Consumed contracts (pointers, not copied)

- the `ValidatedTaskRequest` envelope — `design/pages/00-component-catalog.md:87`.
- the G4 component table and the dispatcher's place in it — `design/pages/00-component-catalog.md`.

### Who consumes this overlay

The fleet adapters (`adapters_oblysk`, `control.fleet-adapter`) act on awarded commitments, and the
Traffic Schedule Node turns honoured commitments into reservations. The tie that will become
load-bearing once `control.fleet-adapter` is manifested is the award→execution boundary: a
commitment this overlay considers live and an adapter considers released is the same
two-views-of-one-state failure `INV-RMF-3` is written against, one hop further down.

## 5. Threading / real-time model

**No component in this overlay carries a real-time budget.** The dispatcher sits behind the
permission gate, and `INV-FP-12` (`edge_oblysk`) forbids any detect–triage–permit function from
depending on anything outside the facility — but it does not forbid the *loop* from depending on the
dispatcher, because the dispatcher is not on it. Dispatch is post-permission by construction: HAZ-1's
gate has already answered before a request is accepted here.

The tiers reflect a group whose hazards are about **concurrency and staleness**, not latency:

- **T0 — `INV-RMF-3` only**, and undischarged (`formal/single-live-commitment/`). The failure is an
  award in flight racing a timeout-driven release, and no deterministic simulation explores the
  schedule that exhibits it. It is the only T0 row in the group and no rung below L3 claims it.
- **T1 — `INV-RMF-2` and `INV-RMF-4`.** Both are injected-condition properties: no bidders, no
  eligible bidders, sustained overload; and a capability change at a controlled point in the auction.
- **T3 — `INV-RMF-1` and `INV-RMF-5`.** Both are decided by enumeration over declared structure with
  nothing running — a decision table over (required tags × reported tags), and the refusal reason set
  against the distinguishable outcomes.
- **T2 — none.** Nothing here is a claim about a real host, a real network or real timing.

That shape — one hard concurrency row at T0, the rest split between injected conditions and
enumeration, and nothing needing reality — is the clearest statement of what this overlay is. It is a
commitment component, not a latency one.
