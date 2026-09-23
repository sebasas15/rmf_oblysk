# Invariants

What must **always hold**, derived from the problem space **before** the design that has to hold it.
Verification of these rows — oracles, faults, tiers — lives in
[`../verification/control.md`](../verification/control.md); the two trees are peers, one page per
group.

Method, four-phase discovery, and dispatch rules: the `discovering-invariants` skill in the
superproject (`.claude/skills/discovering-invariants/`).

## 1. Provenance taxonomy

| Kind | Derived from | Home | ID | Admissible here |
|---|---|---|---|---|
| **hazard** | the controlled process and its failure modes — design-free by construction | superproject `requirements/invariants.md` | `HAZ-<n>` | no — traced **to** |
| **contract** | an obligation a group or component owes a peer — needs decomposition, not internals | this tree | `INV-<GROUP>-<n>` | **yes** |
| **mechanism** | a chosen algorithm's internals | the design page's `§6` | — | **no** |

A mechanism invariant is a *consequence*, not a discovery: it vanishes the moment the algorithm is
replaced. It may appear in a design page only while naming the `HAZ-`/`INV-` row it discharges.

**This group has an unusual exposure to mechanism, and it comes from the requirements themselves.**
CTL-6 and CTL-15 write the mechanism into their own text — "bid/award auction", "filter-before-cost",
"typed tag grammar". A requirement that names a mechanism is still a requirement, but a row phrased
in its vocabulary is a mechanism invariant with a requirement id attached. The separator is
[`control.md §6`](control.md#6-mechanism-swap-self-check)'s swap: replace the auction with a static
assignment table and see which rows move. None did, and the three rejected candidates in
[`§5`](control.md#5-rejection-log) all would have.

**Scope.** All five rows are `group`-scoped. Nothing here is `domain`-scoped, which is the right
shape for a component that binds work to robots inside one facility's control plane rather than
constraining the facility itself.

## 2. Admission gate

Five clauses, all mandatory. A candidate failing any one is rejected, not softened.

| # | Clause | Rejects |
|---|---|---|
| 1 | **Predicate** — a state predicate or trace property, mechanically checkable in principle | goals, "MUST do" obligations, aspirations |
| 2 | **Provenance** — `contract` + parent `HAZ-` ID, or `standalone(<reason>)` | orphans; mechanism invariants (no valid parent slot) |
| 3 | **Violation** — the observable that exhibits a breach, at a named interface or boundary | unfalsifiable rows |
| 4 | **Forbids** — at least one plausible design this rules out | true-but-idle rows that constrain nothing |
| 5 | **Minimal** — not implied by another admitted row | redundancy that inflates the catalog and the T0 bill |

**Clause 5 did the most work in this group, and it did it *before* drafting.** Two candidates were
rejected by reading the catalogs that already parent to this group's hazards, rather than by
discovering the overlap afterwards:

- *"The dispatcher acts only on a request that traversed the permission gate"* — `INV-FP-1`
  (`edge_oblysk`) already quantifies over every movement request enacted at the dispatch boundary.
- *"A retry of an already-dispatched request produces no second commitment"* — `INV-FP-14` already
  places absorption **at the receiving boundary**, which is this component.

The second is worth reading twice. `INV-FP-14`'s obligation *lands on this component* while the row
lives in another catalog. That is a cross-group tie (§5), not a gap — and the row that survived,
`INV-RMF-3`, is the strictly different case of the dispatcher committing twice on its own initiative
with no redelivery involved.

**The procedure this establishes.** Before drafting any row here, read
`requirements/invariants.md`'s refinement column for the hazard you intend to parent to, and read
every row it names. It costs minutes and it is the difference between a rejection and a withdrawal.

**Tier** is recorded per row but is **not** an admission criterion. It is derived, mapping the row
onto the T0–T3 taxonomy in
[`../verification/control.md §1`](../verification/control.md#1-control-harness).

## 3. ID grammar

`^INV-[A-Z]+-[0-9]+$`; group tokens mirror the group names — `RMF` here, as `AUD` does in
`audit_oblysk`, `CDT` in `transport_oblysk`, `IR` in `cognition_oblysk`, and `FP`, `HA`, `ING` in
`edge_oblysk`. Stable and undated; IDs are never reused — a retired row stays, marked
`withdrawn(<reason>)`.

**The token is `RMF`, not `CTL`.** `CTL-1 … CTL-16` are live requirement IDs in
`requirements/05-control-plane.md`, and `INV-CTL-6` sitting one glance from `CTL-6` in adjacent prose
is the collision `cognition_oblysk`'s catalog README records rejecting for `INV-COG`. `RMF` also
names the **family** rather than this one seam: `control.deconflict` is owned by this repo and will
be admitted into this same catalog, so the token must not read as "dispatcher".

Every row traces up to a `HAZ-` in the superproject catalog, or carries `standalone(<reason>)`. All
five rows here have a hazard parent; none is standalone.

## 4. Group pages

| Group | Page | Verification peer | Status |
|-------|------|-------------------|--------|
| control | [`control.md`](control.md) | [`../verification/control.md`](../verification/control.md) | authored 2026-09-21 — 5 admitted (`control.dispatch`) |

This repo owns two census seams (`architecture_v3/pages/51-staging-ladder.md §0`): `control.dispatch`,
manifested in Stage 0 Cycle B, and `control.deconflict`, still unmanifested and carried in every
stage vector because it co-admits HAZ-1 with `fastpath.sev`. When it is manifested it joins
`control.md` rather than getting its own catalog — the graining is per repo, not per seam.

**This repo is a fork, and the design tree lives off the merge surface.** `services/rmf_oblysk` is a
mirror of `open-rmf/rmf`; its root `README.md`, `docs/`, `media/` and `scripts/` are upstream files
that `git merge upstream/main` runs against. All oblysk-authored content lives under `oblysk/`.
`collect_seams` walks the whole repository root rather than a fixed glob, so a manifest here is found
exactly as one in a conventionally-shaped repo.

Discovery for `control` ran **inline**, per the skill's dispatch rule: no oblysk design existed to be
biased by. The **upstream sources were deliberately not read** — they are an implementation of the
thing being constrained, and reading them is precisely the contamination the skill's dispatch rule
exists to prevent. That a design exists *in the fork* does not make this a retrofit; what matters is
whether a design exists for the obligations being written, and none did.

## 5. Cross-group ties

Where one group's invariant assumes another group's guarantee.

**`Minimal` is checked across catalogs, not only within one** — a change this cycle made and worth stating, because the older phrasing ("`Minimal` is checked *within* a catalog") is what allowed a row to be admitted here and withdrawn two tasks later. Two candidates below were rejected on clause 5 against `edge_oblysk`'s catalog *before drafting*. A tie remains the place a surviving dependency is recorded; it is no longer the only place another catalog is consulted.

| Upstream | Downstream | What the pairing asserts | Which half each side owns |
|---|---|---|---|
| `INV-FP-1` (`edge_oblysk`) — every request enacted at the dispatch boundary has exactly one prior PASS verdict | *(no row here — rejected on clause 5)* | CTL-6's single-gate obligation is one claim about one boundary, and the boundary already has an owner. This group consumes the verdict and asserts nothing about it. | **fast-path** owns that a permission exists before the boundary. **control** owns nothing here, deliberately. |
| `INV-FP-14` (`edge_oblysk`) — one intent yields at most one enacted motion, absorbed **at the receiving boundary** | `INV-RMF-3` — at most one live commitment per request | **The obligation lands here while the row lives there.** `INV-FP-14` requires *this component* to absorb a duplicate delivery; `INV-RMF-3` forbids *this component* from committing twice on its own initiative. A receiver with perfect upstream idempotency can still double-award after a timeout. | **fast-path** owns duplicate *delivery*. **control** owns duplicate *decision*. |
| `INV-HA-18`, `INV-HA-20`, `INV-HA-21` — single-writer discipline over the schedule of record (HAZ-19) | *(no row here — rejected on clause 5)* | Commitments become reservations in the schedule, so this group writes to something another group owns the safety of. | **ha** owns who may write the schedule. **control** owns whether a commitment should have been made at all. |
| `INV-HA-8` — command age from issue to actuation | *(no row here, and **no row anywhere**)* | The first draft cleared CA1's wrong-timing cell against `INV-HA-8` and `INV-FP-1`. Neither holds it: `INV-FP-1` bounds the *authority's* currency at the moment the verdict was used, and `INV-HA-8` bounds a *command's* age downstream of issuance. A verdict issued at `T` and accepted at `T + Δ` satisfies both. | **control** owns not re-deciding a verdict it was handed. **Nobody owns `Δ`** — see [`control.md §7`](control.md#7-open-gaps-and-escalations). |

**Strength, recorded rather than smoothed.**

- The `INV-FP-14` tie is the **strongest on this page** and it was discovered rather than arranged:
  `INV-FP-14` was admitted in an earlier cycle, in another repo, by a sweep that never saw this one,
  and its predicate independently names the receiving boundary that turns out to be this component.
  Two catalogs agreeing on where an obligation lands, written years of cycles apart, is the closest
  thing to independent confirmation this method produces.
- The `INV-FP-1` tie is **asymmetric and worth re-checking**: nothing in `fast-path.md` records that
  a downstream consumer is relying on it, so a future narrowing of `INV-FP-1`'s scope — to exclude
  the RMF-API submission path, say — would silently open a hole here with no row anywhere noticing.
- The `INV-HA-18`/`20`/`21` tie is **latent**: this group does not write the schedule today, because
  nothing is built. It becomes live the moment a commitment is persisted, and the rejection in §5 is
  the record that the question was asked.

**One T0 obligation, undischarged.** `INV-RMF-3`'s is `formal/single-live-commitment/`, listed in
[`../verification/control.md §4`](../verification/control.md#4-t0-formal-model-obligations) as
deferred. `control.md`'s `§12.1` peer does not claim that row below L3. An undischarged T0 row that a
capability level claimed anyway would be the one defect the whole staging ladder exists to prevent.
