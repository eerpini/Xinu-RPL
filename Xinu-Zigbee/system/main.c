/*  main.c  - main */

#include <xinu.h>
#include <mc1322x.h>
#include <stdio.h>

uint16	chksum16(uint16 *bptr, int len)
{
	uint16	word;
	uint32	cksum;
	int	i;

	cksum = 0;
	for (i=0; i<((len+1)/2); i++) {
		word = *bptr++;
		cksum += word;
	}
	cksum += (cksum >> 16);
	cksum = 0xffff & ~cksum;
	if (cksum == 0xffff) {
		cksum = 0;
	}
	return (uint16) (0xffff & cksum);
}

uint8 ping_addr[16] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

int32	main(void)

{
	char c;
	struct	netpacket	*nptr;
	struct	ip6hdr		*iptr;
	struct	icmphdr		*icmpptr;
	int seq = 0;
	int transmitting = FALSE;

	printf("\n\n##############################################\n\n");

	printf("enter t to transmit\n");
	c = getc(CONSOLE);
	if (c == 't') transmitting = TRUE;
	while (1) {
      		/* Implement a shell here
		 */
		/* c = getc(CONSOLE);	//Scan available channels
		 * chan = (chan + 1) % 16;
		 * set_channel(chan);
		 * printf("channel %d\n", chan);
		 */

		if (transmitting) {
		nptr = (struct netpacket *)getbuf(if_tab[0].if_ipool);
		nptr->netpkt_dstfmt = RAD_SHORT_ADDR;
		nptr->netpkt_srcfmt = RAD_LONG_ADDR;
		nptr->netpkt_dstvalid = nptr->netpkt_srcvalid = TRUE;
		memset(nptr->netpkt_raddst, 0, RAD_ADDR_LEN);
		memset(nptr->netpkt_raddst, 0xff, 2);
		memcpy(nptr->netpkt_radsrc, if_tab[0].if_macucast, RAD_ADDR_LEN);
		nptr->netpkt_dstpan[0] = nptr->netpkt_srcpan[0] = 0xde;
		nptr->netpkt_dstpan[1] = nptr->netpkt_srcpan[1] = 0xad;
		iptr = &(nptr->netpkt_ip6hdr);
		iptr->net_ipvers = 6;
		iptr->net_iptclass = 0;
		iptr->net_ipflowlabel = 0;
		iptr->net_ip6len = 48;
		iptr->net_ipnxthdr = PROTO_ICMPV6;
		iptr->net_iphoplim = 255;
		memcpy(iptr->net_ipsrc, if_tab[0].if_ip6ucast[1], 16);
		memcpy(iptr->net_ipdst, ping_addr, 16);
		icmpptr = (struct icmphdr *)&(nptr->netpkt_ip6nxthdr);
		icmpptr->net_ictype = 128;
		icmpptr->net_iccode = 0;
		icmpptr->net_iccksum = 0;	/* for checksum computation */
		icmpptr->net_icident = 0;
		icmpptr->net_icseq = seq++;
		icmpptr->net_iccksum = chksum16((uint16 *)&(nptr->netpkt_ip6nxthdr), 8);
		pdump6(nptr);
		printf("sending 0x%08x\n", nptr);
		net_out(nptr);
		}
		sleep(5);
	}
	return OK;
}

