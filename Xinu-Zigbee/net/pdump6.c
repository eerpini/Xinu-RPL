/* pdump6.c - net packet dump function */

#include <xinu.h>

void pdump6(struct netpacket *pptr)
{
	struct ip6hdr *iptr = &pptr->netpkt_ip6hdr;
	struct icmphdr *icmpptr = (struct icmphdr *)(pptr->netpkt_ip6nxthdr);
	struct udphdr *uptr = (struct udphdr *)(pptr->netpkt_ip6nxthdr);

	if(pptr->netpkt_dstfmt == RAD_SHORT_ADDR)
		kprintf("%02x:%02x/", 
			pptr->netpkt_raddst[0],
			pptr->netpkt_raddst[1]
			);
	else
		kprintf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x/",
			pptr->netpkt_raddst[0],
			pptr->netpkt_raddst[1],
			pptr->netpkt_raddst[2],
			pptr->netpkt_raddst[3],
			pptr->netpkt_raddst[4],
			pptr->netpkt_raddst[5],
			pptr->netpkt_raddst[6],
			pptr->netpkt_raddst[7]
			);
	kprintf("%02x%02x < ", pptr->netpkt_dstpan[0], pptr->netpkt_dstpan[1]);

	if(pptr->netpkt_srcfmt == RAD_SHORT_ADDR)
		kprintf("%02x:%02x/", 
			pptr->netpkt_radsrc[0],
			pptr->netpkt_radsrc[1]
			);
	else
		kprintf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x/",
			pptr->netpkt_radsrc[0],
			pptr->netpkt_radsrc[1],
			pptr->netpkt_radsrc[2],
			pptr->netpkt_radsrc[3],
			pptr->netpkt_radsrc[4],
			pptr->netpkt_radsrc[5],
			pptr->netpkt_radsrc[6],
			pptr->netpkt_radsrc[7]
			);
	kprintf("%02x%02x\n", pptr->netpkt_dstpan[0], pptr->netpkt_dstpan[1]);

	kprintf("LQI 0x%02x\n", pptr->netpkt_lqi);

	kprintf("IPv6 length %d, next header %d, tclass %02x, flowlabel %03x, hop limit %d\n",
		iptr->net_ip6len,
		iptr->net_ipnxthdr,
		iptr->net_iptclass,
		iptr->net_ipflowlabel,
		iptr->net_iphoplim);

	kprintf("src ");
	kprint6addr(iptr->net_ipsrc);
	kprintf("\ndst ");
	kprint6addr(iptr->net_ipdst);
	kprintf("\n");

	switch(iptr->net_ipnxthdr) {
	case PROTO_ICMPV6:
		kprintf("ICMPv6 message type %d, code %d, ident 0x%04x, seq 0x%04x\n",
			icmpptr->net_ictype,
			icmpptr->net_iccode,
			icmpptr->net_icident,
			icmpptr->net_icseq);
		switch (icmpptr->net_ictype) {
		case ICMPV6_ECHOREQUEST: /* ICMPv6 echo request */
			kprintf("ICMPv6 echo request\n");
			break;

		case ICMPV6_ECHOREPLY:
			kprintf("ICMPv6 echo reply\n");
			break;

		case ICMPV6_ROUTERSOLICITATION: /* router solicitation */
			kprintf("router solicitation\n");
			break;
				
		case ICMPV6_NEIGHBORSOLICITATION: /* neighbor solicitation */
			kprintf("neighbor solicitation\n");
			kprint6addr(icmpptr->net_icdata);
			kprintf("\n");
			break;
				
		case ICMPV6_RPLCONTROL: /* RPL Control Message */
			kprintf("RPL control message type %d\n", icmpptr->net_iccode);
			switch (icmpptr->net_iccode) {
			case RPL_DODAGINFOSOLICITATION:
				kprintf("DODAG Information Solicitation\n");
				break;
					
			case RPL_DIO:
				kprintf("DODAG Information Object\n");
				break;
					
			case RPL_DAO:
				kprintf("Destination Advertisement Object\n");
				
			case RPL_DAOACK:
				kprintf("Destination Advertisement Object Acknowledgment\n");

			case RPL_SECUREDODAGINFOSOLICITATION:
				kprintf("Secure DODAG Information Solicitation\n");

			case RPL_SECUREDIO:
				kprintf("Secure DODAG Information Object\n");
				break;
					
			case RPL_SECUREDAO:
				kprintf("Secure Destination Advertisement Object\n");
				break;

			case RPL_SECUREDAOACK:
				kprintf("Secure Destination Advertisement Object Acknowledgment\n");
				break;

			case RPL_CONSISTENCYCHECK:
				kprintf("Consistency Check\n");
				break;

			default:
				kprintf("Unkown Code 0x%02x\n", icmpptr->net_iccode);
			}
			break;

			default:
				kprintf("unknown ICMPv6 message 0x%02x\n", icmpptr->net_ictype);
			}
		break;

	case PROTO_UDP:
		kprintf("UDP src 0x%04x dst 0x%04x len %d\n",
		       uptr->net_udpsport,
		       uptr->net_udpdport,
		       uptr->net_udplen);
		/* code here to dump payload */
		break;

	default:
		kprintf("unknown protocol\n");
	}
}


