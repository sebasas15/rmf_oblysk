# control-group Verification Strategy

How the `INV-RMF-*` rows in [`../invariants/control.md`](../invariants/control.md) are discharged:
the tier taxonomy, the harness each tier needs, the fault catalog, and the one formal obligation this
group owes.

This page owns the tier definitions and the `conform` anchors that
[`../control/dispatch.md §12.1`](../control/dispatch.md#121-capability-levels) cites by number. It
does not restate a `holds` or `admits` set.

## 1. Control harness

Four tiers, in the taxonomy shared across the repo set. What is distinctive about this group is that
its hardest row is **T0 and nothing is T2**: the dispatcher's failures are concurrency and staleness,
not latency or real-host behaviour.

| Tier | Means | Rows |
|---|---|---|
| **T0** | formal model, machine-checked | `INV-RMF-3` — **undischarged**, see §4 |
| **T1** | deterministic simulation, injected condition, no real timing | `INV-RMF-2`, `INV-RMF-4` |
| **T2** | real OS/host execution, real faults *and* real timing | **none** — see §5 |
| **T3** | property / decision table, no running system | `INV-RMF-1`, `INV-RMF-5` |

### 1.1 Property / decision table (T3)

Two rows are decided by enumeration over declared structure, with nothing running.

`INV-RMF-1` is a decision table over (required capability tags × reported capability tags), plus a
**perturbation oracle**: take one qualified and one unqualified bidder, vary the unqualified bidder's
cost, score or priority across its full range, and require the outcome not to change. That second
half is what distinguishes a filter from a steep penalty, and it is still T3 — it is two evaluations
of a pure function, not a driven system.

`INV-RMF-5` is an enumeration of the declared refusal reason set against the operationally
distinguishable outcomes: at minimum no-eligible-bidder, no-bids-within-bound, request-inadmissible,
capacity-refused. A set that cannot distinguish two outcomes a caller must respond to differently
fails by reading, with nothing running.

### 1.2 Deterministic sim (T1)

Two rows are properties under an injected fleet condition. The rig drives a synthetic fleet whose
bidding, capability reporting and silence are all controlled.

`INV-RMF-2` — under each of F1, F2, F3 and F8, every accepted request must leave the accepted set
through a start or a refusal within the bound. The oracle is the *end state*, not the latency, which
is why no real timing is needed.

`INV-RMF-4` — inject a capability change at a controlled point in the auction (F4, F9) and require
the award to reflect it or exclude the bidder. The interesting parameter is *where* in the auction
the change lands, which a deterministic rig can place exactly and a real fleet cannot.

### 1.3 No T2, and why

Nothing in this group is a claim about a real host, a real network, or real timing. The dispatcher
sits behind the permission gate; `INV-FP-12` (`edge_oblysk`) keeps the detect–triage–permit loop off
any off-facility dependency, and dispatch is post-permission by construction, so no row here carries
a real-time budget whose violation would be a safety property.

Every out-of-process boundary this group touches — the SEV upstream, the fleet adapters downstream,
the schedule of record — is a test-double boundary by construction. There is no analogue of
`audit`'s retention substrate or `conduit`'s real firewall: no row here is a claim about something a
double cannot stand in for.

### 1.4 The one place a sim is not enough

`INV-RMF-3` is T0 rather than T1 and the distinction is not conservatism. A deterministic simulation
explores the schedules its rig enumerates; the failure here is an award in flight racing a
timeout-driven release, and the interleaving that exhibits it requires the award to be *neither
delivered nor lost* at the instant the timer fires. A rig that models message delivery as an event it
schedules will explore the delays it was written to explore, and the defect lives in the one it was
not.

§4 records the obligation. Until it is discharged, no capability level claims `INV-RMF-3` below L3,
and **L3 is therefore unreachable** — a rung claiming an undischarged T0 row is the defect the whole
staging ladder exists to prevent.

## 2. Fault catalog

The faults this group's rows are discharged against. Each is injectable at a named boundary.

| # | Fault | Injected at | Rows it exercises |
|---|---|---|---|
| **F1** | no bids arrive within the bound | synthetic fleet, all bidders silent | `INV-RMF-2` |
| **F2** | bids arrive, none eligible | synthetic fleet, all bidders lack a required tag | `INV-RMF-2`, `INV-RMF-1` |
| **F3** | sustained overload — arrival rate exceeds award rate indefinitely | request generator | `INV-RMF-2` — the accepted set must not grow without bound |
| **F4** | a bidder reports a capability loss mid-auction | synthetic fleet, timed report | `INV-RMF-4` |
| **F5** | the award message is delayed in flight | fleet transport stub | `INV-RMF-3` |
| **F6** | a committed robot is silent while executing | synthetic fleet, suppressed progress | `INV-RMF-3` — silence must not be read as availability |
| **F7** | the bid timeout expires with an award still in flight | F5 + timer control | `INV-RMF-3` — **the T0 interleaving**; a sim can stage it but cannot show the absence of neighbours |
| **F8** | the caller withdraws a request after acceptance | request generator | `INV-RMF-2` — withdrawal is an end state, not a stall |
| **F9** | eligibility is decided against an observation older than the bound | fleet state cache, aged | `INV-RMF-4` |

**F6 is the one to design for first.** A robot executing quietly and a robot that has fallen over are
indistinguishable without an explicit release protocol, and every throughput-motivated design resolves
that ambiguity in favour of re-awarding. That resolution is exactly what `INV-RMF-3` forbids, and F6
is the fault that makes the temptation concrete.

## 3. Fault × expected-response matrix

| Fault | Expected response | Row |
|---|---|---|
| F1 | refusal within the bound, reason `no-bids-within-bound` | `INV-RMF-2`, `INV-RMF-5` |
| F2 | refusal within the bound, reason `no-eligible-bidder` — **distinct from F1's**, because one is a standing property of the fleet and the other is transient | `INV-RMF-2`, `INV-RMF-5` |
| F3 | acceptance itself is refused once capacity is reached; the accepted set stays bounded | `INV-RMF-2` |
| F4 | the award reflects the loss, or the bidder is excluded; never awarded on the pre-loss observation | `INV-RMF-4` |
| F5 | no second commitment is created while the first award's fate is unknown | `INV-RMF-3` |
| F6 | the commitment stays live; silence is not a release | `INV-RMF-3` |
| F7 | at most one live commitment at every instant — the property the T0 model must establish | `INV-RMF-3` |
| F8 | the request leaves the accepted set; withdrawal is a stated end state | `INV-RMF-2` |
| F9 | the stale observation is not usable for eligibility; refresh or exclude | `INV-RMF-4` |

## 4. T0 formal-model obligations

One obligation, **not discharged**.

**`formal/single-live-commitment/`** — `INV-RMF-3`.

*Property.* At every instant, each accepted request has at most one live commitment; a second
commitment is reachable only through a state in which the first has been explicitly released, and
release requires the committed party's agreement rather than being inferred from silence.

*The interleaving to hunt*, from the catalog row and extended with the model's own action sequence:

1. The dispatcher awards request `r` to robot `A` and arms an acknowledgement timer.
2. The award is in flight — neither delivered nor lost.
3. The timer expires. The dispatcher observes no acknowledgement.
4. The dispatcher concludes `A` is unavailable, marks `r` uncommitted, and awards it to robot `B`.
5. The original award arrives at `A`, which accepts and begins executing.

Both `A` and `B` hold a live commitment for `r`, and no party has done anything locally wrong. The
model must show that no reachable state has two live commitments for one request, under arbitrary
message delay and arbitrary timer expiry — which is what makes step 2 the load-bearing one: a model
that treats delivery as instantaneous or as loss proves nothing.

*Until it is discharged*, `INV-RMF-3` is claimed at **L3 only**, and
[`../control/dispatch.md §12.1`](../control/dispatch.md#121-capability-levels) records that L3 is
consequently unreachable and no stage vector may assign it.

## 5. What this group's tier shape says about it

Worth stating positively, because the absences are as informative as the presences.

**Nothing at T2.** Compare `audit`, whose `INV-AUD-3` needs a real write-once substrate, and
`conduit`, whose three hardest rows need a real boundary and a real identity authority. This group
has no such row: every boundary it touches is one a double can stand in for, and no claim it makes is
about what a real host does. A design that found itself needing a real fleet to discharge a row here
would be evidence that the row had drifted into describing the fleet rather than constraining the
dispatcher.

**One row at T0, and it is about concurrency rather than ordering.** The other T0 rows in the repo
set — `INV-AUD-1`'s place assignment, `INV-AUD-4`'s spool order, `INV-IR-2`'s durability ordering —
are all about *order*. `INV-RMF-3` is about *simultaneity*: two commitments existing at once, with
no ordering question between them. That is why the shape sweep in
[`../invariants/control.md §4`](../invariants/control.md#4-shape-sweep) answers "ordering /
monotonicity" with an explicit N/A while still carrying a T0 obligation, which would otherwise look
like an inconsistency.
