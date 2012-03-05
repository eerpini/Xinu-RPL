/* intr.s - disable(), restore(), enable(), pause(), halt() */

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

	.section	.text.disable,"ax",%progbits
	.global	disable
	.type	disable,%function
disable:
	mrs	r0, CPSR
	msr	CPSR_c,  #(MODE_SYS | I_BIT | F_BIT)
	bx	lr

	.section	.text.restore,"ax",%progbits
	.global	restore
	.type	restore,%function
restore:
	msr	CPSR_c, r0
	bx	lr

	.section	.text.enable,"ax",%progbits
	.global	enable
	.type	enable,%function
enable:
	msr	CPSR_c,  #(MODE_SYS | F_BIT)
	mrs	r0, CPSR
	bx	lr

	.section	.text.getcpsr,"ax",%progbits
	.global	getcpsr
	.type	getcpsr,%function
getcpsr:
	mrs	r0, CPSR
	bx	lr

	.section	.text.halt,"ax",%progbits
	.global	halt
	.type	halt,%function
halt:
	b	halt
	

	