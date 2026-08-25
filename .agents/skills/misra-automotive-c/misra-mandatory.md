# MISRA C:2012 — Mandatory Rules Reference

Mandatory rules **shall always be complied with**. Deviations are **not permitted**.
These map directly to the highest ASIL levels in ISO 26262.

---

## Rule 1.3 — Undefined Behaviour
**There shall be no occurrence of undefined or critical unspecified behaviour.**

Non-compliant:
```c
int32_t arr[5];
arr[5] = 10;          /* out-of-bounds write — undefined behaviour */
```
Compliant:
```c
int32_t arr[5];
arr[4] = 10;          /* last valid index */
```

---

## Rule 2.1 — Unreachable Code
**A project shall not contain unreachable code.**

Non-compliant:
```c
return result;
result = 0;           /* unreachable — after return */
```
Compliant:
```c
result = compute();
return result;
```

---

## Rule 2.2 — Dead Code
**There shall be no dead code.**
Dead code: code that executes but whose result is never used.

Non-compliant:
```c
x = x + 0;           /* result of +0 is never different — dead */
flag = flag | 0u;
```
Compliant: remove dead assignments entirely.

---

## Rule 2.3 — Unused Type Declarations
**A project should not contain unused type declarations.**

Non-compliant:
```c
typedef uint8_t MyByte;   /* declared but never used in translation unit */
```
Compliant: remove unused typedefs or move them to a shared header where they are used.

---

## Rule 2.6 — Unused Labels
**A function should not contain unused labels.**

Non-compliant:
```c
void foo(void) {
    label1:               /* never jumped to */
    x = 1;
}
```
Compliant: remove unused labels.

---

## Rule 2.7 — Unused Parameters
**There should be no unused parameters in functions.**

Non-compliant:
```c
void process(uint8_t data, uint8_t unused_param) {
    do_something(data);
}
```
Compliant:
```c
void process(uint8_t data) {
    do_something(data);
}
```
If a parameter is intentionally unused (e.g., callback signature), cast to `(void)`:
```c
void callback(uint8_t event, uint8_t reserved) {
    (void)reserved;
    handle_event(event);
}
```

---

## Rule 13.2 — Value of Expression Used in Multiple Places
**The value of an expression and its persistent side effects shall be the same under all permitted evaluation orders.**

Non-compliant:
```c
x = a[i] + i++;      /* i modified and used — order unspecified */
```
Compliant:
```c
x = a[i] + i;
i++;
```

---

## Rule 13.6 — sizeof Operands
**The operand of the sizeof operator shall not contain any expression which has potential side effects.**

Non-compliant:
```c
n = sizeof(x++);
```
Compliant:
```c
n = sizeof(x);
x++;
```

---

## Rule 14.3 — Controlling Expressions Not Invariant
**Controlling expressions shall not be invariant.**

Non-compliant:
```c
if (1)          { ... }    /* always true */
while (0)       { ... }    /* always false */
```
Compliant: use actual runtime conditions.

---

## Rule 14.4 — Controlling Expression Shall Be Essentially Boolean
**The controlling expression of an if statement and the controlling expression of an iteration-statement shall have essentially Boolean type.**

Non-compliant:
```c
uint8_t flag = get_flag();
if (flag) { ... }           /* flag is not essentially Boolean */
```
Compliant:
```c
bool flag = (get_flag() != 0u);
if (flag) { ... }
```

---

## Rule 15.1 — No goto
**The goto statement shall not be used.**

Non-compliant:
```c
goto error_handler;
```
Compliant: use structured error handling with return codes.
```c
if (status != STATUS_OK) {
    return handle_error(status);
}
```

---

## Rule 15.2 — goto Forward Jump Only
*(Even if goto were used)* The goto statement shall jump to a label declared later in the same function. (Defensive — since Rule 15.1 forbids goto entirely.)

---

## Rule 15.3 — goto Jump Scope
Any label referenced by a goto statement shall be declared in the same block or an enclosing block. Moot since Rule 15.1 forbids goto.

---

## Rule 17.1 — No stdarg.h
**The features of <stdarg.h> shall not be used.**

Non-compliant:
```c
#include <stdarg.h>
void log(const char *fmt, ...) { ... }
```
Compliant: use fixed-parameter functions. In automotive/embedded, variadic functions are disallowed.

---

## Rule 17.3 — No Implicit Function Declarations
**A function shall not be declared implicitly.**

Non-compliant:
```c
result = compute_value();   /* compute_value not declared before this point */
```
Compliant:
```c
extern int32_t compute_value(void);   /* explicit declaration in header */
result = compute_value();
```

---

## Rule 17.4 — All Exit Paths Return a Value
**All exit paths from a function with non-void return type shall have an explicit return statement with an expression.**

Non-compliant:
```c
int32_t get_speed(void) {
    if (sensor_valid) {
        return (int32_t)sensor_value;
    }
    /* missing return on else path */
}
```
Compliant:
```c
int32_t get_speed(void) {
    int32_t result;
    if (sensor_valid) {
        result = (int32_t)sensor_value;
    } else {
        result = SPEED_INVALID;
    }
    return result;
}
```

---

## Rule 21.3 — No Dynamic Memory Allocation
**The memory allocation and deallocation functions of <stdlib.h> shall not be used.**
`malloc`, `calloc`, `realloc`, `free` are forbidden.

Non-compliant:
```c
uint8_t *buf = (uint8_t *)malloc(BUFFER_SIZE);
```
Compliant:
```c
static uint8_t buf[BUFFER_SIZE];    /* static allocation */
/* OR */
uint8_t buf[BUFFER_SIZE];           /* stack allocation — check stack depth */
```

---

## Rule 21.6 — No stdio for I/O in Production Code
**The Standard Library input/output functions shall not be used in production code.**
`printf`, `scanf`, `fprintf` etc. are forbidden in safety-critical embedded builds.

Non-compliant:
```c
printf("Speed: %d\n", speed);
```
Compliant: use a dedicated embedded logging/tracing layer, or remove for production builds with `#ifdef DEBUG`.

---

## Rule 21.7 — No atof / atoi / atol / atoll
**The atof, atoi, atol and atoll functions of <stdlib.h> shall not be used.**
These provide no error detection.

Non-compliant:
```c
speed = atoi(input_str);
```
Compliant:
```c
/* Use strtol with error checking */
char *end;
int32_t speed = (int32_t)strtol(input_str, &end, 10);
if (end == input_str) { /* handle parse error */ }
```

---

## Rule 21.8 — No system / abort / exit
**The functions abort, exit, and system of <stdlib.h> shall not be used.**

Non-compliant:
```c
if (error) { exit(1); }
```
Compliant: use safe shutdown routines that allow the system to reach a defined safe state.
```c
if (error) { enter_safe_state(ERROR_CODE_FATAL); }
```
