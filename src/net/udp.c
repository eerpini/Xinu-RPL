/* udp.c - udp_init, udp_in, udp_register, udp_send, udp_recv,		*/
/*		 udp_recvaddr, udp_release, udp_ntoh, udp_hton		*/

#include <xinu.h>

struct	udpentry udptab[UDP_SLOTS];		/* table of UDP endpts	*/

/*------------------------------------------------------------------------
 * udp_init - initialize UDP endpoint table
 *------------------------------------------------------------------------
 */
void	udp_init(void) {

	int32	i;			/* table index */

	for(i=0; i<UDP_SLOTS; i++) {
		udptab[i].udstate = UDP_FREE;
	}

	return;
}

/*------------------------------------------------------------------------
 * udp_in - handle an incoming UDP packet
 *------------------------------------------------------------------------
 */
void	udp_in(void) {			/* currpkt points to the packet	*/

	intmask	mask;			/* saved interrupt mask		*/
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
        struct  ipv4_packet *ippkt = (struct ipv4_packet *)(currpkt->net_ethdata);
        struct  udp_packet * udppkt = (struct udp_packet *)(ippkt->net_ipdata);

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

        //kprintf("Inside udp_in\r\n");
	/* Convert IP and UDP header fields to host byte order */

	udp_ntoh(udppkt);



        /*
        if (ippkt->net_ipproto == IP_UDP) {
                kprintf("proto UDP (%d), length %d",
                                ippkt->net_ipproto, ntohs(ippkt->net_iplen));
                kprintf(")\n");
                kprintf("\t%d.%d.%d.%d > ",
                                ((ippkt->net_ipsrc)>>24)&0xff,
                                ((ippkt->net_ipsrc)>>16)&0xff,
                                ((ippkt->net_ipsrc)>>8)&0xff, 
                                ((ippkt->net_ipsrc)&0xff));
                kprintf("%d.%d.%d.%d: ",
                                ((ippkt->net_ipdst)>>24)&0xff,
                                ((ippkt->net_ipdst)>>16)&0xff,
                                ((ippkt->net_ipdst)>>8)&0xff, 
                                ((ippkt->net_ipdst)&0xff));
                //kprintf("PDUMP Check 9\r\n");
                kprintf("[udp checksum none] ");
                kprintf("UDP, src port %d, dst port %d, length %d\n",
                                (udppkt->net_udpsport),
                                (udppkt->net_udpdport),
                                (udppkt->net_udplen) - UDP_HDR_LEN);
        }
        */


	for (i=0; i<UDP_SLOTS; i++) {
	    udptr = &udptab[i];
	    if ( (udptr->udstate != UDP_FREE) &&
		 (udppkt->net_udpdport == udptr->udlocport)  &&
		    ((udptr->udremport == 0) ||
			(udppkt->net_udpsport == udptr->udremport)) &&
		 (  ((udptr->udremip==0)     ||
			(ippkt->net_ipsrc == udptr->udremip)))    ) {

		/* Entry matches incoming packet */

		if (udptr->udcount < UDP_QSIZ) {
			udptr->udcount++;
			udptr->udqueue[udptr->udtail++] = (struct eth_packet *)currpkt;
			if (udptr->udtail >= UDP_QSIZ) {
				udptr->udtail = 0;
			}
			currpkt = (struct eth_packet *)getbuf(netbufpool);
			if (udptr->udstate == UDP_RECV) {
				udptr->udstate = UDP_USED;
				send (udptr->udpid, OK);
			}
			restore(mask);
			return;
		}
	    }
	}

	/* No match - simply discard packet */
        //kprintf("Done with udp_in\r\n");
	restore(mask);
	return;
}

/*------------------------------------------------------------------------
 * udp_register - register a remote (IP,port) and local port to receive
 *		  incoming UDP messages from the specified remote site
 *		  sent to a specific local port
 *------------------------------------------------------------------------
 */
status	udp_register (
	 uint32	remip,			/* remote IP address or zero	*/
	 uint16	remport,		/* remote UDP protocol port	*/
	 uint16	locport			/* local UDP protocol port	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();

	/* See if request already registered */

	for (i=0; i<UDP_SLOTS; i++) {
		udptr = &udptab[i];
		if (udptr->udstate == UDP_FREE) {
			continue;
		}
		if ((remport == udptr->udremport) &&
			(locport == udptr->udlocport) &&
			(remip   == udptr->udremip  ) ) {

			/* Entry in table matches request */

			restore(mask);
			return SYSERR;
		}
	}

	/* Find a free slot and allocate it */

	for (i=0; i<UDP_SLOTS; i++) {
		udptr = &udptab[i];
		if (udptr->udstate == UDP_FREE) {
			udptr->udstate = UDP_USED;
			udptr->udlocport = locport;
			udptr->udremport = remport;
			udptr->udremip = remip;
			udptr->udcount = 0;
			udptr->udhead = udptr->udtail = 0;
			udptr->udpid = -1;
			restore(mask);
			return OK;
		}
	}

	restore(mask);
	return SYSERR;
}

/*------------------------------------------------------------------------
 * udp_recv - receive a UDP packet
 *------------------------------------------------------------------------
 */
int32	udp_recv (
	 uint32	remip,			/* remote IP address or zero	*/
	 uint16	remport,		/* remote UDP protocol port	*/
	 uint16	locport,		/* local UDP protocol port	*/
	 char   *buff,			/* buffer to hold UDP data	*/
	 int32	len,			/* length of buffer		*/
	 uint32	timeout			/* read timeout in msec		*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	umsg32	msg;			/* message from recvtime()	*/
	struct	eth_packet *pkt = NULL;		/* ptr to packet being read	*/
        struct  ipv4_packet *ippkt = NULL;
        struct  udp_packet * udppkt = NULL;
	int32	msglen;			/* length of UDP data in packet	*/
	char	*udataptr;		/* pointer to UDP data		*/

	/* Insure only one process can access the UDP table at a time */

	mask = disable();
	for (i=0; i<UDP_SLOTS; i++) {
		udptr = &udptab[i];
		if ((remport == udptr->udremport) &&
			(locport == udptr->udlocport) &&
			(remip   == udptr->udremip  ) ) {

			/* Entry in table matches request */

			break;
		}
	}

	if (i >= UDP_SLOTS) {
		restore(mask);
		return SYSERR;
	}

	if (udptr->udcount == 0) {		/* No packet is waiting	*/
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* Wait for a packet	*/
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
        ippkt = (struct ipv4_packet *)(pkt->net_ethdata);
        udppkt = (struct udp_packet *)(ippkt->net_ipdata);
	if (udptr->udhead >= UDP_SLOTS) {
		udptr->udhead = 0;
	}
	udptr->udcount--;

	/* Copy UDP data from packet into caller's buffer */

	msglen = udppkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)udppkt->net_udpdata;
	for (i=0; i<msglen; i++) {
		if (i >= len) {
			break;
		}
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return i;
}

/*------------------------------------------------------------------------
 * udp_recvaddr - receive a UDP packet and record the sender's address
 *------------------------------------------------------------------------
 */
int32	udp_recvaddr (
	 uint32	*remip,			/* loc to record remote IP addr.*/
	 uint16	*remport,		/* loc to record remote port	*/
	 uint16	locport,		/* local UDP protocol port	*/
	 char   *buff,			/* buffer to hold UDP data	*/
	 int32	len,			/* length of buffer		*/
	 uint32	timeout			/* read timeout in msec		*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	umsg32	msg;			/* message from recvtime()	*/
	struct	eth_packet *pkt;		/* ptr to packet being read	*/
        struct  ipv4_packet *ippkt = NULL;
        struct  udp_packet * udppkt = NULL;
	int32	msglen;			/* length of UDP data in packet	*/
	char	*udataptr;		/* pointer to UDP data		*/

	/* Insure only one process access UDP table at a time */

	mask = disable();

	for (i=0; i<UDP_SLOTS; i++) {
		udptr = &udptab[i];
		if ( (udptr->udremip == 0 ) &&
			(locport == udptr->udlocport) ) {

			/* Entry in table matches request */
			break;
		}
	}

	if (i >= UDP_SLOTS) {
		restore(mask);
		return SYSERR;
	}

	if (udptr->udcount == 0) {	/* no packet is waiting */
		udptr->udstate = UDP_RECV;
		udptr->udpid = currpid;
		msg = recvclr();
		msg = recvtime(timeout);	/* wait for packet */
		udptr->udstate = UDP_USED;
		if (msg == TIMEOUT) {
			restore(mask);
			return TIMEOUT;
		} else if (msg != OK) {
			restore(mask);
			return SYSERR;
		}
	}

	/* Packet has arrived -- dequeue it */

	pkt = udptr->udqueue[udptr->udhead++];
        ippkt = (struct ipv4_packet *)(pkt->net_ethdata);
        udppkt = (struct udp_packet *)(ippkt->net_ipdata);
	if (udptr->udhead >= UDP_SLOTS) {
		udptr->udhead = 0;
	}
	udptr->udcount--;

	/* Record sender's IP address and UDP port number */

	*remip = ippkt->net_ipsrc;
	*remport = udppkt->net_udpsport;

	/* Copy UDP data from packet into caller's buffer */

	msglen = udppkt->net_udplen - UDP_HDR_LEN;
	udataptr = (char *)udppkt->net_udpdata;
	for (i=0; i<msglen; i++) {
		if (i >= len) {
			break;
		}
		*buff++ = *udataptr++;
	}
	freebuf((char *)pkt);
	restore(mask);
	return i;
}

/*------------------------------------------------------------------------
 * udp_send - send a UDP packet
 *------------------------------------------------------------------------
 */
status	udp_send (
	 uint32	remip,			/* remote IP address or IP_BCAST*/
					/*  for a local broadcast	*/
	 uint16	remport,		/* remote UDP protocol port	*/
	 uint32	locip,			/* local IP address		*/
	 uint16	locport,		/* local UDP protocol port	*/
	 char   *buff,			/* buffer of UDP data		*/
	 int32	len			/* length of data in buffer	*/
	)
{
	struct	eth_packet pkt;		/* ptr to packet being read	*/
        struct ipv4_packet *ippkt = (struct ipv4_packet *)(pkt.net_ethdata);
        struct udp_packet *udppkt = (struct udp_packet *)(ippkt->net_ipdata);
	int32	pktlen;			/* total packet length		*/
	static	uint16 ident = 1;	/* datagram IDENT field		*/
	char	*udataptr;		/* pointer to UDP data		*/
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

	/* Compute packet length as UDP data size + fixed header size	*/

	pktlen = ((char *)(udppkt->net_udpdata) - (char *)&pkt) + len;

	/* Create UDP packet in pkt */

	memcpy(pkt.net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        pkt.net_ethtype = 0x0800;	/* Type is IP */
	ippkt->net_ipvh = 0x45;		/* IP version and hdr length	*/
	ippkt->net_iptos = 0x00;		/* Type of service		*/
	ippkt->net_iplen= pktlen - ETH_HDR_LEN;/* total IP datagram length	*/
	ippkt->net_ipid = ident++;		/* datagram gets next IDENT	*/
	ippkt->net_ipfrag = 0x0000;	/* IP flags & fragment offset	*/
	ippkt->net_ipttl = 0xff;		/* IP time-to-live		*/
	ippkt->net_ipproto = IP_UDP;	/* datagram carries UDP		*/
	ippkt->net_ipcksum = 0x0000;	/* initial checksum		*/
	ippkt->net_ipsrc = locip;		/* IP source address		*/
	ippkt->net_ipdst = remip;		/* IP destination address	*/

	udppkt->net_udpsport = locport;	/* local UDP protocol port	*/
	udppkt->net_udpdport = remport;	/* remote UDP protocol port	*/
	udppkt->net_udplen = (uint16)(UDP_HDR_LEN+len); /* UDP length	*/
	udppkt->net_udpcksum = 0x0000;	/* ignore UDP checksum		*/
	udataptr = (char *) udppkt->net_udpdata;
	for (; len>0; len--) {
		*udataptr++ = *buff++;
	}

	/* Set MAC address in packet, using ARP if needed */

        //kprintf("Udp_send called with ip : %x\r\n", remip);
	if (remip == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(pkt.net_ethdst, ethbcast, ETH_ADDR_LEN);

	/* If destination isn't on the same subnet, send to router	*/

	} else if ((locip & NetData.addrmask)
			!= (remip & NetData.addrmask)) {
                //kprintf("localip [%08x] NetData.addrmask [%08x] remote ip [%08x] 12 [%08x] 23 [%08x]\r\n",
                                //locip, NetData.addrmask, remip, locip&NetData.addrmask, remip & NetData.addrmask);
		if (arp_resolve(NetData.routeraddr, pkt.net_ethdst)!=OK) {
		    kprintf("udp_send: cannot resolve router %08x\n\r",
				NetData.routeraddr);
		    return SYSERR;
		}
	} else {
		/* Destination is on local subnet -  get MAC address */

                //kprintf("UDP_Send trying to resolve address : %x\r\n", remip);
		if (arp_resolve(remip, pkt.net_ethdst) != OK) {
			kprintf("udp_send: cannot resolve %08x\n\r",remip);
			return SYSERR;
		}
	}

	/* Convert IP and UDP header fields from net to host byte order */

	udp_hton(udppkt);
	ip_hton(ippkt);
	eth_hton(&pkt);
        //kprintf("Cleared conversion of header fields\r\n");

	/* Compute IP header checksum */
	ippkt->net_ipcksum = 0xffff & ipcksum(ippkt);

        //kprintf("Writing to ethernet device\r\n");
	write(ETHER0, (char *)&pkt, pktlen);

	return OK;
}

/*------------------------------------------------------------------------
 * udp_release - release a previously-registered remote IP, remote
 *			port, and local port (exact match required)
 *------------------------------------------------------------------------
 */
status	udp_release (
	 uint32	remip,			/* remote IP address or zero	*/
	 uint16	remport,		/* remote UDP protocol port	*/
	 uint16	locport			/* local UDP protocol port	*/
	)
{
	int32	i;			/* index into udptab		*/
	struct	udpentry *udptr;	/* pointer to udptab entry	*/
	struct	eth_packet *pkt;		/* ptr to packet being read	*/

	for (i=0; i<UDP_SLOTS; i++) {
		udptr = &udptab[i];
		if (udptr->udstate != UDP_USED) {
			continue;
		}
		if ((remport == udptr->udremport) &&
			(locport == udptr->udlocport) &&
			(remip   == udptr->udremip  ) ) {

			/* Entry in table matches */

			sched_cntl(DEFER_START);
			while (udptr->udcount > 0) {
				pkt = udptr->udqueue[udptr->udhead++];
				if (udptr->udhead >= UDP_SLOTS) {
					udptr->udhead = 0;
				}
				freebuf((char *)pkt);
				udptr->udcount--;
			}
			udptr->udstate = UDP_FREE;
			sched_cntl(DEFER_STOP);
			return OK;
		}
	}
	return SYSERR;
}

/*------------------------------------------------------------------------
 * udp_ntoh - convert UDP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
void 	udp_ntoh(
	struct udp_packet *pktptr
	)
{
	pktptr->net_udpsport = ntohs(pktptr->net_udpsport);
	pktptr->net_udpdport = ntohs(pktptr->net_udpdport);
	pktptr->net_udplen = ntohs(pktptr->net_udplen);
	return;
}

/*------------------------------------------------------------------------
 * udp_hton - convert packet header fields from host to net byte order
 *------------------------------------------------------------------------
 */
void 	udp_hton(
	struct udp_packet *pktptr
	)
{
	pktptr->net_udpsport = htons(pktptr->net_udpsport);
	pktptr->net_udpdport = htons(pktptr->net_udpdport);
	pktptr->net_udplen = htons(pktptr->net_udplen);
	return;
}
