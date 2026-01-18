/**
 * @file
 * @brief 架构描述层：ARMv8A异常
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#define ARMV8A_GPREG_SIZE (8UL)
#define ARMV8A_GPREG_FRAME_SIZE (34UL * ARMV8A_GPREG_SIZE)
#define ARMV8A_FPUREG_SIZE (16UL)
#define ARMV8A_FPUREG_FRAME_SIZE ((32UL * ARMV8A_FPUREG_SIZE) + 16UL)

  .macro armv8a_save_gpreg_volatile_frame, current_el, sp_el
    .if \current_el == \sp_el
      sub     sp, sp, #ARMV8A_GPREG_FRAME_SIZE
      stp     x0, x1, [sp, #0x00]
      stp     x2, x3, [sp, #0x10]
      stp     x4, x5, [sp, #0x20]
      stp     x6, x7, [sp, #0x30]
      stp     x8, x9, [sp, #0x40]
      stp     x10, x11, [sp, #0x50]
      stp     x12, x13, [sp, #0x60]
      stp     x14, x15, [sp, #0x70]
      stp     x16, x17, [sp, #0x80]
      mov     x0, sp
      add     x0, x0, #ARMV8A_GPREG_FRAME_SIZE
      stp     lr, x0, [sp, #0xF0]
      mrs     x0, elr_el\()\current_el
      mrs     x1, spsr_el\()\current_el
      stp     x0, x1, [sp, #0x100]
    .else
      stp     x0, x1, [sp, #-0x10]! /* push x0, x1, then use them as tmp reg */
      mrs     x0, sp_el\()\sp_el /* use x0 as sp */
      mov     x1, x0 /* backup sp to save later */
      sub     x0, x0, #ARMV8A_GPREG_FRAME_SIZE
      stp     x2, x3, [x0, #0x10]
      stp     x4, x5, [x0, #0x20]
      stp     x6, x7, [x0, #0x30]
      stp     x8, x9, [x0, #0x40]
      stp     x10, x11, [x0, #0x50]
      stp     x12, x13, [x0, #0x60]
      stp     x14, x15, [x0, #0x70]
      stp     x16, x17, [x0, #0x80]
      stp     lr, x1, [x0, #0xF0] /* x1 is backup sp */
      mrs     x2, elr_el\()\current_el
      mrs     x3, spsr_el\()\current_el
      stp     x2, x3, [x0, #0x100]
      ldp     x2, x3, [sp], #0x10 /* pop origin x0, x1 */
      stp     x2, x3, [x0, #0x00] /* save x0, x1 to Reg Frame */
    .endif
  .endm

  .macro armv8a_save_gpreg_nonvolatile_frame, sp
    stp     x18, x19, [\sp, #0x90]
    stp     x20, x21, [\sp, #0xA0]
    stp     x22, x23, [\sp, #0xB0]
    stp     x24, x25, [\sp, #0xC0]
    stp     x26, x27, [\sp, #0xD0]
    stp     x28, x29, [\sp, #0xE0]
  .endm

  .macro armv8a_save_sp, sp_el, sp
    isb
    msr     sp_el\()\sp_el, \sp
  .endm

  .macro armv8a_restore_sp, sp, sp_el
    mrs     \sp, sp_el\()\sp_el
    isb
  .endm

  .macro armv8a_restore_gpreg_nonvolatile_frame, sp
    ldp     x28, x29, [\sp, #0xE0]
    ldp     x26, x27, [\sp, #0xD0]
    ldp     x24, x25, [\sp, #0xC0]
    ldp     x22, x23, [\sp, #0xB0]
    ldp     x20, x21, [\sp, #0xA0]
    ldp     x18, x19, [\sp, #0x90]
  .endm

  .macro armv8a_restore_gpreg_volatile_frame, current_el, sp_el
    .if \current_el == \sp_el
      ldp     x0, x1, [sp, #0x100]
      msr     spsr_el\()\current_el, x1
      msr     elr_el\()\current_el, x0
      ldp     lr, xzr, [sp, #0xF0]
      ldp     x16, x17, [sp, #0x80]
      ldp     x14, x15, [sp, #0x70]
      ldp     x12, x13, [sp, #0x60]
      ldp     x10, x11, [sp, #0x50]
      ldp     x8, x9, [sp, #0x40]
      ldp     x6, x7, [sp, #0x30]
      ldp     x4, x5, [sp, #0x20]
      ldp     x2, x3, [sp, #0x10]
      ldp     x0, x1, [sp, #0x00]
      add     sp, sp, #ARMV8A_GPREG_FRAME_SIZE
    .else
      ldp     x2, x3, [x0, #0x100]
      msr     spsr_el\()\current_el, x3
      msr     elr_el\()\current_el, x2
      ldp     lr, x1, [x0, #0xF0]
      msr     sp_el\()\sp_el, x1
      ldp     x16, x17, [x0, #0x80]
      ldp     x14, x15, [x0, #0x70]
      ldp     x12, x13, [x0, #0x60]
      ldp     x10, x11, [x0, #0x50]
      ldp     x8, x9, [x0, #0x40]
      ldp     x6, x7, [x0, #0x30]
      ldp     x4, x5, [x0, #0x20]
      ldp     x2, x3, [x0, #0x10]
      ldp     x0, x1, [x0, #0x00]
    .endif
  .endm


  .section .armv8a.exception.vector.el1, "ax"
  .align 11
  .global armv8a_exception_vector_el1
  .type armv8a_exception_vector_el1, "function"
/**
 * @brief EL1 Vector
 */
armv8a_exception_vector_el1:
/* exceptions from current EL, using SP0 */
  .type armv8a_exception_sync_el1_sp0, "function"
armv8a_exception_sync_el1_sp0:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0
  bl      armv8a_trap_sync_el1
  b       armv8a_exception_sync_el1_sp0_return

  .org 0x080
  .type armv8a_exception_irq_el1_sp0, "function"
armv8a_exception_irq_el1_sp0:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x80
  bl      armv8a_irq_el1
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el1_sp0_return

  .org 0x100
  .type armv8a_exception_fiq_el1_sp0, "function"
armv8a_exception_fiq_el1_sp0:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x100
  bl      armv8a_fiq_el1
  b       armv8a_exception_fiq_el1_sp0_return

  .org 0x180
  .type armv8a_exception_serror_el1_sp0, "function"
armv8a_exception_serror_el1_sp0:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x180
  bl      armv8a_trap_serror_el1
  b       armv8a_exception_serror_el1_sp0_return

/* exceptions from current EL, using SPx */
  .org 0x200
  .type armv8a_exception_sync_el1_sp1, "function"
armv8a_exception_sync_el1_sp1:
  armv8a_save_gpreg_volatile_frame 1, 1
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x200
  bl      armv8a_trap_sync_el1
  b       armv8a_exception_sync_el1_sp1_return

  .org 0x280
  .type armv8a_exception_irq_el1_sp1, "function"
armv8a_exception_irq_el1_sp1:
  armv8a_save_gpreg_volatile_frame 1, 1
  armv8a_save_gpreg_nonvolatile_frame sp
  msr     daifclr, #1 /* reenable fiqs */
  mov     x0, sp
  mov     x1, #0x280
  bl      armv8a_irq_el1
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el1_sp1_return

  .org 0x300
  .type armv8a_exception_fiq_el1_sp1, "function"
armv8a_exception_fiq_el1_sp1:
  armv8a_save_gpreg_volatile_frame 1, 1
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x300
  bl      armv8a_fiq_el1
  b       armv8a_exception_fiq_el1_sp1_return

  .org 0x380
  .type armv8a_exception_serror_el1_sp1, "function"
armv8a_exception_serror_el1_sp1:
  armv8a_save_gpreg_volatile_frame 1, 1
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x380
  bl      armv8a_trap_serror_el1
  b       armv8a_exception_serror_el1_sp1_return

/* exceptions from lower EL, running AArch64 */
  .org 0x400
  .type armv8a_exception_sync_el1_sp0_aarch64, "function"
armv8a_exception_sync_el1_sp0_aarch64:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x400
  bl      armv8a_trap_sync_el1
  b       armv8a_exception_sync_el1_sp0_return

  .org 0x480
  .type armv8a_exception_irq_el1_sp0_aarch64, "function"
armv8a_exception_irq_el1_sp0_aarch64:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x480
  bl      armv8a_irq_el1
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el1_sp0_return

  .org 0x500
  .type armv8a_exception_fiq_el1_sp0_aarch64, "function"
armv8a_exception_fiq_el1_sp0_aarch64:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x500
  bl      armv8a_fiq_el1
  b       armv8a_exception_fiq_el1_sp0_return

  .org 0x580
  .type armv8a_exception_serror_el1_sp0_aarch64, "function"
armv8a_exception_serror_el1_sp0_aarch64:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x580
  bl      armv8a_trap_serror_el1
  b       armv8a_exception_serror_el1_sp0_return

/* exceptions from lower EL, running AArch32 */
  .org 0x600
  .type armv8a_exception_sync_el1_sp0_aarch32, "function"
armv8a_exception_sync_el1_sp0_aarch32:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x600
  bl      armv8a_trap_sync_el1
  b       armv8a_exception_sync_el1_sp0_return

  .org 0x680
  .type armv8a_exception_irq_el1_sp0_aarch32, "function"
armv8a_exception_irq_el1_sp0_aarch32:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x680
  bl      armv8a_irq_el1
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el1_sp0_return

  .org 0x700
  .type armv8a_exception_fiq_el1_sp0_aarch32, "function"
armv8a_exception_fiq_el1_sp0_aarch32:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x700
  bl      armv8a_fiq_el1
  b       armv8a_exception_fiq_el1_sp0_return

  .org 0x780
  .type armv8a_exception_serror_el1_sp0_aarch32, "function"
armv8a_exception_serror_el1_sp0_aarch32:
  armv8a_save_gpreg_volatile_frame 1, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x780
  bl      armv8a_trap_serror_el1
  b       armv8a_exception_serror_el1_sp0_return

/* exceptions return from current EL, using SP0 */
  .section .armv8a.exception.text.el1
  .type armv8a_exception_sync_el1_sp0_return, "function"
armv8a_exception_sync_el1_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 1, 0
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_irq_el1_sp0_return, "function"
armv8a_exception_irq_el1_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 1, 0
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_fiq_el1_sp0_return, "function"
armv8a_exception_fiq_el1_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 1, 0
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_serror_el1_sp0_return, "function"
armv8a_exception_serror_el1_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 1, 0
  eret

/* exceptions return from current EL, using SPx */
  .section .armv8a.exception.text.el1
  .type armv8a_exception_sync_el1_sp1_return, "function"
armv8a_exception_sync_el1_sp1_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 1, 1
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_irq_el1_sp1_return, "function"
armv8a_exception_irq_el1_sp1_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 1, 1
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_fiq_el1_sp1_return, "function"
armv8a_exception_fiq_el1_sp1_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 1, 1
  eret

  .section .armv8a.exception.text.el1
  .type armv8a_exception_serror_el1_sp1_return, "function"
armv8a_exception_serror_el1_sp1_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 1, 1
  eret



  .section .armv8a.exception.vector.el2, "ax"
  .align 11
  .global armv8a_exception_vector_el2
  .type armv8a_exception_vector_el2, "function"
/**
 * @brief EL2 Vector
 */
armv8a_exception_vector_el2:
/* exceptions from current EL, using SP0 */
armv8a_exception_sync_el2_sp0:
  armv8a_save_gpreg_volatile_frame 2, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0
  bl      armv8a_trap_sync_el2
  b       armv8a_exception_sync_el2_sp0_return

  .org 0x080
armv8a_exception_irq_el2_sp0:
  armv8a_save_gpreg_volatile_frame 2, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x80
  bl      armv8a_irq_el2
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el2_sp0_return

  .org 0x100
armv8a_exception_fiq_el2_sp0:
  armv8a_save_gpreg_volatile_frame 2, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x100
  bl      armv8a_fiq_el2
  b       armv8a_exception_fiq_el2_sp0_return

  .org 0x180
armv8a_exception_serror_el2_sp0:
  armv8a_save_gpreg_volatile_frame 2, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x180
  bl      armv8a_trap_serror_el2
  b       armv8a_exception_serror_el2_sp0_return

/* exceptions from current EL, using SPx */
  .org 0x200
armv8a_exception_sync_el2_sp2:
  armv8a_save_gpreg_volatile_frame 2, 2
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x200
  bl      armv8a_trap_sync_el2
  b       armv8a_exception_sync_el2_sp2_return

  .org 0x280
armv8a_exception_irq_el2_sp2:
  armv8a_save_gpreg_volatile_frame 2, 2
  armv8a_save_gpreg_nonvolatile_frame sp
  msr     daifclr, #1 /* reenable fiqs */
  mov     x0, sp
  mov     x1, #0x280
  bl      armv8a_irq_el2
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el2_sp2_return

  .org 0x300
armv8a_exception_fiq_el2_sp2:
  armv8a_save_gpreg_volatile_frame 2, 2
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x300
  bl      armv8a_fiq_el2
  b       armv8a_exception_fiq_el2_sp2_return

  .org 0x380
armv8a_exception_serror_el2_sp2:
  armv8a_save_gpreg_volatile_frame 2, 2
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x380
  bl      armv8a_trap_serror_el2
  b       armv8a_exception_serror_el2_sp2_return

/* exceptions from lower EL, running AArch64 */
  .org 0x400
armv8a_exception_sync_el2_sp1_aarch64:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x400
  bl      armv8a_trap_sync_el2
  b       armv8a_exception_sync_el2_sp1_return

  .org 0x480
armv8a_exception_irq_el2_sp1_aarch64:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x480
  bl      armv8a_irq_el2
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el2_sp1_return

  .org 0x500
armv8a_exception_fiq_el2_sp1_aarch64:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x500
  bl      armv8a_fiq_el2
  b       armv8a_exception_fiq_el2_sp1_return

  .org 0x580
armv8a_exception_serror_el2_sp1_aarch64:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x580
  bl      armv8a_trap_serror_el2
  b       armv8a_exception_serror_el2_sp1_return

/* exceptions from lower EL, running AArch32 */
  .org 0x600
armv8a_exception_sync_el2_sp1_aarch32:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x600
  bl      armv8a_trap_sync_el2
  b       armv8a_exception_sync_el2_sp1_return

  .org 0x680
armv8a_exception_irq_el2_sp1_aarch32:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x680
  bl      armv8a_irq_el2
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el2_sp1_return

  .org 0x700
armv8a_exception_fiq_el2_sp1_aarch32:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x700
  bl      armv8a_fiq_el2
  b       armv8a_exception_fiq_el2_sp1_return

  .org 0x780
armv8a_exception_serror_el2_sp1_aarch32:
  armv8a_save_gpreg_volatile_frame 2, 1
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 1, x0
  mov     x1, #0x780
  bl      armv8a_trap_serror_el2
  b       armv8a_exception_serror_el2_sp1_return

/* exceptions return from current EL, using SP0 */
  .section .armv8a.exception.text.el2
  .type armv8a_exception_sync_el2_sp0_return, "function"
armv8a_exception_sync_el2_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 0
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_irq_el2_sp0_return, "function"
armv8a_exception_irq_el2_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 0
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_fiq_el2_sp0_return, "function"
armv8a_exception_fiq_el2_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 0
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_serror_el2_sp0_return, "function"
armv8a_exception_serror_el2_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 0
  eret

/* exceptions return from current EL, using SPx */
  .section .armv8a.exception.text.el2
  .type armv8a_exception_sync_el2_sp2_return, "function"
armv8a_exception_sync_el2_sp2_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 2, 2
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_irq_el2_sp2_return, "function"
armv8a_exception_irq_el2_sp2_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 2, 2
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_fiq_el2_sp2_return, "function"
armv8a_exception_fiq_el2_sp2_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 2, 2
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_serror_el2_sp2_return, "function"
armv8a_exception_serror_el2_sp2_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 2, 2
  eret

/* exceptions return from lower EL, running AArch64 & AArch32 */
  .section .armv8a.exception.text.el2
  .type armv8a_exception_sync_el2_sp1_return, "function"
armv8a_exception_sync_el2_sp1_return:
  armv8a_restore_sp x0, 1
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 1
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_irq_el2_sp1_return, "function"
armv8a_exception_irq_el2_sp1_return:
  armv8a_restore_sp x0, 1
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 1
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_fiq_el2_sp1_return, "function"
armv8a_exception_fiq_el2_sp1_return:
  armv8a_restore_sp x0, 1
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 1
  eret

  .section .armv8a.exception.text.el2
  .type armv8a_exception_serror_el2_sp1_return, "function"
armv8a_exception_serror_el2_sp1_return:
  armv8a_restore_sp x0, 1
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 2, 1
  eret



  .section .armv8a.exception.vector.el3, "ax"
  .align 11
  .global armv8a_exception_vector_el3
  .type armv8a_exception_vector_el3, "function"
/**
 * @brief EL3 Vector
 */
armv8a_exception_vector_el3:
/* exceptions from current EL, using SP0 */
armv8a_exception_sync_el3_sp0:
  armv8a_save_gpreg_volatile_frame 3, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0
  bl      armv8a_trap_sync_el3
  b       armv8a_exception_sync_el3_sp0_return

  .org 0x080
armv8a_exception_irq_el3_sp0:
  armv8a_save_gpreg_volatile_frame 3, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x80
  bl      armv8a_irq_el3
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el3_sp0_return

  .org 0x100
armv8a_exception_fiq_el3_sp0:
  armv8a_save_gpreg_volatile_frame 3, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x100
  bl      armv8a_fiq_el3
  b       armv8a_exception_fiq_el3_sp0_return

  .org 0x180
armv8a_exception_serror_el3_sp0:
  armv8a_save_gpreg_volatile_frame 3, 0
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 0, x0
  mov     x1, #0x180
  bl      armv8a_trap_serror_el3
  b       armv8a_exception_serror_el3_sp0_return

/* exceptions from current EL, using SPx */
  .org 0x200
armv8a_exception_sync_el3_sp3:
  armv8a_save_gpreg_volatile_frame 3, 3
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x200
  bl      armv8a_trap_sync_el3
  b       armv8a_exception_sync_el3_sp3_return

  .org 0x280
armv8a_exception_irq_el3_sp3:
  armv8a_save_gpreg_volatile_frame 3, 3
  armv8a_save_gpreg_nonvolatile_frame sp
  msr     daifclr, #1 /* reenable fiqs */
  mov     x0, sp
  mov     x1, #0x280
  bl      armv8a_irq_el3
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el3_sp3_return

  .org 0x300
armv8a_exception_fiq_el3_sp3:
  armv8a_save_gpreg_volatile_frame 3, 3
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x300
  bl      armv8a_fiq_el3
  b       armv8a_exception_fiq_el3_sp3_return

  .org 0x380
armv8a_exception_serror_el3_sp3:
  armv8a_save_gpreg_volatile_frame 3, 3
  armv8a_save_gpreg_nonvolatile_frame sp
  mov     x0, sp
  mov     x1, #0x380
  bl      armv8a_trap_serror_el3
  b       armv8a_exception_serror_el3_sp3_return

/* exceptions from lower EL, running AArch64 */
  .org 0x400
armv8a_exception_sync_el3_sp2_aarch64:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x400
  bl      armv8a_trap_sync_el3
  b       armv8a_exception_sync_el3_sp2_return

  .org 0x480
armv8a_exception_irq_el3_sp2_aarch64:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x480
  bl      armv8a_irq_el3
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el3_sp2_return

  .org 0x500
armv8a_exception_fiq_el3_sp2_aarch64:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x500
  bl      armv8a_fiq_el3
  b       armv8a_exception_fiq_el3_sp2_return

  .org 0x580
armv8a_exception_serror_el3_sp2_aarch64:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x580
  bl      armv8a_trap_serror_el3
  b       armv8a_exception_serror_el3_sp2_return

/* exceptions from lower EL, running AArch32 */
  .org 0x600
armv8a_exception_sync_el3_sp2_aarch32:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x600
  bl      armv8a_trap_sync_el3
  b       armv8a_exception_sync_el3_sp2_return

  .org 0x680
armv8a_exception_irq_el3_sp2_aarch32:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  msr     daifclr, #1 /* reenable fiqs */
  mov     x1, #0x680
  bl      armv8a_irq_el3
  msr     daifset, #1 /* disable fiqs to restore reg */
  b       armv8a_exception_irq_el3_sp2_return

  .org 0x700
armv8a_exception_fiq_el3_sp2_aarch32:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x700
  bl      armv8a_fiq_el3
  b       armv8a_exception_fiq_el3_sp2_return

  .org 0x780
armv8a_exception_serror_el3_sp2_aarch32:
  armv8a_save_gpreg_volatile_frame 3, 2
  armv8a_save_gpreg_nonvolatile_frame x0
  armv8a_save_sp 2, x0
  mov     x1, #0x780
  bl      armv8a_trap_serror_el3
  b       armv8a_exception_serror_el3_sp2_return

/* exceptions return from current EL, using SP0 */
  .section .armv8a.exception.text.el3
  .type armv8a_exception_sync_el3_sp0_return, "function"
armv8a_exception_sync_el3_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 0
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_irq_el3_sp0_return, "function"
armv8a_exception_irq_el3_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 0
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_fiq_el3_sp0_return, "function"
armv8a_exception_fiq_el3_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 0
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_serror_el3_sp0_return, "function"
armv8a_exception_serror_el3_sp0_return:
  armv8a_restore_sp x0, 0
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 0
  eret

/* exceptions return from current EL, using SPx */
  .section .armv8a.exception.text.el3
  .type armv8a_exception_sync_el3_sp3_return, "function"
armv8a_exception_sync_el3_sp3_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 3, 3
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_irq_el3_sp3_return, "function"
armv8a_exception_irq_el3_sp3_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 3, 3
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_fiq_el3_sp3_return, "function"
armv8a_exception_fiq_el3_sp3_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 3, 3
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_serror_el3_sp3_return, "function"
armv8a_exception_serror_el3_sp3_return:
  armv8a_restore_gpreg_nonvolatile_frame sp
  armv8a_restore_gpreg_volatile_frame 3, 3
  eret

/* exceptions return from lower EL, running AArch64 & AArch32 */
  .section .armv8a.exception.text.el3
  .type armv8a_exception_sync_el3_sp2_return, "function"
armv8a_exception_sync_el3_sp2_return:
  armv8a_restore_sp x0, 2
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 2
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_irq_el3_sp2_return, "function"
armv8a_exception_irq_el3_sp2_return:
  armv8a_restore_sp x0, 2
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 2
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_fiq_el3_sp2_return, "function"
armv8a_exception_fiq_el3_sp2_return:
  armv8a_restore_sp x0, 2
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 2
  eret

  .section .armv8a.exception.text.el3
  .type armv8a_exception_serror_el3_sp2_return, "function"
armv8a_exception_serror_el3_sp2_return:
  armv8a_restore_sp x0, 2
  armv8a_restore_gpreg_nonvolatile_frame x0
  armv8a_restore_gpreg_volatile_frame 3, 2
  eret
