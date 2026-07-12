# Control Flow — MISRA C:2012 for Automotive Embedded C

## Rules Covered
Rule 15.1, 15.2, 15.3, 15.4, 15.5, 15.6, 15.7, 16.1–16.7, 14.1, 17.2

---

## 1. No goto (Rule 15.1 — Mandatory)

`goto` is **absolutely forbidden**. No exceptions, no deviations.

Non-compliant:
```c
if (init_uart() != OK)  { goto error; }
if (init_can()  != OK)  { goto error; }
if (init_adc()  != OK)  { goto error; }
start_application();
return;

error:
    shutdown_all();
    return;
```
Compliant — use structured error handling:
```c
StatusCode_t status = init_uart();
if (status == OK) { status = init_can();  }
if (status == OK) { status = init_adc();  }
if (status == OK) {
    start_application();
} else {
    shutdown_all();
}
```

---

## 2. Always Use Braces (Rule 15.6 — Required)

Every `if`, `else`, `for`, `while`, `do` body MUST have `{ }` even for single statements.

Non-compliant:
```c
if (error_flag)
    handle_error();    /* no braces — next line accidentally added here = hidden bug */

for (i = 0u; i < N; i++)
    process(i);
```
Compliant:
```c
if (error_flag) {
    handle_error();
}

for (i = 0u; i < N; i++) {
    process(i);
}
```

---

## 3. All if-else if Chains End With else (Rule 15.7 — Required)

Non-compliant:
```c
if (state == IDLE) {
    handle_idle();
} else if (state == RUNNING) {
    handle_running();
} else if (state == ERROR) {
    handle_error();
}
/* What if state is SHUTDOWN? Silent — no handler */
```
Compliant:
```c
if (state == IDLE) {
    handle_idle();
} else if (state == RUNNING) {
    handle_running();
} else if (state == ERROR) {
    handle_error();
} else {
    /* defensive: unexpected state — log and enter safe state */
    log_unexpected_state((uint32_t)state);
    enter_safe_state();
}
```

---

## 4. Single Exit From Function (Rule 15.5 — Required)

Functions should have **one** `return` statement, at the end.

Non-compliant:
```c
uint8_t validate_speed(uint16_t speed_rpm) {
    if (speed_rpm == 0u)           { return STATUS_STOPPED; }
    if (speed_rpm > MAX_SPEED_RPM) { return STATUS_OVERSPEED; }
    if (!sensor_valid)             { return STATUS_SENSOR_FAULT; }
    return STATUS_OK;
}
```
Compliant:
```c
uint8_t validate_speed(uint16_t speed_rpm) {
    uint8_t status;
    if (speed_rpm == 0u) {
        status = STATUS_STOPPED;
    } else if (speed_rpm > MAX_SPEED_RPM) {
        status = STATUS_OVERSPEED;
    } else if (!sensor_valid) {
        status = STATUS_SENSOR_FAULT;
    } else {
        status = STATUS_OK;
    }
    return status;
}
```

---

## 5. switch Statement Rules (Rules 16.1–16.7)

### Every case ends with break or return (Rule 16.3)
### Every switch has a default (Rule 16.4)
### default is first or last (Rule 16.5)

Non-compliant:
```c
switch (gear) {
    case GEAR_P:
        engage_park();         /* falls through — Rule 16.3 violation */
    case GEAR_R:
        enable_reverse();
        break;
    case GEAR_D:
        enable_drive();
        break;
    /* no default — Rule 16.4 violation */
}
```
Compliant:
```c
switch (gear) {
    case GEAR_P:
        engage_park();
        break;
    case GEAR_R:
        enable_reverse();
        break;
    case GEAR_N:
        disengage_drive();
        break;
    case GEAR_D:
        enable_drive();
        break;
    default:
        /* Defensive — unexpected gear position */
        report_fault(FAULT_INVALID_GEAR);
        enter_neutral_safe_state();
        break;
}
```

### Intentional Fall-Through Must Be Commented (Note: still violates 16.3)
If a deviation is raised for intentional fall-through:
```c
switch (event) {
    case EVT_KEY_PRESS:
        /* INTENTIONAL FALL-THROUGH: key press and key hold have same handling */
        /* MISRA C:2012 Rule 16.3 — Deviation raised: deviation_id=DEV-016-001 */
    case EVT_KEY_HOLD:
        process_key_input();
        break;
    default:
        /* no action */
        break;
}
```

---

## 6. Bounded Loops (Rules 14.2, 15.4)

### for loops: counter must be the loop variable, not modified in body
Non-compliant:
```c
for (i = 0u; i < buf_len; i++) {
    if (buf[i] == DELIMITER) {
        i = buf_len;    /* modifying loop counter in body — Rule 14.2 */
    }
    process(buf[i]);
}
```
Compliant:
```c
bool found = false;
for (i = 0u; (i < buf_len) && (!found); i++) {
    if (buf[i] == DELIMITER) {
        found = true;
    } else {
        process(buf[i]);
    }
}
```

### while loops: always include a timeout
```c
#define SENSOR_READY_TIMEOUT   (5000u)   /* cycles */

uint32_t timeout = SENSOR_READY_TIMEOUT;
while (!sensor_is_ready()) {
    timeout--;
    if (timeout == 0u) {
        set_fault(FAULT_SENSOR_TIMEOUT);
        break;
    }
}
```

### No Floating-Point Loop Counters (Rule 14.1 — Required)
Non-compliant:
```c
float step;
for (step = 0.0f; step < 10.0f; step += 0.5f) { ... }
```
Compliant:
```c
uint8_t step;
for (step = 0u; step < 20u; step++) {
    float value = (float)step * 0.5f;
    /* use value */
}
```

---

## 7. Controlling Expressions (Rules 14.3, 14.4)

### Must Be Essentially Boolean (Rule 14.4)
Non-compliant:
```c
uint8_t flag = get_status();
if (flag)      { ... }     /* uint8_t, not bool */
while (count)  { ... }     /* uint8_t, not bool */
```
Compliant:
```c
bool flag = (get_status() != 0u);
if (flag)      { ... }

bool has_items = (count > 0u);
while (has_items) {
    process_item();
    count--;
    has_items = (count > 0u);
}
```

### Shall Not Be Invariant (Rule 14.3 — Mandatory)
Non-compliant:
```c
#define MAX_ITEMS  256u
uint8_t i;
if (i < MAX_ITEMS) { }    /* uint8_t max is 255 — always true for uint8_t */
```
Compliant:
```c
uint16_t i;
if (i < MAX_ITEMS) { }    /* uint16_t can be ≥ 256 — condition is real */
```

---

## 8. Boolean Expressions

### No Assignment in Condition (Implied by Rule 13.4 — Advisory)
Non-compliant:
```c
if ((status = get_status()) != OK) { }    /* assignment in condition */
```
Compliant:
```c
status = get_status();
if (status != OK) { }
```

### Logical vs Bitwise Operators
Non-compliant:
```c
if (flag_a & flag_b) { }    /* bitwise & used as logical AND — Rule 10.1 */
```
Compliant:
```c
if (flag_a && flag_b) { }   /* logical AND for Boolean conditions */

uint32_t masked = reg_val & FLAG_MASK;  /* bitwise & for bit manipulation */
```
