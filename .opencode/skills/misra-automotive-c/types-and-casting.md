# Types and Casting — MISRA C:2012 for Automotive Embedded C

## The Golden Rule
> **Never use `int`, `unsigned`, `long`, `short`, `char` for numeric data.**
> Always use fixed-width typedefs from `<stdint.h>`.

---

## Fixed-Width Type Reference

| Use case | Required type | Forbidden alternatives |
|---|---|---|
| 8-bit unsigned register value | `uint8_t` | `unsigned char`, `unsigned` |
| 8-bit signed value | `int8_t` | `char`, `signed char` |
| 16-bit unsigned (ADC, timer) | `uint16_t` | `unsigned short`, `unsigned int` |
| 16-bit signed | `int16_t` | `short`, `int` |
| 32-bit unsigned (CAN ID, timer) | `uint32_t` | `unsigned int`, `unsigned long` |
| 32-bit signed (speed, torque) | `int32_t` | `int`, `long` |
| 64-bit timestamp | `uint64_t` | `unsigned long long` |
| Boolean flags | `bool` | `int`, `uint8_t`, `_Bool` directly |
| Pointer-sized integer | `uintptr_t` | `unsigned int`, `unsigned long` |

---

## Essential Type Model (Rules 10.1–10.8)

MISRA C:2012 defines "essential types" — the effective type category for each expression:
- **Boolean** — `bool` only
- **Character** — `char`, only for character data (not numeric)
- **Signed integer** — `int8_t`, `int16_t`, `int32_t`, `int64_t`
- **Unsigned integer** — `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`
- **Floating point** — `float`, `double` (avoid in embedded unless FPU present)
- **Enum** — distinct category; enums are not integers in MISRA

### Rule 10.1 — Inappropriate Operand Type

Non-compliant:
```c
int32_t reg = 0xFF;
int32_t masked = reg & 0x0F;    /* bitwise op on signed essential type */
```
Compliant:
```c
uint32_t reg = 0xFFu;
uint32_t masked = reg & 0x0Fu;  /* bitwise op on unsigned essential type */
```

### Rule 10.3 — Narrowing Assignment

Non-compliant:
```c
uint32_t raw_adc = get_adc_value();    /* 0–4095 */
uint8_t  display = raw_adc;            /* narrowing — no cast */
```
Compliant:
```c
uint32_t raw_adc = get_adc_value();
uint8_t  display = (uint8_t)(raw_adc & 0xFFu);   /* explicit narrow with mask */
```

### Rule 10.4 — Mixed Signed/Unsigned Operations

Non-compliant:
```c
int32_t  signed_val   = -5;
uint32_t unsigned_val = 10u;
uint32_t result = unsigned_val + signed_val;    /* implicit signed→unsigned */
/* -5 as uint32_t = 4294967291 — catastrophic */
```
Compliant:
```c
int32_t signed_val = -5;
int32_t base       = 10;
int32_t result     = base + signed_val;   /* both signed */
```

### Rule 10.6 — Widening Composite Expressions

Non-compliant (common automotive bug):
```c
uint16_t engine_speed = 6000u;    /* RPM */
uint16_t gear_ratio   = 4u;
uint32_t output_speed = engine_speed * gear_ratio;
/* Multiplication done in uint16_t — 6000 * 4 = 24000 — overflows uint16_t! */
/* uint16_t max = 65535 — 24000 fits but: */
uint16_t high_speed = 20000u;
uint32_t result2    = high_speed * gear_ratio;   /* 80000 overflows uint16_t */
```
Compliant:
```c
uint32_t output_speed = (uint32_t)engine_speed * (uint32_t)gear_ratio;
```

---

## Casting Rules

### Explicit Casts Required — No Silent Implicit Conversions

Non-compliant:
```c
uint32_t adc_val = 1024u;
uint8_t  byte    = adc_val;        /* implicit narrowing */
float    f       = adc_val;        /* implicit int→float */
```
Compliant:
```c
uint8_t  byte = (uint8_t)(adc_val & 0xFFu);
float    f    = (float)adc_val;
```

### Pointer Casting — Hardware Register Access Pattern

The correct pattern for memory-mapped peripheral registers:
```c
/* MISRA Deviation: Rule 11.4 */
/* Rationale: MCU peripheral register requires absolute address casting */
/* Risk assessment: Address defined in SoC datasheet — fixed, reviewed in design */
/* Deviation approved by: <safety engineer sign-off> */

#define UART1_BASE    (0x40011000u)
#define UART1_SR      (*(volatile uint32_t *)(UART1_BASE + 0x00u))
#define UART1_DR      (*(volatile uint32_t *)(UART1_BASE + 0x04u))
#define UART1_BRR     (*(volatile uint32_t *)(UART1_BASE + 0x08u))
```

### Boolean Conversions

Non-compliant:
```c
uint8_t flag = 1u;
if (flag) { ... }              /* uint8_t not essentially Boolean */
bool    b    = (bool)sensor;   /* cast without comparison */
```
Compliant:
```c
bool flag = (sensor_value != 0u);
if (flag) { ... }

bool is_active = (state == STATE_ACTIVE);
```

---

## Enumeration Types (Rules 10.1, 10.3, 10.5)

Enumerations are their own essential type category — do not mix with integers.

Non-compliant:
```c
typedef enum { GEAR_P=0, GEAR_R, GEAR_N, GEAR_D } GearPos_t;

GearPos_t gear = GEAR_D;
uint8_t gear_num = gear;       /* enum to integer without cast */
gear = 2;                      /* integer to enum without cast */
```
Compliant:
```c
GearPos_t gear = GEAR_D;
uint8_t   gear_index = (uint8_t)gear;   /* explicit cast */

/* To convert integer input to enum — validate first */
if ((uint8_t)input < (uint8_t)GEAR_COUNT) {
    gear = (GearPos_t)input;
} else {
    report_invalid_gear(input);
}
```

---

## Floating Point in Embedded (Advisory — Use with Caution)

If floating point is used (e.g. with hardware FPU on Cortex-M4/M7):
- Use `float` (32-bit) rather than `double` (64-bit) unless precision required
- Never use floating-point loop counters (Rule 14.1)
- Never test floating-point for equality — use epsilon comparison:

Non-compliant:
```c
if (temperature == 37.5f) { ... }   /* exact float comparison — unreliable */
```
Compliant:
```c
#define FLOAT_EPSILON  (0.001f)
if (fabsf(temperature - 37.5f) < FLOAT_EPSILON) { ... }
```

---

## typedef Conventions for Automotive Embedded C

Recommended naming pattern:
```c
/* Basic types — always use these, never raw int/char */
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef bool     BOOL;

/* Physical unit typedefs — documents the unit in the type name */
typedef uint16_t EngineSpeed_RPM_t;   /* 0–65535 RPM */
typedef int16_t  Temperature_dC_t;    /* degrees Celsius × 10, range –3276.8 to 3276.7 */
typedef uint8_t  Voltage_100mV_t;     /* 0–25.5 V in 100mV steps */
```
