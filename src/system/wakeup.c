/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */

        if(rpl_dao_timeout == 0){
                struct rpl_sim_packet pkt ;
                pkt->msg_type = RPL_TIMER_EXPIRY;
                wait(rpl_sim_write_sem);
                memcpy
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		ready(dequeue(sleepq), RESCHED_NO);
	}
	
	if ( (slnonempty = nonempty(sleepq)) == TRUE ) {
		sltop = &queuetab[firstid(sleepq)].qkey;
	}
	resched();
	return;
}

