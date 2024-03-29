/* kprintf.c -  kputc, kgetc, kprintf */

#include <xinu.h>
#include <stdarg.h>

/*
 * local_delay works around very strange UART1 behavior.  Without
 * this call, polled output driver hangs in wait loop waiting
 * for UART1 to empty.  Ralph doesn't understand.
 */

void local_delay(void) {	
	volatile uint32_t i;
	
	i = 1;
}


/*------------------------------------------------------------------------
 * kputc - use polled I/O to write a character to the console serial line
 *------------------------------------------------------------------------
 */
syscall kputc(
	  byte	c			/* character to write		*/
	)
{
	struct	dentry		*devptr;
	struct	uart_csreg	*csrptr;

	/* Get CSR address of the console */

	devptr = (struct dentry *) &devtab[CONSOLE];
	csrptr = (struct uart_csreg *) devptr->dvcsr;

	/* wait for UART transmit queue to empty
	 * TXCONbits.LVL returns empty slots in UART FIFO
	 * loop waits until FIFO has room for new characters
	 */

	while (csrptr->TXCONbits.LVL == 0) {
		local_delay();
	}

	/* write the character */

	csrptr->DATA = c;

	/* Honor CRLF - when writing NEWLINE also send CARRIAGE RETURN	*/

	if (c == '\n') {
	        while (csrptr->TXCONbits.LVL == 0) {
			local_delay();
		}
		csrptr->DATA = '\r';
	}

	return OK;
}

/**
 * perform a synchronous kernel read from a serial device
 * @param *devptr pointer to device on which to write character
 * @return character read on success, SYSERR on failure
 */

/*
 * NOTE: this code has not been tested and is probably incorrect.
 * Character ready test is checking "RXRDY caused interrupt" bit,
 * which isn't right
 */

syscall kgetc(void)
{
    struct	dentry	*devptr;
    struct	uart_csreg *csrptr;
    //int irmask;
    byte c;

    devptr = (struct dentry *) &devtab[CONSOLE];
    csrptr = (struct uart_csreg *)devptr->dvcsr;

    /* Disable UART interrupts */
    //irmask = csrptr->CON;
    //csrptr->STAT &= ~(1 << 1);

    //kprintf("kgetc LVL = %d\n", csrptr->RXCONbits.LVL); 

    /* Doesn't this code need to disable interrupts? */
    while (csrptr->RXCONbits.LVL == 0)
    {
	    local_delay();/* Do Nothing */
    }

    /* read character from Receive Holding Register */
    c = csrptr->DATA;
    kprintf("kgetc read %c\n", c);

    /* Restore UART interrupts.     */
    //csrptr->CON = irmask;

    return c;
}

extern	void	_doprnt(char *, va_list, int (*)(int), int);

/**
 * kernel printf: formatted, unbuffered output to CONSOLE
 * @param *fmt pointer to string being printed
 * @return OK on success
 */
syscall kprintf(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _doprnt(fmt, ap, (int (*)(int))kputc, (int)&devtab[CONSOLE]);
    va_end(ap);
    return OK;
}

syscall kprint6addr(char *addr) {
	int i;

	for (i = 0; i < 15; i++)
		kprintf("%02x:", *addr++);
	kprintf("%02x", *addr);
	return OK;
}
