# misra-automotive-c

> MISRA C:2012 code review skill for automotive embedded C development.
> Reviews pasted C code, flags violations with rule numbers and ASIL classification,
> and provides a MISRA-compliant replacement for every non-compliant line.

---

## What This Skill Does

When you paste C code and say **"misra check"**, **"review my code"**, or **"misra review"**, the agent:

1. Scans for **Mandatory** rule violations (never permitted)
2. Checks **Required** rules (permitted only with formal deviation)
3. Reports each violation with:
   - MISRA C:2012 rule number and category
   - ASIL A–D classification (ISO 26262 relevance)
   - The exact non-compliant line
   - A plain-English explanation of why it violates the rule
   - A ready-to-use MISRA-compliant replacement
4. Outputs a **summary table** with total violation counts by category and ASIL level

---

## Trigger Phrases

Say any of these to activate the skill:
- `misra check`
- `misra review`
- `review my c code`
- `automotive c`
- `embedded c review`
- `check misra`
- `iso 26262`
- `asil`

Or just paste C code — the agent will offer a review.

---

## Example Usage

**You:**
```
misra check:

uint8_t compute_torque(int speed, int load) {
    uint8_t result;
    if (speed > 6000) {
        result = 255;
        return result;
    } else if (load > 100) {
        result = load * 2;
    }
    return result;
}
```

**Agent output:**
```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VIOLATION #1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Rule        : MISRA C:2012 Rule 8.1 / D.4.6
Category    : Required
ASIL        : B
Severity    : MEDIUM

Location    : Function signature — parameters use 'int'

Non-Compliant Code:
  uint8_t compute_torque(int speed, int load)

Why it violates Rule D.4.6:
  Parameters use the basic 'int' type. For automotive embedded code, fixed-width
  typedefs must be used so the bit-width is explicit and portable across compilers
  and platforms.

MISRA-Compliant Replacement:
  uint8_t compute_torque(int16_t speed_rpm, uint8_t load_pct)

Explanation of fix:
  Replaced 'int' with int16_t for speed (can be negative for reverse) and uint8_t
  for load percentage (0–100). Types now have explicit, guaranteed widths.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
...
```

---

## Files in This Skill

| File | Contents |
|---|---|
| `SKILL.md` | Agent instructions and review workflow |
| `misra-mandatory.md` | All Mandatory rules with examples |
| `misra-required.md` | Key Required rules with examples |
| `types-and-casting.md` | Essential type model, fixed-width types, casting |
| `memory-embedded.md` | No heap, volatile, ISRs, stack discipline |
| `control-flow.md` | goto, braces, switch, if-else chains, bounded loops |
| `preprocessor.md` | Macros, include guards, #undef, token pasting |
| `iso26262-mapping.md` | ASIL A–D mapping for all key rules + deviation template |
| `LICENSE` | MIT-0 |

---

## Scope

- **Standard:** MISRA C:2012 (Third Edition — C99/C90)
- **Safety standard:** ISO 26262 Part 6 (ASIL A–D mapping)
- **Target:** Automotive ECU / microcontroller embedded C
- **NOT covered:** AUTOSAR C++, MISRA C++, CERT-C (separate skills)

---

## Installing via ClawHub

```bash
npm i -g clawhub
clawhub install <your-username>/misra-automotive-c
```

Or from the ClawHub web UI: search `misra-automotive-c`.

---

## Installing Locally (Without ClawHub)

```bash
# Copy this folder into your OpenClaw skills directory
cp -r misra-automotive-c/ ~/.openclaw/skills/

# Or into your project workspace
cp -r misra-automotive-c/ ./skills/
```

Restart OpenClaw and the skill loads automatically.

---

## Publishing Updates

```bash
# After editing, bump version in SKILL.md frontmatter then:
clawhub publish ~/.openclaw/skills/misra-automotive-c \
    --slug misra-automotive-c \
    --version 1.0.1 \
    --changelog "Added Rule 12.2 shift count examples"
```

---

## Known Limitations

- This skill performs **AI-assisted review** — not a substitute for certified MISRA checkers (PC-lint Plus, Helix QAC, Polyspace, CodeSonar)
- **Undecidable rules** (those requiring full program analysis) cannot be fully checked from a code snippet — the skill will flag likely violations but cannot guarantee completeness
- For **formal MISRA compliance certification**, use this skill alongside a certified static analysis tool
- MISRA C:2023 amendments (AMD2–AMD4) are not yet covered — use the base C:2012 rules

---

## Contributing

Issues and improvements welcome. Fork, edit, and open a PR on ClawHub.
