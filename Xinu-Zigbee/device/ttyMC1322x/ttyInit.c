/* ttyInit.c - ttyInit */

#include <mc1322x.h>
#include <xinu.h>

struct	ttycblk	ttytab[Ntty];

/*------------------------------------------------------------------------
 *  ttyInit - initialize buffers and modes for a tty line
 *------------------------------------------------------------------------
 */
devcall	ttyInit(
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct	ttycblk	*typtr;		/* pointer to ttytab entry	*/
	struct	uart_csreg *uptr;	/* address of UART's CSRs	*/

	typtr = &ttytab[ devptr->dvminor ];

	/* Initialize values in the tty control block */

	typtr->tyihead = typtr->tyitail = 	/* set up input queue	*/
		&typtr->tyibuff[0];		/*    as empty		*/
	typtr->tyisem = semcreate(0);		/* input semaphore	*/
	typtr->tyohead = typtr->tyotail = 	/* set up output queue	*/
		&typtr->tyobuff[0];		/*    as empty		*/
	typtr->tyosem = semcreate(TY_OBUFLEN);	/* output semaphore	*/
	typtr->tyehead = typtr->tyetail = 	/* set up echo queue	*/
		&typtr->tyebuff[0];		/*    as empty		*/
	typtr->tyimode = TY_IMCOOKED;		/* start in cooked mode	*/
	typtr->tyiecho = TRUE;			/* echo console input	*/
	typtr->tyieback = TRUE;			/* honor erasing bksp	*/
	typtr->tyevis = TRUE;			/* visual control chars	*/
	typtr->tyecrlf = TRUE;			/* echo CRLF for NEWLINE*/
	typtr->tyicrlf = TRUE;			/* map CR to NEWLINE	*/
	typtr->tyierase = TRUE;			/* do erasing backspace	*/
	typtr->tyierasec = TY_BACKSP;		/* erase char is ^H	*/
	typtr->tyeof = TRUE;			/* honor eof on input	*/
	typtr->tyeofch = TY_EOFCH;		/* end-of-file character*/
	typtr->tyikill = TRUE;			/* allow line kill	*/
	typtr->tyikillc = TY_KILLCH;		/* set line kill to ^U	*/
	typtr->tyicursor = 0;			/* start of input line	*/
	typtr->tyoflow = TRUE;			/* handle flow control	*/
	typtr->tyoheld = FALSE;			/* output not held	*/
	typtr->tyostop = TY_STOPCH;		/* stop char is ^S	*/
	typtr->tyostart = TY_STRTCH;		/* start char is ^Q	*/
	typtr->tyocrlf = TRUE;			/* send CRLF for NEWLINE*/
	typtr->tyifullc = TY_FULLCH;		/* send ^G when buffer	*/
						/*   is full		*/
	/* Initialize the UART */
	uptr = (struct uart_csreg *)devptr->dvcsr;

	/* UART init code adapter from default_lowlevel.c */
        /* UART must be disabled to set the baudrate */
	uptr->CON = 0;

/* UCON bits */
#define UCON_SAMP     10
#define UCON_SAMP_8X   0
#define UCON_SAMP_16X  1
	
/* Baud rate */
#define MOD 9999
/*  230400 bps, INC=767, MOD=9999, 24Mhz 16x samp */
/*  115200 bps, INC=383, MOD=9999, 24Mhz 8x samp */
#define INC 767
/*  921600 bps, MOD=9999, 24Mhz 16x samp */
//#define INC 3071 
#define SAMP UCON_SAMP_8X
//#define SAMP UCON_SAMP_16X

	/* Set up UART for 115200 bps */
	uptr->BR = ( INC << 16 ) | MOD;

	/* TX and CTS as outputs */
	GPIO->PAD_DIR_SET.GPIO_14 = 1;
	GPIO->PAD_DIR_SET.GPIO_16 = 1;

	/* RX and RTS as inputs */
	GPIO->PAD_DIR_RESET.GPIO_15 = 1;
	GPIO->PAD_DIR_RESET.GPIO_17 = 1;

	/* see Section 11.5.1.2 Alternate Modes		*/
	/* you must enable the peripheral first BEFORE	*/
	/*    setting the function in GPIO_FUNC_SEL	*/
	/* From the datasheet: "The peripheral function	*/
	/*   will control operation of the pad IF	*/
	/*   THE PERIPHERAL IS ENABLED.			*/
	uptr->CON = (1 << 0) | (1 << 1); /* enable receive, transmit */
	
	/* set GPIO15-14 to UART (uptr TX and RX)	*/
	GPIO->FUNC_SEL.GPIO_14 = 1;
	GPIO->FUNC_SEL.GPIO_15 = 1;
       
	/* interrupt when there are this number or more	*/
	/*    bytes free in the TX buffer		*/

	uptr->TXCON = 16;
	uptr->RXCON = 16;

	//	uptr->CON = 0; /* leave enabled for kprinf */

	/* Still need to set interrupt vector? */
	enable_irq(UART1);

	return OK;
}
