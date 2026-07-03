# Memory and Embedded-Specific Rules — MISRA C:2012

## Core Principle
> In safety-critical automotive embedded code:
> **No heap. No recursion. No unbounded loops. volatile everything the hardware touches.**

---

## 1. No Dynamic Memory Allocation (Rule 21.3 — Mandatory)

`malloc`, `calloc`, `realloc`, `free` are **absolutely forbidden** in all safety-critical code paths.

### Why
- Heap fragmentation can cause non-deterministic allocation failure at runtime
- Timing of allocation is non-deterministic — violates ASIL D real-time requirements
- Memory leaks can accumulate over vehicle operating lifetime (ignition cycles)
- ISO 26262 Part 6 requires deterministic memory use for ASIL B and above

### Compliant Patterns

**Static pool allocation:**
```c
/* Pre-allocate a fixed pool at compile time */
#define CAN_MSG_POOL_SIZE   (16u)

typedef struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  dlc;
    bool     in_use;
} CanMsg_t;

static CanMsg_t can_msg_pool[CAN_MSG_POOL_SIZE];

CanMsg_t *CanMsg_Alloc(void) {
    CanMsg_t *msg = NULL;
    uint8_t   i;
    for (i = 0u; i < CAN_MSG_POOL_SIZE; i++) {
        if (!can_msg_pool[i].in_use) {
            can_msg_pool[i].in_use = true;
            msg = &can_msg_pool[i];
            break;
        }
    }
    return msg;   /* returns NULL if pool exhausted — caller must check */
}

void CanMsg_Free(CanMsg_t *msg) {
    if (msg != NULL) {
        msg->in_use = false;
    }
}
```

**Stack allocation (small, bounded objects):**
```c
void process_frame(void) {
    uint8_t frame_buf[64];    /* fixed-size, stack allocated */
    /* ... */
}
/* Frame buf is released automatically on return */
```

---

## 2. volatile — Hardware Registers and ISR-Shared Variables

### Rule: Any variable that can change outside the compiler's view MUST be `volatile`

Without `volatile`, the compiler is free to:
- Cache the value in a register and never re-read from memory
- Optimise away a write it thinks is "redundant"
- Reorder reads/writes around other operations

### Hardware Registers

```c
/* Memory-mapped register — MUST be volatile */
/* MISRA Deviation: Rule 11.4 — hardware address cast — see deviation record */
#define TIMER2_CNT   (*(volatile uint32_t *)0x40000024u)
#define GPIOB_IDR    (*(volatile uint32_t *)0x40020410u)
#define SPI1_DR      (*(volatile uint32_t *)0x4001300Cu)

/* Usage */
uint32_t count = TIMER2_CNT;        /* always reads from peripheral */
GPIOB_IDR & 0x01u;                  /* always reads pin state */
```

### ISR-Shared Variables (ISR writes, main loop reads)

```c
/* In global header or module header */
extern volatile uint32_t g_system_tick_ms;     /* incremented in SysTick_Handler */
extern volatile bool     g_can_rx_pending;      /* set in CAN RX ISR */
extern volatile uint8_t  g_adc_buffer[16];      /* filled in ADC DMA complete ISR */

/* In ISR */
void SysTick_Handler(void) {
    g_system_tick_ms++;                  /* volatile — safe to write from ISR */
}

/* In main loop */
uint32_t now = g_system_tick_ms;        /* volatile read — always fresh */
if (g_can_rx_pending) {
    process_can_message();
    g_can_rx_pending = false;
}
```

### Common Mistake — Missing volatile on Flag

Non-compliant (leads to infinite loop after compiler optimisation):
```c
bool done = false;              /* not volatile */

void IRQHandler(void) {
    done = true;                /* ISR sets this */
}

/* In main: */
while (!done) { }               /* compiler may hoist done into register
                                   and loop forever even after ISR fires */
```
Compliant:
```c
volatile bool done = false;
```

---

## 3. No Recursion (Rule 17.2 — Required)

Recursion is forbidden because:
- Stack depth is unbounded without static analysis
- Stack overflow in embedded = undefined behaviour / hard fault
- Not compatible with WCET (Worst Case Execution Time) analysis required by ISO 26262

Non-compliant:
```c
uint32_t crc_recursive(const uint8_t *data, uint32_t len, uint32_t crc) {
    if (len == 0u) { return crc; }
    return crc_recursive(data + 1u, len - 1u, crc ^ *data);   /* recursion */
}
```
Compliant:
```c
uint32_t crc_iterative(const uint8_t *data, uint32_t len) {
    uint32_t crc = 0xFFFFFFFFu;
    uint32_t i;
    for (i = 0u; i < len; i++) {
        crc ^= (uint32_t)data[i];
        /* ... CRC polynomial steps ... */
    }
    return crc ^ 0xFFFFFFFFu;
}
```

---

## 4. Bounded Loops

All loops must have a provable maximum iteration count.

Non-compliant (unbounded — what if condition never met?):
```c
while (UART_SR_TXE == 0u) { }   /* spin forever if TX never empties */
```
Compliant (with timeout):
```c
#define UART_TX_TIMEOUT_CYCLES   (10000u)

uint32_t timeout = UART_TX_TIMEOUT_CYCLES;
while ((UART1_SR & UART_SR_TXE_MASK) == 0u) {
    timeout--;
    if (timeout == 0u) {
        set_error_flag(ERR_UART_TX_TIMEOUT);
        break;
    }
}
```

---

## 5. ISR Guidelines (Interrupt Service Routines)

ISRs are the tightest constraint in automotive embedded C. Apply ALL of these:

### Do in ISRs:
```c
void CAN1_RX0_IRQHandler(void) {
    /* 1. Read hardware register immediately — clear interrupt flag */
    uint32_t raw_id  = CAN1_sFIFOMailBox[0].RIR;
    uint32_t raw_dlc = CAN1_sFIFOMailBox[0].RDTR & 0x0Fu;

    /* 2. Copy data to shared volatile buffer */
    uint8_t i;
    for (i = 0u; i < (uint8_t)raw_dlc; i++) {
        g_can_rx_buf[i] = (uint8_t)(CAN1_sFIFOMailBox[0].RDLR >> (i * 8u));
    }
    g_can_rx_id      = raw_id >> 21u;    /* extract CAN ID */
    g_can_rx_dlc     = (uint8_t)raw_dlc;

    /* 3. Set flag for main loop — don't process here */
    g_can_rx_pending = true;

    /* 4. Release FIFO */
    CAN1_RF0R |= CAN_RF0R_RFOM0;
}
```

### Never in ISRs:
```c
/* DON'T DO ANY OF THESE IN AN ISR: */
malloc(size);                    /* dynamic allocation — never */
printf("CAN RX: %d\n", id);      /* blocking I/O */
long_computation();              /* heavy work — defeats real-time */
acquire_mutex();                 /* can block — causes priority inversion */
HAL_Delay(10);                   /* blocking delay in ISR — system freeze */
```

### ISR Variable Sharing — Critical Section Pattern:
```c
/* When main loop reads a multi-byte value updated in ISR,
   disable interrupts to prevent torn reads */

uint32_t safe_read_tick(void) {
    uint32_t tick;
    __disable_irq();          /* enter critical section */
    tick = g_system_tick_ms;  /* atomic read */
    __enable_irq();           /* exit critical section */
    return tick;
}
```

---

## 6. Stack Usage Guidelines

```c
/* Avoid large stack objects — use static or module-level buffers */

/* BAD — 1KB on stack in function called from multiple contexts */
void process_data(void) {
    uint8_t temp_buffer[1024];   /* stack pressure */
    /* ... */
}

/* GOOD — static buffer (single instance, no stack pressure) */
static uint8_t s_process_buffer[1024];   /* static — exists once */
void process_data(void) {
    /* use s_process_buffer */
}

/* NOTE: static buffers are not re-entrant — fine for non-reentrant functions */
/* If reentrancy needed, pass buffer from caller */
void process_data_reentrant(uint8_t *buf, uint32_t buf_size) {
    /* buf provided by caller */
}
```

---

## 7. Pointer Safety

### Null Check Before Dereference

Non-compliant:
```c
void send(CanMsg_t *msg) {
    uint32_t id = msg->id;    /* no null check */
}
```
Compliant:
```c
void send(const CanMsg_t *msg) {
    if (msg == NULL) {
        report_error(ERR_NULL_PTR);
        return;
    }
    uint32_t id = msg->id;
}
```

### Function Pointer Safety

```c
/* Function pointer typedef with clear signature */
typedef void (*IsrCallback_t)(uint32_t event);

/* Validate before call */
static IsrCallback_t s_user_callback = NULL;

void invoke_callback(uint32_t event) {
    if (s_user_callback != NULL) {
        s_user_callback(event);
    }
}
```

### Array Bounds

```c
/* Always bounds-check array index before access */
#define FAULT_TABLE_SIZE  (32u)
static FaultEntry_t s_fault_table[FAULT_TABLE_SIZE];

void log_fault(uint8_t fault_id, uint32_t value) {
    if (fault_id < FAULT_TABLE_SIZE) {
        s_fault_table[fault_id].value     = value;
        s_fault_table[fault_id].timestamp = get_tick_ms();
    } else {
        report_error(ERR_FAULT_ID_OUT_OF_RANGE);
    }
}
```
