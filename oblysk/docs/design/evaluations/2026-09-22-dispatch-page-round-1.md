# Design-Doc Evaluation — control.dispatch (component page) · round 1 · 2026-09-22

**Scope:** `services/rmf_oblysk/oblysk/docs/design/control/dispatch.md`,
`.../docs/design/verification/control.md`, `services/rmf_oblysk/oblysk/ARCHITECTURE.md`
**Altitude:** component-detailed-design (reduced page: §1, §2, §6, §12 incl. §12.1, §13 written;
§3–5, §7–11 deliberately pointer-stubbed per the Stage 0 Cycle B manifest — not evaluated as gaps)
**Evaluator:** independent subagent
**Verdict:** What is written is disciplined and largely does establish what it claims: §1/§2 cite
`INV-` IDs rather than restating them, §6 names the exclusion each algorithm choice forces and ties it
to a row, §12/§12.1 pass the bidirectional coverage check (every `INV-` cited anywhere has a §12 row
and vice versa), the T0/L3-unreachable declaration is consistent between the catalog, the design page
and `check_stage_vector.py`, and the ladder's row-counting for the L2 hazard-closure claim is exactly
right (all four rows held at L2 are individually named in the closure narrative). Two real gaps: the
page's §2 claims `withdraw()` preserves `INV-RMF-2` in a way the catalog's own two-state predicate
does not actually establish, and the §12 test-plan's pass thresholds for three of `INV-RMF-2`'s four
fault rows are currently ungradable for the same undeclared-bound reason the catalog explicitly calls
out for `INV-RMF-4`/F9 — but does not call out here.
**Grade:** B — faithful to the catalog and honest about the vendor line, with one over-claimed
interface guarantee and one under-disclosed verification gap
**Gate:** PASS — no open finding is critical or high

| ID | Sev | Type | Status | Title |
|----|-----|------|--------|-------|
| F1 | medium | over-claim | fixed | `withdraw()`'s claim to preserve `INV-RMF-2` exceeds what the admitted predicate covers |
| F2 | medium | verification-gap | fixed | `INV-RMF-2`'s F1/F2/F3/F8 pass thresholds are ungradable without a declared bound, undisclosed unlike F9 |

## Findings

### F1 — `withdraw()`'s claim to preserve `INV-RMF-2` exceeds what the admitted predicate covers
- **Severity:** medium
- **Type:** over-claim
- **Status:** fixed
- **Refs:** `dispatch.md §2` interface table (`withdraw(RequestId)` row: *"`INV-RMF-2` — withdrawal is
  an end state, not a stall"*); `verification/control.md` F8 fault row and fault×response row;
  `invariants/control.md` `INV-RMF-2` Predicate/Violation clauses
- **Claim:** `INV-RMF-2`'s admitted Predicate is a strict two-state property: *"within a declared bound
  it either reaches a state in which some step of it can start, or it is refused... No accepted
  request is in neither state after the bound."* This phrasing is not incidental — "two legitimate end
  states" is the repeated, load-bearing phrase across HAZ-35, the catalog, and the design page (e.g.
  §1: "an accepted artifact can come to sit in neither of its two legitimate end states"). Withdrawal
  is neither "started" nor "refused." Yet §2's interface table asserts `withdraw()` preserves
  `INV-RMF-2` by treating withdrawal as a de facto third legitimate exit, and
  `verification/control.md`'s F8 fault row and fault×response matrix score it the same way ("the
  request leaves the accepted set; withdrawal is a stated end state"). Neither `INV-RMF-2`'s Violation
  clause nor its Forbids clause was amended to actually admit a third state, or to constrain what
  "withdrawn" may mean.
- **Consequence:** Nothing in the admitted row, as written, forbids a dispatcher from manufacturing a
  "withdrawal" to explain away a request it silently let pass its bound with no start and no refusal —
  precisely the HAZ-35 pattern `INV-RMF-2`'s Forbids clause exists to rule out ("Any design that
  reports acceptance to the requester as evidence of progress"). Because withdrawal sits outside the
  row's own two-state enumeration, a design that quietly reclassifies a stalled request as
  "caller-withdrawn" is not visibly excluded by any clause of the row `§2` cites as the interface's
  authority. The F8 fault, as currently scoped (caller-initiated, harness-controlled), does not
  exercise this — it tests a legitimate withdrawal, not a manufactured one — so no oracle in §12
  currently discriminates the manufactured case from the legitimate one.
- **Confidence:** medium — this could be resolved as a documentation-only fix if the design's intent
  is that "withdrawn" is always caller-attributable and observably distinct from "silently dropped"
  (e.g., a withdrawal always carries the caller's own request-id and timestamp, making a
  dispatcher-manufactured one detectable), but that distinction is not yet written anywhere reachable
  from `dispatch.md` §2 or the deferred §8 state diagram, so it is currently asserted rather than
  established.
- **Fix:** Either (a) amend `INV-RMF-2`'s admitted Predicate to enumerate three legitimate end states
  (started, refused, withdrawn-by-caller) and add a Forbids clause bullet excluding
  dispatcher-attributed withdrawal, or (b) narrow §2's interface table and the F8 fault description to
  state plainly that caller withdrawal is *outside* `INV-RMF-2`'s quantification (the request leaves
  the domain the row cares about, not that it satisfies the row), and add an explicit oracle
  distinguishing a genuine caller-initiated withdrawal from a dispatcher-side silent drop relabeled as
  one.
- **SMART:** **S** amend one Predicate/Forbids clause in `invariants/control.md` (or narrow one
  sentence in `dispatch.md §2` and `verification/control.md`'s F8 row) · **M** F8's expected-response
  row names an observable that distinguishes caller-initiated from dispatcher-initiated exit · **A**
  a clause edit plus one new oracle line, no architecture change · **R** closes the one path by which
  a HAZ-35-shaped silent stall could currently be self-certified as compliant · **T** before this
  catalog/page pair is used to gate an L2 claim, since L2 is where `INV-RMF-2` first holds
- **Backlog:** —

### F2 — `INV-RMF-2`'s F1/F2/F3/F8 pass thresholds are ungradable without a declared bound, undisclosed unlike F9
- **Severity:** medium
- **Type:** verification-gap
- **Status:** fixed
- **Refs:** `dispatch.md §12` test-plan table (`INV-RMF-2` row: pass threshold "end state reached under
  all four [F1, F2, F3, F8]... within the bound"); `invariants/control.md §4.1`; `verification/control.md
  §2–3` (fault catalog, fault×response matrix)
- **Claim:** `invariants/control.md §4.1` states plainly: *"Two rows quantify over a bound whose value
  is not declared anywhere yet. `INV-RMF-2`'s is the time within which an accepted request must start
  or be refused; `INV-RMF-4`'s is the maximum age of a capability observation..."* It then draws the
  consequence for exactly one of the two: *"F9 is not injectable as written until `INV-RMF-4`'s bound
  has a value, because 'an observation older than the bound' has no referent."* The identical
  consequence for `INV-RMF-2` is never drawn. It is real: F1 ("no bids within the bound"), F2 ("bids
  arrive, none eligible"), and F8 ("caller withdraws") are each scored in §12/§3 against "within the
  bound" — a liveness check that needs a concrete deadline to know when to declare failure — and no
  numeric bound for `INV-RMF-2` exists anywhere in the tree (§11, where it would be pinned per the
  reduced-page plan, is stubbed). F3's threshold ("the accepted set stays bounded") is a capacity
  bound, not a time bound, and is unaffected.
- **Consequence:** As written, three of `INV-RMF-2`'s four fault rows cannot presently produce a
  pass/fail verdict — the harness has no value to compare an elapsed time against — exactly the
  situation the catalog already flagged as worth recording "so it is not discovered at harness time"
  for the sibling row. Because the disclosure was made for `INV-RMF-4`/F9 but not for `INV-RMF-2`,
  someone building the T1 harness from §12/§3 alone (without independently re-deriving §4.1's general
  point) could reasonably believe `INV-RMF-2`'s harness is ready to run today, when it is blocked on
  the same missing constant.
- **Confidence:** high that the gap is real (verified: no bound value appears in `dispatch.md`,
  `verification/control.md`, or `ARCHITECTURE.md`; §11 is explicitly deferred); medium on whether this
  rises above "already implicitly covered" — §4.1's general sentence does technically mention
  `INV-RMF-2`'s bound, so a careful reader can derive the consequence themselves, which is why this is
  medium rather than high.
- **Fix:** Add one sentence to `invariants/control.md §4.1` (or `verification/control.md §1.2`)
  drawing the same conclusion already drawn for F9: name F1/F2/F8 as similarly not gradable — not
  "not injectable," since the fault itself needs no bound to inject, but not *scoreable* — until
  `INV-RMF-2`'s bound has a value.
- **SMART:** **S** one sentence in `§4.1` or `verification/control.md §1.2` · **M** the same
  "recorded so it is not discovered at harness time" treatment F9 already got · **A** doc-only · **R**
  prevents the T1 harness build from discovering this gap mid-implementation instead of at design time
  · **T** next revision pass, ideally alongside whatever change lands the actual bound value with the
  full `§11` uplift
- **Backlog:** —

## Remediation record — 2026-09-22

| Finding | Edit | Verified by |
|---|---|---|
| F1 — §2 claims `withdraw()` preserves `INV-RMF-2`, whose predicate is strictly two-state | `INV-RMF-2`'s predicate now names **three** declared exits — started, refused, or withdrawn by the caller that submitted it — and its Forbids clause gains the design the gap allowed: *any design in which the dispatcher may record a withdrawal the caller did not request*, which would let a silent stall be relabelled as one and satisfy the row vacuously. The clause states the division: withdrawal is the **caller's** act; HAZ-35's two states are the dispatcher's. | HAZ-35's own predicate covers what the dispatcher owes ("starts, or is refused with a stated reason"); a caller withdrawing its own request is not the dispatcher failing, but it *is* a third way the accepted set is left, and the predicate has to account for it or `withdraw()` preserves nothing. |
| F2 — the undeclared-bound disclosure covered `INV-RMF-4` only | §4.1 now records that **F1, F2 and F8 are equally ungradable**, because all three feed `INV-RMF-2`'s "within the bound" threshold and that bound is undeclared too. Four of the nine faults, not one. | `INV-RMF-2`'s §12 pass threshold reads "within the bound"; `§11` of the design page is deferred, so no value exists for either row's bound. |

**Gate: PASS** — both findings `medium` and now `fixed`; no open finding at `critical` or `high`.
