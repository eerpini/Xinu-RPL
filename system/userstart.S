/* userstart.s - userstart */

/* adapted from ARM "beagleboard" port */

.text
	.align	4
	.globl	userstart

/*------------------------------------------------------------------------
 * userstart -	setup process registers and jump to start address
 *------------------------------------------------------------------------
 */

/*
 * Look in create for more detail.  This code addresses the initial
 * resume of a new process as a special case.  create pushes the
 * process arguments on the stack and the address of the code for
 * the new process.  This code pops the arguments into R0-R3,
 * sets lr (return address) to point to userret() and jumps to the
 * process code
 *
 * NOTE: as of 1/20/2012, the argument passing code is not working
 * correctly.  The new process is resumed correctly, but the arguments
 * don't have the correct values in the new process.
 */
	
userstart:

	
	pop {r4, r5}		/* r4 = start address, r5 = location of argument list */

	/* copy first 4 arguments into registers r0-r3 (ARM calling convention) */
	ldr r0, [r5], #-4
	ldr r1, [r5], #-4
	ldr r2, [r5], #-4
	ldr r3, [r5]

	ldr lr, =userret	/* set return address for process to point to userret() */
	mov pc, r4		/* jump to process start address */

