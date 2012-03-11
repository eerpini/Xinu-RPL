/* ctxsw.s - ctxsw */

/* adapted from ARM "beagleboard" port */
//#include <arm.h>

.text
	.align 4
	.globl	ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one process context to another
 *------------------------------------------------------------------------
 */
ctxsw:
	/* save current process' registers on stack */
	mrs r3, cpsr
	push {r3-r12, lr}	// save R3-R12, R14

	/* store current process' stack pointer */
	str sp, [r0]

	/* load new process' stack pointer */
	ldr sp, [r1]

	/* restore new process' registers from stack */
	pop {r3-r12, lr}
	msr cpsr, r3
	mov pc, lr
