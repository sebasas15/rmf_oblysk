# Design-Doc Evaluation — control.dispatch (invariant catalog) · round 1 · 2026-09-22

**Scope:** `services/rmf_oblysk/oblysk/docs/design/invariants/control.md`, `.../invariants/README.md`
**Altitude:** invariant-catalog
**Evaluator:** independent subagent
**Verdict:** This is a mature, self-aware catalog: all five admitted rows clear Predicate, Provenance,
Violation and Forbids with real force (every Forbids clause names a design a competent engineer would
plausibly ship), the UCA sweep's own arithmetic checks out exactly (24 cells, 6 clear, 18 hazard,
15/18 to HAZ-35), and the hazard-side enumeration is clean — both named hazards (HAZ-35, HAZ-15) have
an admitted parent row, so there is no orphaned sweep cell. The two defects found are both about
honesty of cross-reference rather than the rows themselves: the superproject hazard index was never
updated to carry these five new refinements, and one of the two "independent grounds" offered for
`INV-RMF-3` surviving `Minimal` against `INV-FP-14` does not hold up against the catalog's own
canonical interleaving. Neither defect invalidates an admitted row on its own; the second ground for
that Minimal claim is untouched and sufficient by itself.
**Grade:** B+ — rigorous row-by-row craft; two real cross-reference gaps
**Gate:** PASS — no open finding is critical or high

| ID | Sev | Type | Status | Title |
|----|-----|------|--------|-------|
| F1 | medium | inconsistency | fixed | `requirements/invariants.md`'s refinement column was never updated for these five rows |
| F2 | medium | inconsistency | fixed | `INV-RMF-3`'s Ground 1 against `INV-FP-14` is unsupported by the catalog's own canonical interleaving |
| F3 | low | inconsistency | fixed | §6 counts "three rejected mechanism candidates" against a rejection log that names two |

## Findings

### F1 — `requirements/invariants.md`'s refinement column was never updated for these five rows
- **Severity:** medium
- **Type:** inconsistency
- **Status:** fixed
- **Refs:** `services/rmf_oblysk/oblysk/docs/design/invariants/control.md` (Provenance rows for
  `INV-RMF-1..5`), `requirements/invariants.md` lines 90, 104, 124
- **Claim:** The catalog's own README leans on `requirements/invariants.md`'s refinement column as
  the authoritative record consulted *before drafting* ("Before drafting any row here, read
  `requirements/invariants.md`'s refinement column for the hazard you intend to parent to... It costs
  minutes and it is the difference between a rejection and a withdrawal"), and cites it by exact line
  number (HAZ-1 → `INV-FP-1` line 90, HAZ-15 → `INV-FP-14` line 104, HAZ-35 → `INV-IR-7` line 124 —
  all three citations verified accurate). But `requirements/invariants.md` was never updated with this
  catalog's own five admissions: `grep -n "INV-RMF" requirements/invariants.md` returns nothing.
  HAZ-15's refinement cell still lists only `INV-FP-14`; HAZ-35's still lists only `INV-IR-7`. Every
  other group catalog in the repo set (`ha`, `ing`, `ir`, `aud`, `fp`) *does* appear in this index, so
  the omission is specific to this cycle's rows, not a structural absence.
- **Consequence:** A future cycle discovering invariants for any group that ties to HAZ-15 or HAZ-35 —
  including this repo's own still-unmanifested `control.deconflict`, which the README says will join
  this same catalog — will read the index, see only `INV-FP-14` / `INV-IR-7`, and never learn that
  `INV-RMF-1..5` exist. That is exactly the failure mode this catalog's own provenance narrative is
  built to prevent ("the correction Cycle B's first seam learned the hard way: the catalog that
  already parents to your own hazards is the first one to read"). A row could be re-admitted that
  clause 5 should have rejected, or a genuinely distinct row could be withdrawn against a phantom
  duplicate — the exact two outcomes the README says the read is supposed to prevent.
- **Confidence:** high the fact holds (verified by direct grep and line-read of both files); medium on
  severity — `services/rmf_oblysk` and two sibling submodules show as modified-but-uncommitted in the
  superproject working tree at time of review, and the repo's established pattern (`ha`, `ing`)
  bundles the index update with the submodule-pointer bump in one follow-up commit, so this may simply
  be mid-cycle rather than forgotten. It is nonetheless a live gap in a shared, load-bearing artifact
  right now.
- **Fix:** Add `INV-RMF-3` to HAZ-15's refinement cell and `INV-RMF-1`, `INV-RMF-2`, `INV-RMF-4`,
  `INV-RMF-5` to HAZ-35's refinement cell in `requirements/invariants.md`, in the same commit/PR that
  bumps the `rmf_oblysk` submodule pointer — matching the `ha`/`ing` precedent.
- **SMART:** **S** edit two table cells in `requirements/invariants.md` to add five `INV-RMF-*` IDs ·
  **M** `grep -c INV-RMF requirements/invariants.md` goes from 0 to 5 · **A** a two-line diff, no
  design change · **R** closes the exact gap this catalog's own admission procedure depends on for the
  next repo · **T** before or alongside the next submodule-pointer bump for `rmf_oblysk`
- **Backlog:** —
- **Closed 2026-09-22:** the edit landed as one pass over the table covering `INV-CDT-*`, `INV-RMF-*`
  and `INV-ADP-*` together. HAZ-15's cell now reads `INV-FP-14, INV-RMF-3`; HAZ-35's reads
  `INV-IR-7, INV-RMF-1, INV-RMF-2, INV-RMF-4, INV-RMF-5`. The stated measure holds —
  `INV-RMF-[0-9]*` occurrences in `requirements/invariants.md` went 0 → 5. The finding's medium-
  confidence read on severity was the right one: this was mid-cycle, and the bundled edit is the
  `ha`/`ing` precedent it names.

### F2 — `INV-RMF-3`'s Ground 1 against `INV-FP-14` is unsupported by the catalog's own canonical interleaving
- **Severity:** medium
- **Type:** inconsistency
- **Status:** fixed
- **Refs:** `control.md` `INV-RMF-3`'s Minimal clause and Interleaving clause;
  `services/edge_oblysk/docs/design/invariants/fast-path.md` `INV-FP-14`
- **Claim:** The catalog offers two "independent grounds" for `INV-RMF-3` surviving clause 5 against
  `INV-FP-14`. Ground 2 (release must be agreed by the committed robot, never inferred from silence —
  "has no counterpart anywhere in the repo set") is solid and, on inspection of `INV-FP-14`'s actual
  text, correct: `INV-FP-14` says nothing about release semantics. Ground 1 is weaker than presented.
  It argues: *"If `B` is awarded while `A`'s award is in flight and `A` completes first, `INV-FP-14` —
  at most one motion allocation is enacted — is satisfied, while this row is violated for the whole
  interval both commitments were live."* But the catalog's own canonical interleaving — the one used to
  justify the T0 tier and named as the formal-model target in
  `../verification/control.md §4` — has step 5 read: *"The original award arrives at `A`, which
  accepts it and begins executing. Both `A` and `B` now hold a live commitment for `r`."* Both parties
  execute. `INV-FP-14`'s own Violation clause is written generally — *"Two allocations, or two
  awarded tasks, carrying one verdict identity"* — with no textual restriction to the redelivery
  mechanism its Interleaving clause illustrates. Read against that Violation clause, the canonical
  interleaving (both `A` and `B` physically executing for one request identity) appears to violate
  `INV-FP-14` too, which is the opposite of what Ground 1 claims for "the" scenario. Ground 1's own
  hypothetical ("`A` completes first") is a *different*, underspecified trace from the one the catalog
  otherwise treats as canonical, and never explains the mechanism by which `A`'s later, accepted award
  would fail to result in `A` executing.
- **Consequence:** The Minimal clause's stated confidence ("two independent grounds") overstates what
  is actually shown. If a future reader relies on Ground 1 as written — rather than on the correct,
  sufficient Ground 2 — they could conclude the cardinality half of `INV-RMF-3` is genuinely orthogonal
  to `INV-FP-14`'s Violation clause on its face, when in fact the distinction that actually survives is
  narrower: `INV-FP-14`'s Scope (`fast-path ↔ allocator boundary`) and its formal obligation
  (`dispatch-exactly-once`, a lost-ack-races-a-retry-across-a-crash shape) are specifically about
  *redelivery* of the same request from upstream, not about the dispatcher's own internal
  timeout-driven re-award — and that scope restriction, not the "completes first" hypothetical, is
  what actually does the work.
- **Confidence:** medium — resolving this fully requires the `INV-FP-14` catalog to state its Scope
  restriction inside the Violation clause itself (today the restriction lives only in Scope +
  Interleiving, one repo away, and is easy to miss on a plain read of the Violation clause). The
  admission of `INV-RMF-3` itself is very likely sound on Ground 2 alone; this finding is about the
  rigor of the written argument, not the outcome.
- **Fix:** Replace Ground 1's example with one that is actually consistent with the row's own
  canonical interleaving (e.g., argue directly from `INV-FP-14`'s Scope/Established-by, which names the
  redelivery-across-a-crash mechanism specifically, rather than from a hypothetical ordering that
  contradicts the interleaving used elsewhere on the same page), or drop Ground 1 and rely on Ground 2,
  which is independently sufficient and unaffected by this issue.
- **SMART:** **S** rewrite one paragraph (the Ground 1 bullet) in `control.md`'s `INV-RMF-3` Minimal
  clause · **M** the rewritten example must not contradict the row's own Interleaving clause · **A**
  a doc-only edit, no re-admission needed · **R** protects the catalog's central "Minimal checked
  across catalogs" claim from a reader finding a counterexample in its own text · **T** next revision
  pass on `control.md`
- **Backlog:** —

### F3 — §6 counts "three rejected mechanism candidates" against a rejection log that names two
- **Severity:** low
- **Type:** inconsistency
- **Status:** fixed
- **Refs:** `control.md §6` ("the three rejected mechanism candidates in §5 would all have moved under
  this swap — a sealed-bid close, an award-latency bound expressed in auction rounds, and a
  queue-depth alert on a queue that no longer exists"); `control.md §5` rejection log
- **Claim:** §5's rejection log has exactly one row that is unambiguously mechanism-shaped ("Bidding is
  a sealed-bid auction with a declared close", rejected on clauses 1/2 and explicitly tagged
  "Mechanism"). The "award-latency bound expressed in auction rounds" and "queue-depth alert on a
  queue that no longer exists" both trace to a *single* other row — "Award latency and queue depth are
  bounded and alerted" (rejected on clauses 4/5, reasoning tagged as an operability requirement, not
  "Mechanism"). §6's "three... candidates" phrasing implies three distinct rejected rows; the log
  contains two.
- **Consequence:** Minor — a reader auditing §6 against §5 row-for-row will count two log entries
  against a claim of three, a small friction rather than a substantive gap, since the underlying
  observation (both halves of the bundled candidate would move under the swap) is true.
- **Confidence:** high the count is off; low impact.
- **Fix:** Say "two rejected candidates, three of their claims" or similar, or split the bundled
  §5 row if the two halves are meant to be tracked separately going forward.
- **SMART:** **S** one clause in §6's closing sentence · **M** count matches on re-read · **A**
  trivial · **R** keeps the page's own self-check claims exact, which is the standard this catalog
  otherwise holds itself to · **T** next revision pass
- **Backlog:** —

## Remediation record — 2026-09-22

| Finding | Edit | Verified by |
|---|---|---|
| F3 — §6 counts "three rejected mechanism candidates" against a five-row rejection log | Reworded: **two** of §5's rejections are mechanism-shaped and between them they would have moved three phrasings. The parenthetical names the miscount. | §5 has five rows: one mechanism (clause 1/2), one operability (clause 4/5), three clause-5 duplications. |
| F2 — `INV-RMF-3`'s Minimal Ground 1 is contradicted by this row's own T0 interleaving | Ground 1 rewritten around the case that actually distinguishes the rows: `B` awarded while `A`'s award is in flight, `A` executes to completion, `B`'s commitment released without `B` ever starting. **One** motion allocation, so `INV-FP-14` is satisfied; two live commitments, so this row is violated. The clause now says explicitly that the canonical `Interleaving` narrative is *not* that case — there both robots execute, which violates `INV-FP-14` too. | The distinguishing case turns on whether the race resolves before the second robot moves; `INV-FP-14`'s predicate quantifies over *enacted* allocations and this row over *live commitments*, which the rewritten clause now names as the difference. |

F1 stays **open**: `requirements/invariants.md` genuinely does not yet list `INV-RMF-*` under HAZ-15
or HAZ-35. That is a scheduled superproject step of this same cycle, held until all three seams' rows
exist so one edit covers `INV-CDT-*`, `INV-RMF-*` and `INV-ADP-*` together. Left open rather than
`deferred` — there is a named step, not an owner-less intention. The evaluator is right that the gap
breaks the very "read the index before drafting" discipline this catalog's README champions, which
is why it is not being closed by narrowing the finding.

**Gate: PASS** — F1 and F2 are `medium`; no open finding at `critical` or `high`.
