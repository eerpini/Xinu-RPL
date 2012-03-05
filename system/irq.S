/* intdispatch.S - intdispatch, savestate, restorestate */

/* adapted from ARM "beagleboard" port */

//#include <arm.h>
//#include <interrupt.h>
	
// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
        .set  MODE_USR, 0x10            // User Mode
        .set  MODE_FIQ, 0x11            // FIQ Mode
        .set  MODE_IRQ, 0x12            // IRQ Mode
        .set  MODE_SVC, 0x13            // Supervisor Mode
        .set  MODE_ABT, 0x17            // Abort Mode
        .set  MODE_UND, 0x1B            // Undefined Mode
        .set  MODE_SYS, 0x1F            // System Mode

        .equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
        .equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled


.text
	.align 4
	.globl irq

/*
 * Entry point for interrupts. Saves the interrupt context and calls the
 * high-level interrupt dispatcher.
 */

/*
 * For ARM port, return address is [lr] - 4
 *
 * irq saves regsiters on IRQ stack and then switches to SYS mode
 * SYS mode uses process stack for interrupt handler isr
 * after interrupt handler completes, irq returns to IRQ mode,
 * restores registers and then returns to interrupted instruction
 */
	
irq:
	/* adjust return address to account for prefetch */
	sub lr, lr, #4

	/* save non-callee-save registers on stack */
	push {r0-r3, r12, lr}
	
	/* switch to supervisor mode */
	mrs r0, spsr
	msr cpsr, #(I_BIT | F_BIT | MODE_SYS)

	push {r0, lr}	/* push additional supervisor mode registers */
	bl isr		/* call high-level dispatcher */
	pop {r0, lr}	/* restore supervisor mode registers */

	/* return to IRQ mode */
	msr cpsr, #(I_BIT | F_BIT | MODE_IRQ)
	msr spsr, r0
	
	/* restore saved registers */
	pop {r0-r3, r12, lr}

	movs pc, lr

