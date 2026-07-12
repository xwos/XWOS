# Preprocessor Directives — MISRA C:2012

## Rules Covered
Rules 20.1–20.14, and related macro safety patterns

---

## 1. Include Guards on Every Header (Rules 4.10, 20.1)

Every header file `.h` MUST have an include guard.

Non-compliant (missing guard):
```c
/* speed_sensor.h — no include guard */
#include "types.h"
typedef struct { ... } SpeedSensor_t;
```
Compliant:
```c
/* speed_sensor.h */
#ifndef SPEED_SENSOR_H
#define SPEED_SENSOR_H

#include "types.h"

typedef struct {
    uint16_t raw_value;
    bool     is_valid;
} SpeedSensor_t;

extern SpeedSensor_t SpeedSensor_Read(void);

#endif /* SPEED_SENSOR_H */
```

---

## 2. Macro Parenthesisation (Rule 20.7 — Required)

Every parameter in a function-like macro and the full expansion MUST be parenthesised.

Non-compliant:
```c
#define CLAMP(val, lo, hi)   val < lo ? lo : val > hi ? hi : val
/* CLAMP(x+1, 0, 100): x+1 < 0 ? 0 : x+1 > 100 ? 100 : x+1 — precendece ambiguous */
```
Compliant:
```c
#define CLAMP(val, lo, hi)   (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))
```

### Object-like macros — also parenthesise values with expressions:
Non-compliant:
```c
#define MAX_SPEED_RPM   6000 + 500    /* used as: speed < MAX_SPEED_RPM * 2
                                          = speed < 6000 + 500 * 2 = 6000 + 1000 = 7000
                                          NOT 6500 * 2 = 13000 */
```
Compliant:
```c
#define MAX_SPEED_RPM   (6000u + 500u)
/* OR better — just use a constant: */
#define MAX_SPEED_RPM   (6500u)
```

---

## 3. Prefer static inline Over Function-Like Macros (Rule 20.10 Advisory)

Function-like macros bypass type checking. `static inline` is safer.

Non-compliant:
```c
#define ABS(x)   ((x) < 0 ? -(x) : (x))   /* works for int, breaks for uint32_t */
```
Compliant:
```c
static inline int32_t abs_s32(int32_t x) {
    return (x < 0) ? (-x) : x;
}
```

Exception: macros are acceptable when inlining truly cannot be used (e.g. token pasting in code generation, platform portability wrappers declared in OS/HAL headers).

---

## 4. No #undef (Rule 20.5 — Advisory)

Undefining and redefining macros is confusing and error-prone.

Non-compliant:
```c
#define BUFFER_SIZE  64u
/* ... some code ... */
#undef  BUFFER_SIZE
#define BUFFER_SIZE  128u   /* silently changes meaning mid-file */
```
Compliant: use separate named constants or configure at build time:
```c
#ifndef BUFFER_SIZE
#  define BUFFER_SIZE  (64u)    /* default — override via compiler flag */
#endif
```

---

## 5. #include Only at Top of File (Rule 20.1 — Required)

Non-compliant:
```c
void some_function(void) {
    /* ... */
}

#include "debug.h"    /* include inside function scope — Rule 20.1 violation */
```
Compliant: all `#include` at top of file, before any code.

---

## 6. Matching #endif in Same File (Rule 20.14 — Required)

Non-compliant: `#if` in one `.h` file, `#endif` in another `.h` that includes the first.
Compliant: each `#if/#ifdef/#ifndef` and its `#endif` are in the same file.

---

## 7. Conditional Compilation — Debug vs Production

Use conditional compilation to strip debug code from production builds:
```c
/* types.h or compiler flags */
/* Production build: -DNDEBUG or -DPRODUCTION_BUILD */

#ifdef DEBUG_BUILD
#  define LOG(fmt, ...)   debug_printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#  define LOG(fmt, ...)   /* no-op in production */
#endif

/* Usage */
LOG("CAN RX: id=%lu dlc=%u\n", (uint32_t)msg_id, dlc);
/* In production: expands to nothing — no code generated */
```

---

## 8. Macro Naming Conventions

```c
/* Object-like macros: ALL_CAPS_WITH_UNDERSCORES */
#define MAX_CAN_NODES        (127u)
#define ENGINE_IDLE_RPM_MIN  (600u)
#define ENGINE_IDLE_RPM_MAX  (900u)

/* Function-like macros: ALL_CAPS + parenthesised params */
#define BIT_SET(reg, bit)    ((reg) |=  (1u << (bit)))
#define BIT_CLR(reg, bit)    ((reg) &= ~(1u << (bit)))
#define BIT_GET(reg, bit)    (((reg) >> (bit)) & 1u)

/* These are safe — no double-evaluation of (bit) since it appears once */
```

---

## 9. Stringification and Token Pasting (Rules 20.10, 20.11, 20.12)

`#` (stringification) and `##` (token pasting) should not be used in user code.
They are permitted in compiler-internal or code-generation framework headers.

If absolutely required (deviation needed):
```c
/* MISRA Deviation: Rule 20.10 */
/* Rationale: register bit-field accessor generation for HAL */
/* Only used in hardware abstraction layer — not in application code */
#define MAKE_REG_ACCESSOR(name, base, offset)  \
    static inline uint32_t name##_read(void) { \
        return *(volatile uint32_t *)((base) + (offset)); \
    }
```

---

## 10. Safe Register Bit Manipulation Pattern

Preferred pattern for automotive embedded register access:
```c
/* hardware/gpio.h */
#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/* Register base addresses — MISRA Rule 11.4 deviation documented */
#define GPIOA_BASE     (0x40020000u)
#define GPIOA_MODER    (*(volatile uint32_t *)(GPIOA_BASE + 0x00u))
#define GPIOA_ODR      (*(volatile uint32_t *)(GPIOA_BASE + 0x14u))
#define GPIOA_IDR      (*(volatile uint32_t *)(GPIOA_BASE + 0x10u))

/* Bit manipulation — safe, single-expansion macros */
#define GPIO_PIN_0_MASK     (0x00000001u)
#define GPIO_PIN_1_MASK     (0x00000002u)
#define GPIO_PIN_5_MASK     (0x00000020u)

/* Static inline functions preferred over macros for type safety */
static inline void GPIO_SetPin(volatile uint32_t *odr, uint32_t pin_mask) {
    *odr |= pin_mask;
}

static inline void GPIO_ClearPin(volatile uint32_t *odr, uint32_t pin_mask) {
    *odr &= ~pin_mask;
}

static inline bool GPIO_ReadPin(const volatile uint32_t *idr, uint32_t pin_mask) {
    return ((*idr & pin_mask) != 0u);
}

#endif /* GPIO_H */
```
