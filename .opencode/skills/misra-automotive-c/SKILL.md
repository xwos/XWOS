---
name: misra-automotive-c
version: 1.0.0
description: >
  Review automotive Embedded C code against MISRA C:2012 rules. Flags violations
  with rule numbers, ASIL classification, and provides a MISRA-compliant
  replacement for every non-compliant line.
metadata:
  openclaw:
    emoji: "🚗"
    requires:
      bins: []
    always: false
    triggers:
      - misra
      - misra check
      - misra review
      - review my c code
      - automotive c
      - embedded c review
      - check misra
      - iso 26262
      - asil
license: MIT-0
---

# MISRA Automotive C — Review Skill

## Quick Reference

| File | Coverage |
|---|---|
| `misra-mandatory.md` | Mandatory rules — never violate, no deviation allowed |
| `misra-required.md` | Required rules — must comply or raise formal deviation |
| `types-and-casting.md` | Essential types, fixed-width integers, casts, essential type model |
| `memory-embedded.md` | No dynamic alloc, volatile, ISR constraints, stack discipline |
| `control-flow.md` | goto, loops, switch, if-else chains, single exit |
| `preprocessor.md` | Macros, include guards, #undef, ## operator |
| `iso26262-mapping.md` | ASIL A–D relevance for key MISRA C:2012 rules |

---

## When to Activate

Activate this skill when the user:
- Pastes C code and asks for a MISRA review, compliance check, or audit
- Uses trigger words: "misra", "misra check", "misra review", "automotive c", "embedded c review", "iso 26262", "asil"
- Asks "is this MISRA compliant?" or "what rules does this violate?"

---

## Review Workflow — Follow These Steps in Order

### Step 1 — Parse the Code
- Read every line of the submitted C code carefully.
- Identify the context: function body, header file, ISR, macro definition, type declaration.
- Note all variable types, control flow paths, preprocessor directives.

### Step 2 — Check Mandatory Rules First (load `misra-mandatory.md`)
Mandatory rules have zero tolerance — flag every violation immediately.
Priority mandatory checks:
- Rule 1.3 — No undefined behaviour
- Rule 2.1 — No unreachable code
- Rule 13.2 — No side effects in expressions where order is unsequenced
- Rule 14.3 — Controlling expressions shall not be invariant
- Rule 15.1 — No `goto`
- Rule 17.1 — No `<stdarg.h>` features
- Rule 17.3 — No implicit function declarations
- Rule 17.4 — All exit paths of a non-void function shall have an explicit return
- Rule 21.3 — No `malloc`, `calloc`, `realloc`, `free`

### Step 3 — Check Required Rules (load `misra-required.md`, `types-and-casting.md`, `control-flow.md`, `preprocessor.md`)
Required rules must be complied with unless a formal deviation exists. Check:
- Typedef usage for fixed-width types (Rule 4.6 → D.4.6)
- Essential type model violations (Rules 10.1–10.8)
- All switch statements have a `default` clause (Rule 16.4)
- All `if-else if` chains end with `else` (Rule 15.7)
- Single-entry single-exit preferred (Rule 15.5)
- All struct/union members initialised (Rule 9.1)
- All macros parenthesised (Rule 20.7)
- No function-like macros unless unavoidable (Rule 20.10)

### Step 4 — Check Embedded / Memory Rules (load `memory-embedded.md`)
- `volatile` used on all hardware-mapped registers and shared ISR variables
- No dynamic memory allocation (heap) anywhere in safety-critical paths
- No recursion (Rule 17.2)
- Fixed-width integer types used exclusively for hardware registers and protocol fields
- ISR functions: no heavy computation, no blocking calls

### Step 5 — Map to ISO 26262 ASIL (load `iso26262-mapping.md`)
- For every rule violation found, report its ASIL classification
- ASIL D violations are highest severity — highlight prominently
- Report ASIL A violations but mark as lower urgency

### Step 6 — Generate the Violation Report

Output the report in EXACTLY this format for every violation found:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VIOLATION #<N>
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Rule        : MISRA C:2012 Rule <X.Y>
Category    : Mandatory | Required | Advisory
ASIL        : A | B | C | D | Not mapped
Severity    : CRITICAL | HIGH | MEDIUM | LOW

Location    : Line <N> — <brief description of what the line does>

Non-Compliant Code:
  <exact offending line(s)>

Why it violates Rule <X.Y>:
  <1–3 sentence plain-English explanation of the rule and why this code breaks it>

MISRA-Compliant Replacement:
  <corrected code, complete and ready to use>

Explanation of fix:
  <1–2 sentences explaining what changed and why it is now compliant>
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

After all individual violations, output a summary table:

```
REVIEW SUMMARY
──────────────────────────────────────────────────
Total violations   : <N>
  Mandatory        : <N>  ← must fix before any safety certification
  Required         : <N>  ← must fix or raise formal deviation
  Advisory         : <N>  ← recommended to fix

ASIL breakdown:
  ASIL D           : <N>  ← safety critical, fix immediately
  ASIL C           : <N>
  ASIL B           : <N>
  ASIL A           : <N>
  Not mapped       : <N>

Overall compliance status: FAIL | CONDITIONAL | PASS
──────────────────────────────────────────────────
```

---

## Code Generation Workflow

When asked to **generate** new automotive/embedded C code (not review existing code):

Always apply these rules unconditionally — no exceptions:

**Types:**
- Use `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `int8_t`, `int16_t`, `int32_t`, `int64_t` — never `int`, `unsigned int`, `long`, `char` for numeric data
- Use `bool` (from `<stdbool.h>`) for boolean values — never integer flags
- Always cast explicitly when converting between types — no implicit narrowing

**Memory:**
- Never use `malloc`, `calloc`, `realloc`, or `free`
- All variables initialised at declaration point
- Arrays are fixed-size, statically allocated
- Hardware register pointers declared `volatile`

**Control flow:**
- No `goto`
- All `if` / `else if` chains end with `else`
- All `switch` statements have a `default`
- All loops have a defined maximum iteration count
- Every non-void function has a single explicit `return` at the end when possible

**Functions:**
- All parameters and return types use fixed-width typedefs
- All return values checked by callers
- No recursion
- Maximum one level of pointer indirection unless justified

**Macros:**
- Function-like macros: all parameters and the whole expression parenthesised
- Prefer `static inline` functions over function-like macros
- Include guards on every header (`#ifndef FILENAME_H / #define FILENAME_H / ... / #endif`)

---

## Escalation Rules

- If the code contains **Rule 21.3 violations** (dynamic allocation) in any path reachable from a safety function, output a ⚠️ SAFETY CRITICAL banner at the top of the report before any violations.
- If **Rule 15.1** (goto) is found, flag it as ASIL D regardless of context.
- If **Rule 17.2** (recursion) is found in any function, trace the full call chain if visible and report it.
- If the code appears to be an **ISR** (function name contains `ISR`, `_IRQ`, `_Handler`, `_isr`, or has a `__attribute__((interrupt))` annotation), apply `memory-embedded.md` ISR rules with heightened strictness.
