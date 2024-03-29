/* radout.c - rad_out: put datagrams out on the radio */
#include <xinu.h>


void
rad_out(void)
{
	struct ifentry *ifptr = &if_tab[0];
	struct netpacket *nptr;
	struct ip6hdr *iptr;
	int fragletting, firstfraglet;
	packet_t *p;
	char buf[130], *bptr, *ihc0, *ihc1;

	while (1) {
		wait(ifptr->if_osem);
		/* get next netpacket from output queue	*/
		nptr = ifptr->if_oqueue[ifptr->if_otail];
		ifptr->if_otail = (ifptr->if_otail + 1) % IF_OQUEUESIZE;
		printf("rad_out 0x%08x\n", nptr);
		
		fragletting = firstfraglet = TRUE;
		while (fragletting) {
			/*   get radio device driver buffer	*/

			bptr = buf;
			/*   generate next fraglet; header	*/
			/*     compression if first fraglet	*/
			if (firstfraglet) {
				/* header compression 	*/
				/* check details of first bytes in radio buffer		*/
				/* for now, everything explicitly in-line		*/
				*bptr++ = 0x41;	/* data frame, PAN ID compression	*/
				*bptr++ = 0x88 |
					((nptr->netpkt_dstfmt == RAD_LONG_ADDR) << 2) |
					((nptr->netpkt_srcfmt == RAD_LONG_ADDR) << 6);
						/* address compression from RFC 6282	*/

				*bptr++ = ifptr->if_oseq++; /* frame sequence number	*/

				memrev(bptr, nptr->netpkt_dstpan, 2); /* dst pan ID	*/
				bptr += 2;

				/* assume dst PAN ID == src PAN ID for now */

				/* copy dst addr, using format from netpacket		*/
				if (nptr->netpkt_dstfmt == RAD_SHORT_ADDR) {
					memrev(bptr, nptr->netpkt_raddst, 2);
					bptr += 2;
				} else {
					memrev(bptr, nptr->netpkt_raddst, 8);
					bptr += 8;
				}

				/* copy src addr, using format from netpacket		*/
				if (nptr->netpkt_srcfmt == RAD_SHORT_ADDR) {
					printf("copying short src\n");
					memrev(bptr, nptr->netpkt_radsrc, 2);
					bptr += 2;
				} else {
					printf("copying long src\n");
					memrev(bptr, nptr->netpkt_radsrc, 8);
					bptr += 8;
				}

				/* now do IP header compression				*/
				ihc0 = bptr++;	/* save location of IP header		*/
				ihc1 = bptr++;	/*   compression bytes			*/
				*ihc0 = 0x60;
				iptr = (struct ip6hdr *)&nptr->netpkt_ip6hdr;
				
				/* check ECN and traffic class == 0			*/
				if ((iptr->net_iptclass == 0) && (iptr->net_ipflowlabel == 0)) {
					*ihc0 |= 0x18;	/* 3 => iptclass, flow label 0	*/
				}

				/* check next header					*/
				switch (iptr->net_ipnxthdr) {
					/* combine ICMPv6 and other non-UDP headers?	*/
				case PROTO_ICMPV6:
					/* send ICMP in line				*/
					*bptr++ = PROTO_ICMPV6;
					break;

				case PROTO_UDP:
					/* compress UDP					*/
					*ihc0 |= 0x4;
					/* so, now go compress that UDP header...	*/
					break;

				default:
					/* don't compress				*/
					*bptr++ = iptr->net_ipnxthdr;
				}

				/* hlim							*/
				switch (iptr->net_iphoplim) {
				case 1:
					*ihc0 |= 1;
					break;

				case 64:
					*ihc0 |= 2;
					break;

				case 255:
					*ihc0 |= 3;
					break;

				default:
					*bptr++ = iptr->net_iphoplim;
					break;
				}

				/* CID - no contexts					*/
				/* source address					*/
				/* destination address					*/
				*ihc1 = 0;	/* no compression			*/
				memcpy(bptr, iptr->net_ipsrc, 16);
				bptr += 16;
				memcpy(bptr, iptr->net_ipdst, 16);
				bptr += 16;
				memcpy(bptr, nptr->netpkt_ip6nxthdr, iptr->net_ip6len);
			}
			printhex(buf, (bptr - buf) + iptr->net_ip6len);
			/*   hand radio device driver buffer to	*/
			/*     radio driver			*/
			write(RADIO0, buf, (bptr - buf) + iptr->net_ip6len); 
			fragletting = FALSE;
		}
		/* return netpacket buffer		*/
		printf("rad_out freeing 0x%08x\n", nptr);
		freebuf((char *)nptr);
	}
}
