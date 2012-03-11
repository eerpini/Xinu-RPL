/* net6.c - net6_init, net_in */

#include <xinu.h>
#include <stdio.h>

struct ndcache_entry ndcache[NNDCACHE];

/*------------------------------------------------------------------------
 * net_init - initialize network data structures and processes
 *------------------------------------------------------------------------
 */

void	net_init (void)
{
	int i;
	struct ndcache_entry *nptr;

	netiface_init();

	/* Initialize neighbor caches */
	for (i = 0; i < NNDCACHE; i++) {
		nptr = &ndcache[i];
		memset(nptr->nd_nghbrIP, 0, 16);
		memset(nptr->nd_nghbrMAC64, 0, 8);
		memset(&nptr->nd_nghbrMAC16, 0, 2);
		nptr->nd_pending = (struct netpacket *)NULL;
		nptr->nd_isrouter = 0;
		nptr->nd_state = ND_EMPTY;
		nptr->nd_timer = 0;
	}

	/* Create network input process */
	resume(create((void *)net_in, INITSTK, INITPRIO,
		      "net_in process", 0, NULL));

	/* Create radio input process */
	resume(create((void *)rad_in, INITSTK, INITPRIO,
		      "rad_in process", 0, NULL));

	resume(create((void *)rad_out, INITSTK, INITPRIO,
		      "rad_out process", 0, NULL));

	kprintf("returning from net_init\n");

}

process
net_in (void)
{
	struct	ifentry	*ifptr = &if_tab[0];
	struct	netpacket *nptr;

	/* read packets from network interface and process */
	while (1) {
		/* Obtain next packet */
		wait(ifptr->if_isem);
		nptr = ifptr->if_iqueue[ifptr->if_itail];
		ifptr->if_itail = (ifptr->if_itail + 1) % IF_IQUEUESIZE;

		pdump6(nptr);

		/* Demultiplex on hext header */
		/* This may need to be in a loop (?!?) to process extension headers */
		/* Notes:
		   ICMPv6, UDP, TCP are all terminal payloads
		   Others require further processing
		*/

#ifdef NOTDEF
		switch (netpkt->netpkt_ip6hdr.net_ipnxthdr) {
		case ICMPv6:
			break;

		case UDP:
			break;

		case TCP:
			break;

		case HOPBYHOP:
			break;

		case RPL:
			break;
#endif
		freebuf((char *)nptr);
	}
}

void
net_out(struct netpacket *opkt)
{
	struct	ifentry	*ifptr = &if_tab[0];

	kprintf("net_out 0x%08x\n", opkt);
	/* fill in radio addresses as required	*/
	if (opkt->netpkt_dstvalid == FALSE) {
		printf("need to resolve destination radio address\n");
		freebuf((char *)opkt);
	}
	if (opkt->netpkt_srcvalid == FALSE) {
		printf("need to resolve source radio address\n");
		freebuf((char *)opkt);
	}
	
	/* fill in other fields (?)		*/
	
	/* put on oqueue			*/
	/* is there a mutual exclusion issue here?	*/
	ifptr->if_oqueue[ifptr->if_ohead] = opkt;
	ifptr->if_ohead = (ifptr->if_ohead + 1) % IF_OQUEUESIZE;
	kprintf("net_out head %d tail %d\n", ifptr->if_ohead, ifptr->if_otail);	
	/* signal rad_out			*/
	signal(ifptr->if_osem);
}
