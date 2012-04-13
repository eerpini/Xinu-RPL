/* netin.c - netin, ipcksum, eth_ntoh, eth_hton, ip_ntoh, ip_hton */

#include <xinu.h>

bpid32	netbufpool;			/* ID of network buffer pool	*/
struct	eth_packet *currpkt;		/* packet buffer being used now	*/
struct	network NetData;		/* local network interface	*/

/*------------------------------------------------------------------------
 * netin - continuously read the next incoming packet and handle it
 *------------------------------------------------------------------------
 */

process	netin(void) {

	status	retval;			/* return value from function	*/
        int i=0;
        struct ipv4_packet* ippkt = NULL;

        /*
         * FIXME : This might now work
         */
	netbufpool = mkbufpool(PACKLEN, UDP_SLOTS * UDP_QSIZ +
				ICMP_SLOTS * ICMP_QSIZ + ICMP_OQSIZ + 1);

	if (netbufpool == SYSERR) {
		kprintf("Cannot allocate network buffer pool");
		kill(getpid());
	}

	/* Copy Ethernet address to global variable */

	control(ETHER0, ETH_CTRL_GET_MAC, (int32)NetData.ethaddr, 0);

	/* Indicate that IP address, mask, and router are not yet valid	*/

	NetData.ipvalid = FALSE;

	NetData.ipaddr = 0;
	NetData.addrmask = 0;
	NetData.routeraddr = 0;


        kprintf("My Mac is : %06x\r\n", *(NetData.ethaddr+4));

	/* Initialize ARP cache */

	arp_init();

	/* Initialize UDP table */

	udp_init();

        
	currpkt = (struct eth_packet *)getbuf(netbufpool);

	/* Do forever: read packets from the network and process */

	while(1) {
                //kprintf("Reading in netin\r\n");
	    	retval = read(ETHER0, (char *)currpkt, PACKLEN);
                //kprintf("Read a packet from ethernet\r\n");
	    	if (retval == SYSERR) {
			panic("Ethernet read error");
	    	}

                //pdump(currpkt);
		/* Convert Ethernet Type to host order */
		eth_ntoh(currpkt);
		/* Demultiplex on Ethernet type */

		switch (currpkt->net_ethtype) {

			case ETH_ARP:
				arp_in();	/* Handle an ARP packet	*/
				continue;

			case ETH_IP:
                                //kprintf("Ip packet received \r\n");
                                ippkt = (struct ipv4_packet *)(currpkt->net_ethdata);

				if (ipcksum(ippkt) != 0) {
					kprintf("checksum failed\n\r");
					continue;
				}

				if (ippkt->net_ipvh != 0x45) {
					kprintf("version failed\n\r");
					continue;
				}

				/* Convert IP packet to host order */
                                ip_ntoh(ippkt);

                                if ( (ippkt->net_ipdst != IP_BCAST) &&
				     (NetData.ipvalid) &&
				     (ippkt->net_ipdst != NetData.ipaddr) ) {
                                        //kprintf("IP Packet not destined for us\r\n");
					continue;
				}

				/* Demultiplex UDP and ignore others */

				if (ippkt->net_ipproto == IP_UDP) {
                                        //kprintf("Trying to handle a UDP packet\r\n");
					udp_in();/* Handle a UDP packet	*/
				}else if (ippkt->net_ipproto == IP_ICMP){
                                        kprintf("ICMP Packet \r\n");
					icmp_in();/* Handle ICMP packet */
				}
				continue;

                        case 0x1000:
                                kprintf("Received a simulator packet\r\n");
                                wait(rpl_sim_write_sem);
                                memcpy((char *)&sim_queue[i], currpkt->net_ethdata, sizeof(struct rpl_sim_packet));
                                kprintf("Netin working with iter [%d] message type : %d dest : %04x source : %04x\r\n",i, sim_queue[i].msg_type, *((uint32 *)(sim_queue[i].dest_node)), *((uint32 *)(sim_queue[i].src_node)));
                                signal(rpl_sim_read_sem);
                                i = (i+1)%RPL_SIM_RECV_QUEUE_LEN;
                                continue;

			default: /* Ignore all other Ethernet types */
                                kprintf("Nothing \r\n");
				continue;		
		}
	}
}


/*------------------------------------------------------------------------
 * ipcksum - compute the IP checksum for a packet
 *------------------------------------------------------------------------
 */

uint16	ipcksum(
	 struct  ipv4_packet *pkt		/* ptr to a packet		*/
	)
{
	uint16	*hptr;			/* ptr to 16-bit header values	*/
	int32	i;			/* counts 16-bit values in hdr	*/
	uint32	cksum;			/* computed value of checksum	*/

	hptr= (uint16 *) &pkt->net_ipvh;
	cksum = 0;
	for (i=0; i<10; i++) {
		cksum += (uint32) *hptr++;
	}
	cksum += (cksum >> 16);
	cksum = 0xffff & ~cksum;
	if (cksum == 0xffff) {
		cksum = 0;
	}
	return (uint16) (0xffff & cksum);
}

/*------------------------------------------------------------------------
 * eth_ntoh - convert Ethernet type field to host byte order
 *------------------------------------------------------------------------
 */
void 	eth_ntoh(
	  struct eth_packet *pktptr
	)
{
	pktptr->net_ethtype = ntohs(pktptr->net_ethtype);
}

/*------------------------------------------------------------------------
 * eth_hton - convert Ethernet type field to network byte order
 *------------------------------------------------------------------------
 */
void 	eth_hton(
	  struct eth_packet *pktptr
	)
{
	pktptr->net_ethtype = htons(pktptr->net_ethtype);
}

/*------------------------------------------------------------------------
 * ip_ntoh - convert IP header fields to host byte order
 *------------------------------------------------------------------------
 */
void 	ip_ntoh(
	  struct ipv4_packet *pktptr
	)
{
	pktptr->net_iplen = ntohs(pktptr->net_iplen);
	pktptr->net_ipid = ntohs(pktptr->net_ipid);
	pktptr->net_ipfrag = ntohs(pktptr->net_ipfrag);
	pktptr->net_ipsrc = ntohl(pktptr->net_ipsrc);
	pktptr->net_ipdst = ntohl(pktptr->net_ipdst);
}

/*------------------------------------------------------------------------
 * ip_hton - convert IP header fields to network byte order
 *------------------------------------------------------------------------
 */
void 	ip_hton(
	  struct ipv4_packet *pktptr
	)
{
	pktptr->net_iplen = htons(pktptr->net_iplen);
	pktptr->net_ipid = htons(pktptr->net_ipid);
	pktptr->net_ipfrag = htons(pktptr->net_ipfrag);
	pktptr->net_ipsrc = htonl(pktptr->net_ipsrc);
	pktptr->net_ipdst = htonl(pktptr->net_ipdst);
}
