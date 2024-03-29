/* initialize.c - nulluser, sysinit */

/* Handle system initialization and become the null process */

#include <xinu.h>
#include <string.h>

extern	void	start(void);	/* start of Xinu code */
extern	void	*_end;		/* end of Xinu code */
static	void	sysinit(void);

/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/

/* Memory bounds set by start.S */

void	*minheap;		/* start of heap			*/
void	*maxheap;		/* highest valid memory address		*/

/*------------------------------------------------------------------------
 * nulluser - initialize the system and become the null process
 *
 * Note: execution begins here after the C run-time environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  The code turns itself into the null process
 * after initialization.  Because it must always remain ready to execute,
 * the null process cannot execute code that might cause it to be
 * suspended, wait for a semaphore, put to sleep, or exit.  In
 * particular, the code must not perform I/O except for polled versions
 * such as kprintf.
 *------------------------------------------------------------------------
 */

void	nulluser(void)
{

	sysinit();

	kprintf("\n\r%s\n\n\r", VERSION);

	/* Output Xinu memory layout */

	kprintf("%10d bytes physical memory.\n",
		(uint32)maxheap - (uint32)0x00400000);
	kprintf("           [0x%08X to 0x%08X]\r\n",
		(uint32)0x00400000, (uint32)maxheap - 1);
	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)0x00400000);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)0x00400000, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&end - (uint32)&etext);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&etext, (uint32)&end - 1);
	kprintf("%10d bytes of available heap.\n",
		maxheap - minheap);
	kprintf("           [0x%08X to 0x%08X]\n",
		minheap, maxheap);

	clktime = 0;

	kprintf("           rtc calibrated to %d HZ\n", rtc_freq);
	
	enable_irq(CRM);

	/* Enable interrupts */

	//	global_irq_enable();

	/* Start the network */
	
/*DEBUG*/ // netstart();

	net_init();

	/*
	 * resume(create((void *)rad_in, INITSTK, INITPRIO, "rad_in process", 0,
	 *	      NULL));
	 */

	/* Create a process to execute function main() */
	resume(create((void *)main, INITSTK, INITPRIO, "Main process", 0,
		      NULL));
	
	/* Become the Null process (i.e., guarantee that the CPU has	*/
	/*  something to run when no other process is ready to execute)	*/

	/* There is a bug in CPSR management; shouldn't need to         */
	/* call enable() here                                           */
	enable();



	while (TRUE) {
		;		/* do nothing */
	}
}

/* Portions of this code are adapted from libmc1322x/src/default_lowlevel.c,
 * written by Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org)
 */
void default_vreg_init(void) {
	volatile uint32_t i;
	*CRM_SYS_CNTL = 0x00000018;  /* set default state */
	*CRM_VREG_CNTL = 0x00000f04; /* bypass the buck */
	for(i=0; i<0x161a8; i++)     /* wait for the bypass to take */
		{ continue; }
	*CRM_VREG_CNTL = 0x00000ff8; /* start the regulators */
}

/*------------------------------------------------------------------------
 *
 * sysinit - intialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */
extern uint32 __sys_stack_top__;
static	void	sysinit(void)
{
	int32	i;
	struct	procent	*prptr;		/* ptr to process table entry	*/
	struct	dentry	*devptr;	/* ptr to device table entry	*/
	struct	sentry	*semptr;	/* prr to semaphore table entry	*/
	struct	memblk	*memptr;	/* ptr to memory block		*/

	//	default_vreg_init();

	/* Initialize the interrupt vectors */

	//	initevec();

	/* Initialize system variables */

	/* Count the Null process as the first process in the system */

	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize the free memory list */

	/* Note: PC version has to pre-allocate 640K-1024K "hole" */

	maxheap = (void *)MAXADDR;
	minheap = &end;

	memptr = memlist.mnext = (struct memblk *)roundmb(minheap);
	/* initialize free memory list to one block */
	memlist.mnext = memptr = (struct memblk *) roundmb(&end);
	memptr->mnext = (struct memblk *) NULL;
	memptr->mlength = (uint32) truncmb((uint32)maxheap -
					   (uint32)&end - NULLSTK);

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entry */

	prptr = &proctab[NULLPROC];
	prptr->prstate = PR_CURR;
	prptr->prprio = 0;
	strncpy(prptr->prname, "prnull", 7);
	/* set up in start.S */
	prptr->prstkbase = (char *)&__sys_stack_top__;
	prptr->prstklen = 0x400;
	
	prptr->prstkptr = 0;
	currpid = NULLPROC;

	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	/* Initialize buffer pools */

	bufinit();

	/* Create a ready list for processes */

	readylist = newqueue();

	for (i = 0; i < NDEVS; i++) {
		if (! isbaddev(i)) {
			devptr = (struct dentry *) &devtab[i];
			(devptr->dvinit) (devptr);
		}
	}

	set_channel(15);

	clkinit();

	return;
}

int32	stop(char *s)
{
	kprintf("%s\n", s);
	kprintf("looping... press reset\n");
	while(1){}
	return 0; 	/* for compiler */
}

