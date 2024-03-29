/* ttyInter_out.c - ttyInter_out */

#include <mc1322x.h>
#include <xinu.h>

/*------------------------------------------------------------------------
 *  ttyInter_out - handle an output on a tty device by sending more
 *		    characters to the device FIFO (interrupts disabled)
 *------------------------------------------------------------------------
 */
void	ttyInter_out(
	 struct	ttycblk	*typtr,		/* ptr to ttytab entry		*/
	 struct	uart_csreg *csrptr	/* address of UART's CSRs	*/
	)
{
	
	int32	ochars;			/* number of output chars sent	*/
					/*   to the UART		*/
	int32	avail;			/* available chars in output buf*/
	int32	uspace;			/* space left in onboard UART	*/
					/*   output FIFO		*/

	/* If output is currently held, simply ignore the call */

	if (typtr->tyoheld) {
		csrptr->CON |= 0x2000; /* mask the interrupt */
		return;
	}

	/* If echo and output queues empty, turn off interrupts */

	if ( (typtr->tyehead == typtr->tyetail) &&
	     (semcount(typtr->tyosem) >= TY_OBUFLEN) ) {
		csrptr->CON |= 0x2000; /* mask the interrupt */
		return;
	}
	
	/* Initialize uspace to the size of the transmit FIFO */

	uspace = UART_FIFO_SIZE;

	/* While onboard FIFO is not full and the echo queue is	*/
	/* nonempty, xmit chars from the echo queue		*/
  
	while ( (uspace>0) &&  typtr->tyehead != typtr->tyetail) {

		csrptr->DATA = *typtr->tyehead++;
		if (typtr->tyehead >= &typtr->tyebuff[TY_EBUFLEN]) {
			typtr->tyehead = typtr->tyebuff;
		}
		uspace--;
	}

	/* While onboard FIFO is not full and the output queue	*/
	/* is nonempty,	xmit chars from the output queue	*/

	ochars = 0;
	avail = TY_OBUFLEN - semcount(typtr->tyosem);
	while ( (uspace>0) &&  (avail > 0) ) {
		csrptr->DATA = *typtr->tyohead++;
		if (typtr->tyohead >= &typtr->tyobuff[TY_OBUFLEN]) {
			typtr->tyohead = typtr->tyobuff;
		}
		avail--;
		uspace--;
		ochars++;
	}
	if (ochars > 0) {
		signaln(typtr->tyosem, ochars);
	}

	return;
}
