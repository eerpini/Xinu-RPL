/* ttyInterrupt.c - ttyInterrupt */

#include <mc1322x.h>
#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyInterrupt - handle an interrupt for a tty (serial) device
 *------------------------------------------------------------------------
 */
void ttyInterrupt(void) {
	struct	dentry	*devptr;	/* address of device control blk*/
	struct	ttycblk	*typtr;		/* pointer to ttytab entry	*/	
	struct	uart_csreg *csrptr;	/* address of UART's CSR	*/
	byte	iir = 0;		/* interrupt identification	*/
	byte	lsr = 0;		/* line status			*/

	/* Get CSR address of the device (assume console for now) */

	devptr = (struct dentry *) &devtab[CONSOLE];
	csrptr = (struct uart_csreg *) devptr->dvcsr;

	/* Obtain a pointer to the tty control block */

	typtr = &ttytab[ devptr->dvminor ];

	disable_irq(UART1);
	/* Decode hardware interrupt request from UART device */

	iir = csrptr->STAT;
	//kprintf("IIR: %X\n", iir);

	if ((iir & 0x80) == 0x80) {
		// TX interrupt
		//kprintf("TX interrupt\n");
		ttyInter_out(typtr, csrptr);
	}

	if ((iir & 0x40) == 0x40) {
		//RX interrupt
		//kprintf("RX interrupt\n");
		sched_cntl(DEFER_START);
		while (csrptr->RXCON != 0) {
			ttyInter_in(typtr, csrptr);
		}
		sched_cntl(DEFER_STOP);
	}

	if ((iir & 0x88) == 0x88) {
		//kprintf("TX FIFO overrun: level: %d\n", csrptr->TXCONbits.LVL);
	}

	enable_irq(UART1);
}
