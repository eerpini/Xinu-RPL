/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <xinu.h>


extern	devcall	ioerr(void);
extern	devcall	ionull(void);

/* Device independent I/O switch */

struct	dentry	devtab[NDEVS] =
{
/**
 * Format of entries is:
 * dev-number, minor-number, dev-name,
 * init, open, close,
 * read, write, seek,
 * getc, putc, control,
 * dev-csr-address, intr-handler, irq
 */

/* CONSOLE is tty */
	{ 0, 0, "CONSOLE",
	  (void *)ttyInit, (void *)ionull, (void *)ionull,
	  (void *)ttyRead, (void *)ttyWrite, (void *)ioerr,
	  (void *)ttyGetc, (void *)ttyPutc, (void *)ttyControl,
	  (void *)0x80005000, (void *)ttyDispatch, 0 },

/* TTY1 is tty */
	{ 1, 1, "TTY1",
	  (void *)ttyInit, (void *)ionull, (void *)ionull,
	  (void *)ttyRead, (void *)ttyWrite, (void *)ioerr,
	  (void *)ttyGetc, (void *)ttyPutc, (void *)ttyControl,
	  (void *)0x8000b000, (void *)ttyDispatch, 0 },

/* NULLDEV is null */
	{ 2, 0, "NULLDEV",
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* RADIO0 is radio */
	{ 3, 0, "RADIO0",
	  (void *)radioInit, (void *)ioerr, (void *)ioerr,
	  (void *)radioRead, (void *)radioWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 }
};
