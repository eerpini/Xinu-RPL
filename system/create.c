/* create.c - create, newpid */

#include <xinu.h>
//#include <arm.h>

static	pid32	newpid(void);

/*------------------------------------------------------------------------
 *  create, newpid  -  Create a process to start running a procedure
 *------------------------------------------------------------------------
 */
pid32	create(
	  void		*funcaddr,	/* address of function to run	*/
	  uint32	ssize,		/* stack size in bytes		*/
	  pri16		priority,	/* process priority		*/
	  char		*name,		/* process name (for debugging)	*/
	  uint32	nargs,		/* number of args that follow	*/
	  ...
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	procent *prptr;		/* ptr to process' table entry	*/
	uint32	*savargs;		/* pointer to arg save area	*/
	uint32	*saddr;			/* stack address		*/
	pid32	pid;			/* ID of newly created process	*/
	uint32	*ap;			/* points to list of var args	*/
	int32	pad;			/* padding needed for arg area	*/
	uint32	i;

	void	userstart(void);
	void	userret(void);

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;

	/* Current create limited to 4 args by ARM procedure calling
	 * seqence
	 */
	if (nargs > 4)
		nargs = 4;
	
	if (   (ssize < MINSTK)
	    || (priority <= 0)
	    || (((int32)(pid = newpid())) == (int32) SYSERR)
	    || ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR)) {
		restore(mask);
		return SYSERR;
	}

	prcount++;
	prptr = &proctab[pid];

	/* initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* initial state is suspended	*/
	prptr->prprio = priority;
	prptr->prstkptr = (char *)saddr;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* set up initial device descriptors for the shell		*/
	prptr->prdesc[0] = CONSOLE;	/* stdin  is CONSOLE device	*/
	prptr->prdesc[1] = CONSOLE;	/* stdout is CONSOLE device	*/
	prptr->prdesc[2] = CONSOLE;	/* stderr is CONSOLE device	*/

	/* Initialize process stack */

	/* Debugging information at bottom of stack */

	*saddr = STACKMAGIC;
	*--saddr = pid;
	*--saddr = (uint32)prptr->prstklen;
	*--saddr = (uint32)prptr->prstkbase - prptr->prstklen
					+ sizeof(int);

	/* Bottom half of stack contains process arguments and start address.
	 * These are processed by userstart() which later branches to the start
	 * address.
	 */

	ap = (uint32 *)(&nargs) + 1;	/* pointer to first argument */
	pad = 4 - nargs;

	savargs = saddr - 1;		/* location of first argument on stack */

	/* Push user-supplied arguments */
	for ( ; nargs > 0 ; nargs--)
		*--saddr = *ap++;

	/* userstart() requires at least 4 arguments on the stack
	 * (to fill registers as per ARM calling convention), so
	 * push dummy arguments if required */
	for ( ; pad > 0 ; pad--)
		*--saddr = 0;

	/* Push location of argument list */
	*--saddr = (uint32)savargs;

	/* Push start address of process */
	*--saddr = (uint32)funcaddr;

	/*
	 * Top half of stack contains saved process context in the format that
	 * ctxsw() expects.
	 */
	*--saddr = (uint32)userstart;	/* link register (return address) */
	*--saddr = 0;			/* r12 */
	*--saddr = 0;			/* r11 */
	*--saddr = 0;			/* r10 */
	*--saddr = 0;			/* r9 */
	*--saddr = 0;			/* r8 */
	*--saddr = 0;			/* r7 */
	*--saddr = 0;			/* r6 */
	*--saddr = 0;			/* r5 */
	*--saddr = 0;			/* r4 */
	*--saddr = 0x6000005f;		/* CPSR (in r3) */

	/*
	 * After create, stack is:
	 *
	 * args pointer
	 * new process start address
	 * userstart (to be popped into R14; will pop other stck items)
	 * r12
	 * r11
	 * ...
	 * r5
	 * r4
	 * r3 (CPSR)
	 */

	prptr->prstkptr = (char *)saddr;

	restore(mask);
	return pid;
}

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
local	pid32	newpid(void)
{
	uint32	i;			/* iterate through all processes*/
	static	pid32 nextpid = 1;	/* position in table to try or	*/
					/*  one beyond end of table	*/

	/* check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}
