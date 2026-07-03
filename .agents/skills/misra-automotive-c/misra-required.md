# MISRA C:2012 — Required Rules Reference

Required rules **shall be complied with** unless a formal deviation is raised and documented.
Each deviation requires: rule number, rationale, risk assessment, and sign-off.

---

## Category 4 — Character Sets and Lexical Conventions

### Rule 4.1 — Octal and Hex Escape Sequences
**Octal and hexadecimal escape sequences shall be terminated.**

Non-compliant:
```c
const char msg[] = "\x41BC";   /* is it '\x41', 'B', 'C' or '\x41B', 'C'? */
```
Compliant:
```c
const char msg[] = "\x41" "BC";
```

### Rule 4.2 — No Trigraphs
**Trigraphs shall not be used.**

Non-compliant: Any use of `??=`, `??(`, `??)`, etc.
Compliant: Use the actual character directly.

---

## Category 5 — Identifiers

### Rule 5.1 — Unique External Identifiers
**External identifiers shall be distinct in the first 31 characters.**

Non-compliant:
```c
extern uint32_t vehicle_speed_sensor_front_left_raw;
extern uint32_t vehicle_speed_sensor_front_left_raw_filtered;  /* first 31 chars identical */
```
Compliant: use abbreviations or restructure naming.

### Rule 5.3 — No Identifier Hiding
**An identifier declared in an inner scope shall not hide an identifier in an outer scope.**

Non-compliant:
```c
uint8_t speed = 0u;
void process(void) {
    uint8_t speed = 10u;    /* hides outer speed — Rule 5.3 violation */
}
```
Compliant: rename inner variable.

### Rule 5.4 — Macro Identifiers Unique
**Macro identifiers shall be distinct from any other identifier in the translation unit.**

Non-compliant:
```c
#define TIMEOUT 100
uint32_t TIMEOUT;      /* same name as macro */
```

---

## Category 7 — Literals

### Rule 7.1 — No Octal Constants
**Octal constants shall not be used.**

Non-compliant:
```c
uint8_t mode = 010;    /* this is 8 decimal, not 10! */
```
Compliant:
```c
uint8_t mode = 8u;
```

### Rule 7.2 — Suffix on Unsigned Constants
**A "u" or "U" suffix shall be applied to all integer constants that are used in an unsigned context.**

Non-compliant:
```c
uint32_t reg = 0xFF;
```
Compliant:
```c
uint32_t reg = 0xFFu;
```

### Rule 7.4 — No String Literals Assigned to Non-const Pointer
**A string literal shall not be assigned to an object unless the object's type is pointer to const-qualified char.**

Non-compliant:
```c
char *name = "Engine";       /* non-const pointer */
```
Compliant:
```c
const char *name = "Engine";
```

---

## Category 8 — Declarations and Definitions

### Rule 8.1 — Types Shall Be Explicitly Specified
**Types shall be explicitly specified.**

Non-compliant:
```c
static count = 0;        /* implicit int */
```
Compliant:
```c
static int32_t count = 0;
```

### Rule 8.2 — Function Types in Scope
**Function types shall be in scope at the point of call.**

Ensure all functions are declared in a header before use.

### Rule 8.4 — Compatible Declarations
**A compatible declaration shall be visible when an object or function with external linkage is defined.**

Every `.c` file defining an `extern` function must include the header that declares it, so the compiler can verify signature compatibility.

### Rule 8.7 — Functions Used in One Translation Unit Should Be Internal
**Functions and objects should not be defined with external linkage if they are referenced in only one translation unit.**

Non-compliant:
```c
/* in speed_sensor.c — only used here */
uint32_t compute_raw_speed(void) { ... }   /* should be static */
```
Compliant:
```c
static uint32_t compute_raw_speed(void) { ... }
```

### Rule 8.9 — Objects Defined at Block Scope
**An object should be defined at block scope if its identifier only appears in a single function.**

Prefer local variables over file-scope variables where possible.

### Rule 8.13 — const-qualify Pointers Where Possible
**A pointer should point to a const-qualified type whenever possible.**

Non-compliant:
```c
void display(uint8_t *data) { ... }        /* data not modified */
```
Compliant:
```c
void display(const uint8_t *data) { ... }
```

---

## Category 9 — Initialisation

### Rule 9.1 — Objects Set Before Use
**The value of an object with automatic storage duration shall not be read before it has been set.**

Non-compliant:
```c
uint32_t speed;
if (speed > MAX_SPEED) { ... }    /* speed uninitialized */
```
Compliant:
```c
uint32_t speed = 0u;
```

### Rule 9.3 — Arrays Fully Initialised or Not At All
**Arrays shall not be partially initialized.**

Non-compliant:
```c
uint8_t buf[8] = {0, 1, 2};    /* only first 3 initialised */
```
Compliant:
```c
uint8_t buf[8] = {0u, 1u, 2u, 0u, 0u, 0u, 0u, 0u};
/* OR */
uint8_t buf[8] = {0u};    /* zero-initialise all */
```

---

## Category 10 — Essential Type Model

### Rule 10.1 — No Inappropriate Essential Type Operations
**Operands shall not be of an inappropriate essential type.**
E.g., bitwise operations on signed integers are inappropriate.

Non-compliant:
```c
int32_t mask = 0xFF;
int32_t result = value & mask;   /* bitwise AND on signed type */
```
Compliant:
```c
uint32_t mask = 0xFFu;
uint32_t result = value & mask;
```

### Rule 10.3 — Value Assigned to Correct Essential Type
**The value of an expression shall not be assigned to an object with a narrower essential type.**

Non-compliant:
```c
uint8_t  byte_val;
uint32_t word_val = 300u;
byte_val = word_val;    /* narrowing — 300 doesn't fit in uint8_t */
```
Compliant:
```c
byte_val = (uint8_t)(word_val & 0xFFu);   /* explicit cast with mask */
```

### Rule 10.4 — Operands of Same Essential Type
**Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type category.**

Non-compliant:
```c
uint32_t a = 10u;
int32_t  b = 5;
uint32_t c = a + b;    /* unsigned + signed — implicit conversion */
```
Compliant:
```c
uint32_t a = 10u;
uint32_t b = 5u;
uint32_t c = a + b;
```

### Rule 10.5 — Cast to Essential Type Category
**The value of an expression should not be cast to an inappropriate essential type.**

Non-compliant:
```c
bool flag = (bool)speed_value;   /* numeric to Boolean without explicit test */
```
Compliant:
```c
bool flag = (speed_value != 0u);
```

### Rule 10.6 — Composite Expression Assigned to Wider Type
**The value of a composite expression shall not be assigned to an object with wider essential type without explicit cast.**

Non-compliant:
```c
uint32_t result = u16_a + u16_b;   /* addition done in uint16_t, then widened */
```
Compliant:
```c
uint32_t result = (uint32_t)u16_a + (uint32_t)u16_b;
```

### Rule 10.8 — Cast of Composite Expression
**The value of a composite expression shall not be cast to a different essential type category or a wider essential type.**

---

## Category 11 — Pointer Type Conversions

### Rule 11.3 — No Cast Between Pointer to Object Types
**A cast shall not be performed between a pointer to object type and a different pointer to object type.**

Non-compliant:
```c
uint32_t *reg_ptr = (uint32_t *)byte_ptr;    /* casting pointer types */
```
Compliant: use memcpy for type-punning, or use unions with care:
```c
typedef union {
    uint8_t  bytes[4];
    uint32_t word;
} RegUnion_t;
```

### Rule 11.4 — No Cast Between Pointer to Object and Integer
**A conversion should not be performed between a pointer to object and an integer type.**

Exception: hardware register access — must document as a formal deviation.
```c
/* MISRA Deviation: Rule 11.4 — hardware register access */
/* Rationale: MCU peripheral registers require absolute address casting */
/* Risk: controlled — address is fixed, defined in linker/header */
#define GPIOA_ODR  (*(volatile uint32_t *)0x40020014u)
```

### Rule 11.5 — No Void Pointer Conversion
**A conversion should not be performed from pointer to void into pointer to object.**

Non-compliant:
```c
void     *generic_ptr = get_buffer();
uint8_t  *buf = generic_ptr;        /* implicit void* conversion */
```
Compliant:
```c
uint8_t *buf = (uint8_t *)get_buffer();   /* explicit cast */
```

### Rule 11.8 — No Cast Removing const or volatile
**A cast shall not remove any const or volatile qualification from the type pointed to by a pointer.**

Non-compliant:
```c
const uint8_t *src = get_data();
uint8_t *dst = (uint8_t *)src;     /* drops const */
```

---

## Category 12 — Expressions

### Rule 12.1 — Precedence of Operators
**The precedence of operators within expressions should be made explicit.**

Non-compliant:
```c
result = a + b << 2;       /* unclear precedence */
```
Compliant:
```c
result = (a + b) << 2u;    /* explicit parentheses */
```

### Rule 12.2 — Shift Count Within Range
**The right-hand operand of a shift operator shall lie in the range zero to one less than the width in bits of the essential type of the left-hand operand.**

Non-compliant:
```c
uint8_t x = 1u;
uint8_t y = x << 8u;    /* shift by >= bit width — undefined behaviour */
```
Compliant:
```c
uint8_t y = x << 7u;    /* max shift for uint8_t */
```

---

## Category 14 — Control Statement Expressions

### Rule 14.1 — No Unreachable Code in Loops
**A loop counter shall not have essentially floating-point type.**

Non-compliant:
```c
for (float f = 0.0f; f < 10.0f; f += 1.0f) { ... }
```
Compliant:
```c
for (uint8_t i = 0u; i < 10u; i++) { ... }
```

---

## Category 15 — Control Flow

### Rule 15.4 — Single break Per Loop
**There should be no more than one break or goto statement used to terminate any iteration statement.**

### Rule 15.5 — Single Exit From Function
**A function should have a single point of exit at the end.**

Non-compliant:
```c
uint8_t check(uint8_t val) {
    if (val == 0u) { return ERROR; }      /* early return */
    if (val > MAX)  { return OVERFLOW; }  /* early return */
    return OK;
}
```
Compliant:
```c
uint8_t check(uint8_t val) {
    uint8_t result;
    if (val == 0u) {
        result = ERROR;
    } else if (val > MAX) {
        result = OVERFLOW;
    } else {
        result = OK;
    }
    return result;
}
```

### Rule 15.6 — Single Statement in if/else/loop Bodies
**The body of an iteration-statement or a selection-statement shall be a compound statement.**
Always use braces `{}`.

Non-compliant:
```c
if (error) return -1;
```
Compliant:
```c
if (error) { return -1; }
```

### Rule 15.7 — else Clause on if-else if
**All if-else if constructs shall be terminated with an else statement.**

Non-compliant:
```c
if (mode == MODE_A) {
    run_a();
} else if (mode == MODE_B) {
    run_b();
}
/* missing else */
```
Compliant:
```c
if (mode == MODE_A) {
    run_a();
} else if (mode == MODE_B) {
    run_b();
} else {
    /* default action or error handler */
    handle_unknown_mode(mode);
}
```

---

## Category 16 — Switch Statements

### Rule 16.1 — switch Expression
**All switch statements shall be well-formed.**

### Rule 16.3 — Every switch Clause Ends With break or return
**An unconditional break statement shall terminate every switch-clause.**

Non-compliant:
```c
switch (state) {
    case STATE_IDLE:
        init();          /* falls through to STATE_RUN — likely a bug */
    case STATE_RUN:
        run();
        break;
}
```
Compliant:
```c
switch (state) {
    case STATE_IDLE:
        init();
        break;
    case STATE_RUN:
        run();
        break;
    default:
        handle_error();
        break;
}
```

### Rule 16.4 — Every switch Has a default
**Every switch statement shall have a default label.**

### Rule 16.5 — default Clause in Switch
**A default label shall appear as either the first or last switch label of a switch statement.**

---

## Category 17 — Functions

### Rule 17.2 — No Recursion
**Functions shall not call themselves, either directly or indirectly.**

Non-compliant:
```c
uint32_t factorial(uint32_t n) {
    return (n <= 1u) ? 1u : n * factorial(n - 1u);  /* recursion */
}
```
Compliant:
```c
uint32_t factorial(uint32_t n) {
    uint32_t result = 1u;
    uint32_t i;
    for (i = 2u; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

### Rule 17.6 — Array Declarator Parameters
**The declaration of an array parameter shall not contain the static keyword between the [ ].**

### Rule 17.7 — Return Value Shall Be Used
**The value returned by a function having non-void return type shall be used.**

Non-compliant:
```c
memcpy(dst, src, len);         /* return value ignored */
strncpy(buf, str, sizeof(buf)); /* return value ignored */
```
Compliant:
```c
(void)memcpy(dst, src, len);   /* explicit discard with (void) cast */
```
Or better — check the return value if it signals errors.

---

## Category 20 — Preprocessing Directives

### Rule 20.1 — #include Only at Top
**#include directives should only be preceded by other preprocessor directives or comments.**

### Rule 20.4 — No Macro Redefining Reserved Names
**A macro shall not be defined with the same name as a keyword.**

### Rule 20.7 — Macro Parameters Parenthesised
**Expressions resulting from the expansion of macro parameters shall be enclosed in parentheses.**

Non-compliant:
```c
#define DOUBLE(x)  x + x       /* DOUBLE(a*b) = a*b + a*b — OK */
                                /* DOUBLE(a+b) = a+b + a+b = 2a+2b — OK? */
                                /* used in: result = 4 * DOUBLE(a+b) */
                                /* expands to: 4 * a+b + a+b — WRONG */
```
Compliant:
```c
#define DOUBLE(x)  ((x) + (x))
```

### Rule 20.10 — No # and ## in Macros
**The # and ## preprocessor operators should not be used.**
Prefer inline functions.

### Rule 20.11 — Macro Argument After # Unique
**A macro parameter immediately following a # operator shall not immediately be followed by a ## operator.**

### Rule 20.12 — # and ## Operand Not Macro-replaced
**A macro parameter used as an operand to the # or ## operators shall only be used as an operand to these operators.**

### Rule 20.13 — # Ends with Newline
**A line whose first token is # shall be a valid preprocessing directive.**

### Rule 20.14 — Matching #if / #endif in Same File
**All #else, #elif and #endif preprocessor directives shall reside in the same file as the #if, #ifdef or #ifndef directive to which they are related.**
