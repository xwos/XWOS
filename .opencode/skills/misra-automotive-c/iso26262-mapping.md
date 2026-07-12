# ISO 26262 ASIL Mapping — MISRA C:2012 Rules

## How to Read This File

| Column | Meaning |
|---|---|
| Rule | MISRA C:2012 rule number |
| MISRA Category | Mandatory / Required / Advisory |
| ASIL Relevance | Which ASIL levels this rule is specifically critical for |
| Severity | CRITICAL / HIGH / MEDIUM / LOW — for the review report |
| ISO 26262 Clause | The ISO 26262 Part 6 clause this rule supports |

**ASIL levels (lowest → highest safety integrity):**
- **QM** — Quality Management only (no functional safety requirement)
- **ASIL A** — lowest safety integrity
- **ASIL B** — moderate
- **ASIL C** — high
- **ASIL D** — highest safety integrity (e.g. throttle control, braking)

---

## ASIL D — Safety Critical Rules
*Must be compliant in any ASIL D software element. Violation = immediate safety risk.*

| Rule | MISRA Category | Rule Description | Severity | ISO 26262 Clause |
|---|---|---|---|---|
| 1.3 | Mandatory | No undefined/critical unspecified behaviour | CRITICAL | Part 6, §8.4.3 |
| 2.1 | Mandatory | No unreachable code | CRITICAL | Part 6, §8.4.3 |
| 14.3 | Mandatory | Controlling expressions not invariant | CRITICAL | Part 6, §8.4.3 |
| 15.1 | Mandatory | No goto | CRITICAL | Part 6, §8.4.4 (no jumps) |
| 17.2 | Required | No recursion | CRITICAL | Part 6, §8.4.4 (bounded call depth) |
| 17.4 | Mandatory | All paths return a value | CRITICAL | Part 6, §8.4.3 |
| 21.3 | Mandatory | No dynamic memory allocation | CRITICAL | Part 6, §8.4.6 (no heap) |
| 13.2 | Mandatory | Side effects in evaluation order | CRITICAL | Part 6, §8.4.3 |
| 9.1 | Required | Objects set before use | HIGH | Part 6, §8.4.3 |
| 15.6 | Required | Compound statements for bodies | HIGH | Part 6, §8.4.4 |
| 15.7 | Required | else on all if-else if | HIGH | Part 6, §8.4.4 |
| 16.4 | Required | switch has default | HIGH | Part 6, §8.4.4 |
| 16.3 | Required | break/return ends every case | HIGH | Part 6, §8.4.4 |
| 10.1 | Required | No inappropriate essential type ops | HIGH | Part 6, §8.4.3 |
| 10.3 | Required | No narrowing without explicit cast | HIGH | Part 6, §8.4.3 |
| 10.6 | Required | Widen composite expressions explicitly | HIGH | Part 6, §8.4.3 |
| 11.3 | Required | No cast between pointer-to-object types | HIGH | Part 6, §8.4.3 |
| 21.8 | Mandatory | No abort/exit/system | HIGH | Part 6, §8.4.6 |

---

## ASIL C — High Safety Integrity Rules

| Rule | MISRA Category | Rule Description | Severity | ISO 26262 Clause |
|---|---|---|---|---|
| 8.7 | Required | Internal functions marked static | HIGH | Part 6, §8.4.5 |
| 8.13 | Advisory | const-qualify pointer params | HIGH | Part 6, §8.4.5 |
| 10.4 | Required | Same essential type operands | HIGH | Part 6, §8.4.3 |
| 10.5 | Advisory | No inappropriate essential type casts | MEDIUM | Part 6, §8.4.3 |
| 10.8 | Required | No cast of composite expression to wider type | MEDIUM | Part 6, §8.4.3 |
| 14.1 | Required | No float loop counters | HIGH | Part 6, §8.4.4 |
| 14.4 | Mandatory | Controlling expression essentially Boolean | HIGH | Part 6, §8.4.4 |
| 15.4 | Advisory | Single break per loop | MEDIUM | Part 6, §8.4.4 |
| 15.5 | Advisory | Single exit from function | MEDIUM | Part 6, §8.4.4 |
| 17.7 | Required | Return values shall be used | HIGH | Part 6, §8.4.5 |
| 20.7 | Required | Macro parameters parenthesised | MEDIUM | Part 6, §8.4.5 |
| 20.14 | Required | Matching #endif in same file | MEDIUM | Part 6, §8.4.5 |
| 2.2 | Mandatory | No dead code | MEDIUM | Part 6, §8.4.3 |
| 2.3 | Advisory | No unused type declarations | LOW | Part 6, §8.4.3 |

---

## ASIL B — Moderate Integrity Rules

| Rule | MISRA Category | Rule Description | Severity | ISO 26262 Clause |
|---|---|---|---|---|
| 4.6 (D.4.6) | Advisory | Fixed-width typedefs for size/signedness | MEDIUM | Part 6, §8.4.3 |
| 5.1 | Required | Unique external identifiers (31 chars) | MEDIUM | Part 6, §8.4.5 |
| 5.3 | Required | No identifier hiding | MEDIUM | Part 6, §8.4.5 |
| 7.1 | Required | No octal constants | MEDIUM | Part 6, §8.4.3 |
| 7.2 | Required | U/u suffix on unsigned constants | MEDIUM | Part 6, §8.4.3 |
| 7.4 | Required | String literals to const char * only | MEDIUM | Part 6, §8.4.3 |
| 8.4 | Required | Compatible declarations visible | MEDIUM | Part 6, §8.4.5 |
| 11.5 | Advisory | No void * to object * | MEDIUM | Part 6, §8.4.3 |
| 11.8 | Required | No cast removing const/volatile | HIGH | Part 6, §8.4.3 |
| 12.1 | Advisory | Explicit operator precedence | MEDIUM | Part 6, §8.4.3 |
| 12.2 | Required | Shift count within bit width | HIGH | Part 6, §8.4.3 |
| 17.1 | Mandatory | No stdarg.h | MEDIUM | Part 6, §8.4.5 |
| 17.3 | Mandatory | No implicit function declarations | MEDIUM | Part 6, §8.4.5 |
| 20.1 | Required | #include only at top of file | LOW | Part 6, §8.4.5 |
| 20.4 | Required | No macro redefining keyword | MEDIUM | Part 6, §8.4.5 |
| 21.6 | Required | No stdio in production | MEDIUM | Part 6, §8.4.6 |
| 21.7 | Required | No atof/atoi/atol/atoll | MEDIUM | Part 6, §8.4.6 |

---

## ASIL A — Baseline Integrity Rules

| Rule | MISRA Category | Rule Description | Severity |
|---|---|---|---|
| 2.6 | Advisory | No unused labels | LOW |
| 2.7 | Advisory | No unused parameters | LOW |
| 4.1 | Required | Terminated escape sequences | LOW |
| 4.2 | Required | No trigraphs | LOW |
| 8.1 | Required | Explicit type specifiers | LOW |
| 8.9 | Advisory | Block scope for single-use objects | LOW |
| 9.3 | Required | Arrays fully or not initialised | LOW |
| 11.4 | Advisory | No int↔pointer cast (with noted exception) | LOW |
| 13.6 | Mandatory | No side effects in sizeof | LOW |
| 16.1 | Required | Well-formed switch | LOW |
| 16.5 | Required | default first or last in switch | LOW |
| 20.5 | Advisory | No #undef | LOW |
| 20.10 | Advisory | No # and ## | LOW |

---

## ASIL Classification Quick Reference — Automotive Subsystems

| Subsystem | Typical ASIL | Key rules to enforce |
|---|---|---|
| Electric Power Steering | ASIL D | 1.3, 15.1, 17.2, 21.3, 14.4, 17.4 |
| ABS / ESC | ASIL D | 1.3, 15.1, 17.2, 21.3, 14.4, 17.4 |
| Airbag ECU | ASIL D | 1.3, 17.2, 21.3, 9.1, 15.6, 15.7 |
| Engine Control (throttle) | ASIL D | 1.3, 21.3, 17.2, 14.3, 10.3 |
| Battery Management (EV/HEV) | ASIL C/D | 21.3, 17.2, 10.6, 12.2, 15.7 |
| Transmission Control | ASIL C | 15.7, 16.4, 17.2, 10.4, 9.1 |
| ADAS Camera/Radar fusion | ASIL B/C | 17.2, 21.3, 10.1, 10.6, 11.3 |
| Instrument Cluster | ASIL A/B | 7.2, 5.3, 8.13, 15.6 |
| Infotainment | QM | General coding quality |
| Telematics / OTA | QM/ASIL A | 21.3 if mixed with ASIL partition |

---

## Deviation Record Template

When a Required rule cannot be complied with, document a formal deviation:

```
MISRA C:2012 Deviation Record
──────────────────────────────────────────────────────────────────
Deviation ID       : DEV-<MODULE>-<NNN>
Rule               : MISRA C:2012 Rule <X.Y>
Rule Category      : Required
ASIL               : <A|B|C|D>
Project            : <project name>
Module             : <module/file name>
Deviation scope    : <line range or function name>

Non-Compliant Code :
  <code snippet>

Rationale          :
  <Why compliance is not reasonably practicable — e.g. hardware constraint,
   third-party library, performance requirement>

Risk Assessment    :
  <Describe the risk introduced by this deviation and why it is acceptable>

Compensating measures :
  <What additional measures (code review, testing, analysis) compensate
   for the non-compliance>

Approved by        : <safety engineer name / date>
Review date        : <YYYY-MM-DD>
──────────────────────────────────────────────────────────────────
```
