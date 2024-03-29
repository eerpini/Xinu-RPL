/* clkinit.c - clkinit */

/* Portions of this code are adapted from libmc1322x/tests/xtal-trim.c,
 * written by Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org)
 */

#include <xinu.h>
#include <stdio.h>

int	rtc_freq;

#define abs( x ) ((x) > 0)?(x):(-(x))
uint32_t rtc_try(int loading, int timeout)
{
	/* Total loading is
	     ctune * 1000 fF + ftune * 160 fF
	   ctune = 0-15
	   ftune = 0-31
	   max = 19960 fF
	*/

#define RTC_LOADING_MIN 0
#define RTC_LOADING_MAX 19960

	/* The fine tune covers a range larger than a single coarse
	   step.  Check all coarse steps within the fine tune range to
	   find the optimal CTUNE, FTUNE pairs. */
#define CTUNE_MAX 15
#define FTUNE_MAX 31
#define CSTEP 1000
#define FSTEP 160
#define MAX_F (FSTEP*FTUNE_MAX)  /* actually lcm(CSTEP,FSTEP) would be better,
				    but in this case it's basically the same */
	int ctune;
	int ftune;
	int ctune_start = (loading - MAX_F) / CSTEP;
	int ctune_end = loading / CSTEP;
	int best_err = loading, best_ctune = 0, best_ftune = 0;

	uint32_t count;

	if (ctune_start < 0) ctune_start = 0;
	if (ctune_end > CTUNE_MAX) ctune_end = CTUNE_MAX;

	for (ctune = ctune_start; ctune <= ctune_end; ctune++)
	{
		int this_loading, this_err;

		ftune = ((loading - (ctune * CSTEP)) + (FSTEP / 2)) / FSTEP;
		if (ftune < 0) ftune = 0;
		if (ftune > FTUNE_MAX) ftune = FTUNE_MAX;

		this_loading = ctune * CSTEP + ftune * FSTEP;
		this_err = abs(this_loading - loading);
		if (this_err < best_err) {
			best_err = this_err;
			best_ctune = ctune;
			best_ftune = ftune;
		}
	}

	//kprintf("requested loading %d, actual loading %d\r\n", loading,
	//	  best_ctune * CSTEP + best_ftune * FSTEP);

	/* Run the calibration */
	CRM->RINGOSC_CNTLbits.ROSC_CTUNE = best_ctune;
	CRM->RINGOSC_CNTLbits.ROSC_FTUNE = best_ftune;
	CRM->CAL_CNTLbits.CAL_TIMEOUT = timeout;
	CRM->STATUSbits.CAL_DONE = 1;
	CRM->CAL_CNTLbits.CAL_EN = 1;
	while (CRM->STATUSbits.CAL_DONE == 0)
		continue;

	/* Result should ideally be close to (REF_OSC * (timeout / 2000)) */
	count = CRM->CAL_COUNT;
	if (count == 0) count = 1;  /* avoid divide by zero problems */
	return count;
}


/* Calibrate the ring oscillator */
void rtc_calibrate(void)
{
	/* Just bisect a few times.  Our best tuning accuracy is about
	   1/500 of the full scale, so doing this 8-9 times is about
	   as accurate as we can get */
	int i;
	int low = RTC_LOADING_MIN, high = RTC_LOADING_MAX;
	int mid;
	uint32_t count;

#define RTC_TIMEOUT 100  /* 50 msec per attempt */

	for (i = 0; i < 9; i++)
	{
		mid = (low + high) / 2;
		count = rtc_try(mid, RTC_TIMEOUT);
		// careful about overflow
		rtc_freq = REF_OSC / ((count + RTC_TIMEOUT/2) / RTC_TIMEOUT);

		if (rtc_freq > 2000)
			low = mid;  // increase loading
		else
			high = mid; // decrease loading
	}

	//kprintf("RTC calibrated to %d Hz\r\n", rtc_freq);
}



uint32	clktime;		/* seconds since boot			*/
//uint32	ctr1000 = 0;		/* milliseconds since boot		*/
qid16	sleepq;			/* queue of sleeping processes		*/
int32	slnempty;		/* zero if the sleep queue is empty;	*/
				/*   non-zero otherwise			*/
int32	*sltop;			/* ptr to key in first entry of sleepq	*/
				/*   if sleepq is not empty		*/
uint32	preempt;		/* preemption counter			*/
   
/*------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue at startup
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{

	/* Initialize RTC, using 2KHz ring oscillator */

	/* Enable ring osc */
	CRM->RINGOSC_CNTLbits.ROSC_EN = 1;
	CRM->XTAL32_CNTLbits.XTAL32_EN = 0;

	/* Set default tune values from datasheet */
	CRM->RINGOSC_CNTLbits.ROSC_CTUNE = 0x6;
	CRM->RINGOSC_CNTLbits.ROSC_FTUNE = 0x17;
	
	/* Trigger calibration */
	rtc_calibrate();

	/* 500Hz clock interrupt rate */
	*CRM_RTC_TIMEOUT = 4;

	clear_rtc_wu_evt();
	enable_rtc_wu();
	enable_rtc_wu_irq();

	sleepq = newqueue();	/* allocate a queue to hold the delta	*/
				/* list of sleeping processes		*/
	preempt = QUANTUM;	/* initial time quantum			*/

	/* Specify that seepq is initially empty */

	slnonempty = FALSE;

	clktime = 0;		/* start counting seconds		*/

	return;
}


void rtc_check(void) {
	int count;

	while (1) {
		CRM->CAL_CNTLbits.CAL_TIMEOUT = RTC_TIMEOUT;
		CRM->STATUSbits.CAL_DONE = 1;
		CRM->CAL_CNTLbits.CAL_EN = 1;
		while (CRM->STATUSbits.CAL_DONE == 0)
		  continue;

		/* Result should ideally be close to (REF_OSC * (timeout / 2000)) */
		count = CRM->CAL_COUNT;
		printf("\nrtc_check count %d freq %d\n", count,
			 REF_OSC / ((count + RTC_TIMEOUT/2) / RTC_TIMEOUT));
		printf("prm_mode %d\n", prm_mode);
		sleep(10);
	}
}

