#include <xinu.h>

extern int clkticks, nirqpass;

/* 
 * IPv6 link-local prefix; used for both long and short radio address
 *   IPv6 address compression
 */

uint8 link_local_pref[14] =
	{0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xfe};

#define FRAME802154_SHORTADDRMODE   (0x02)
#define FRAME802154_LONGADDRMODE    (0x03)

void print6addr(unsigned char *addr)
{
	int i;

	for (i = 0; i < 15; i++, addr++)
	  printf("%02x:", *addr);
	printf("%02x", *addr);
}

void printradaddr(unsigned char *addr)
{
	int i;

	for (i = 0; i < 7; i++, addr++)
	  printf("%02x:", *addr);
	printf("%02x", *addr);
}



void *memrev(void *s, const void *ct, int n)
{
    register int i;
    char *dst = (char *)s;
    char *src = (char *)ct + n;

    for (i = 0; i < n; i++)
    {
        *dst++ = *--src;
    }
    return s;
}

void packet_decode(unsigned char *bptr, int length)
{
	unsigned char *bstrt;

	unsigned char cid, cid_extension;

	uint8 mhc0, mhc1, ihc0, ihc1;

	struct netpacket *nptr;
	struct udphdr	 *uptr;

	int	lowpan_nhc = FALSE, cksum_c;

	nptr = (struct netpacket *)getbuf(if_tab[0].if_ipool);
	if ((int)nptr == SYSERR) {
		printf("no packet buffers in rad_in\n");
		return;
	}

	/* keep track of beginning of received packet for length computation */
	bstrt = bptr;

	/* first byte from read is LQI */
	/* look into opporunity to eliminate a memory copy */
	nptr->netpkt_lqi = *bptr++;

	if (*bptr & 8) {
		printf("decoding packet with security bit set\n");
	}


	/* header compression flags in next two bytes */
	mhc0 = *bptr++;
	mhc1 = *bptr++;

	/* skip sequence number */
	bptr++;

	/* Insert code for fragmentation here */

	/* unpack radio addresses */
	/* zero radio addresses and PAN IDs*/
	memset(nptr->netpkt_raddst, 0, 8);
	memset(nptr->netpkt_radsrc, 0, 8);
	memset(nptr->netpkt_dstpan, 0, 2);
	memset(nptr->netpkt_srcpan, 0, 2);
	nptr->netpkt_dstvalid = FALSE;
	nptr->netpkt_srcvalid = FALSE;

	/* frame type	*/
	switch (mhc0 & 7) {
	case 1:
		break;

	default:
		printf("unknown frame type\n");
		freebuf((char *)nptr);
		return;
	}

	if ((mhc1 >> 2) & 3) {	/* dst PAN ID is next */
		memrev(&nptr->netpkt_dstpan, bptr, 2);
		bptr += 2;
	}

	/* dst address is next; 16 or 64 bits?	*/
	switch ((mhc1 >> 2) & 3) {
	case FRAME802154_SHORTADDRMODE:
		memrev(nptr->netpkt_raddst, bptr, 2);
		nptr->netpkt_dstfmt = RAD_SHORT_ADDR;
		nptr->netpkt_dstvalid = TRUE;
		bptr += 2;
		break;

	case FRAME802154_LONGADDRMODE:
		memrev(&nptr->netpkt_raddst, bptr, 8);
		nptr->netpkt_dstfmt = RAD_LONG_ADDR;
		nptr->netpkt_dstvalid = TRUE;
		bptr += 8;
		break;

	default:
		printf("destination address mode unknown\n");
	}

	/* src PAN ID and address may be next; are they in the frame */
	
	if ((mhc1 >> 6) & 3) {
		/* src PAN ID in frame or same as dst PAN ID? */
		if (mhc0 & 0x40) {
			memcpy(&nptr->netpkt_srcpan, &nptr->netpkt_dstpan, 2);
		} else {
			memrev(&nptr->netpkt_srcpan, bptr, 2);
			bptr += 2;
		}
		/* src addr format	*/
		switch ((mhc1 >> 6) & 3) {
		case FRAME802154_SHORTADDRMODE:
			memrev(nptr->netpkt_radsrc, bptr, 2);
			nptr->netpkt_srcfmt = RAD_SHORT_ADDR;
			nptr->netpkt_srcvalid = TRUE;
			bptr += 2;
			break;

		case FRAME802154_LONGADDRMODE:
			memrev(&nptr->netpkt_radsrc, bptr, 8);
			nptr->netpkt_srcfmt = RAD_LONG_ADDR;
			nptr->netpkt_srcvalid = TRUE;
			bptr += 8;
			break;

		default:
			printf("destination address mode unknown\n");
		}
	};


		/* unpack IPv6 header */
	ihc0 = *bptr++;

	if (((ihc0 >> 5) & 7) != 3) {
		printf("unknown dispatch type %d\n", (ihc0 >> 5) & 7);
		freebuf((char *)nptr);
		return;
	}

	ihc1 = *bptr++;

	nptr->netpkt_ip6hdr.net_ipvers = 6;

	/* Traffic class, flow label */
	/*
   TF: Traffic Class, Flow Label:  As specified in [RFC3168], the 8-bit
      IPv6 Traffic Class field is split into two fields: 2-bit Explicit
      Congestion Notification (ECN) and 6-bit Differentiated Services
      Code Point (DSCP).

      00:  ECN + DSCP + 4-bit Pad + Flow Label (4 bytes)

      01:  ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided.

      10:  ECN + DSCP (1 byte), Flow Label is elided.

      11:  Traffic Class and Flow Label are elided.
	*/
	switch ((ihc0 >> 3) & 3) {
	case 0:
	  nptr->netpkt_ip6hdr.net_iptclass = bptr[0];
	  nptr->netpkt_ip6hdr.net_ipflowlabel = (bptr[1] << 16) | (bptr[2] << 8) | bptr[0];
	  bptr += 4;
	  break;

	case 1:
	  nptr->netpkt_ip6hdr.net_iptclass = *bptr++ & 0xE0;
	  nptr->netpkt_ip6hdr.net_ipflowlabel = 
		  ((bptr[0] & 0x3f) << 16) | (bptr[1] << 8) | bptr[2];
	  bptr += 3;
	  break;

	case 2:
	  nptr->netpkt_ip6hdr.net_iptclass = bptr[0];
	  nptr->netpkt_ip6hdr.net_ipflowlabel = 0;
	  break;

	case 3:
	  nptr->netpkt_ip6hdr.net_iptclass = 0;
	  nptr->netpkt_ip6hdr.net_ipflowlabel = 0;
	}


	/*
   NH: Next Header:

      0: Full 8 bits for Next Header are carried in-line.

      1: The Next Header field is compressed and the next header is
         encoded using LOWPAN_NHC, which is discussed in Section 4.1.
	*/
	switch ((ihc0 >> 2) & 1) {
	case 0:
	  nptr->netpkt_ip6hdr.net_ipnxthdr = *bptr++;
	  lowpan_nhc = FALSE;
	  break;

	case 1:
	  /* section 4.1 next header decode */
	  printf("section 4.1 next header compression\n");
	  lowpan_nhc = TRUE;
	  break;
	}

	/*
   HLIM: Hop Limit:

      00:  The Hop Limit field is carried in-line.

      01:  The Hop Limit field is compressed and the hop limit is 1.

      10:  The Hop Limit field is compressed and the hop limit is 64.
      
      11:  The Hop Limit field is compressed and the hop limit is 255.
	*/

	switch (ihc0 & 3) {
	case 0:
	  nptr->netpkt_ip6hdr.net_iphoplim = *bptr++;
	  break;

	case 1:
	  nptr->netpkt_ip6hdr.net_iphoplim = 1;
	  break;

	case 2:
	  nptr->netpkt_ip6hdr.net_iphoplim = 64;
	  break;

	case 3:
	  nptr->netpkt_ip6hdr.net_iphoplim = 255;
	}

	/*
   CID: Context Identifier Extension:

      0: No additional 8-bit Context Identifier Extension is used.  If
         context-based compression is specified in either Source Address
         Compression (SAC) or Destination Address Compression (DAC),
         context 0 is used.

      1: An additional 8-bit Context Identifier Extension field
         immediately follows the Destination Address Mode (DAM) field.
	*/
	cid = (ihc1 >> 7) & 1;
	switch (cid) {
	case 0:
	  cid_extension = 0;
	  break;

	case 1:
	  cid_extension = *bptr++;
	}

	/*
   SAC: Source Address Compression

      0: Source address compression uses stateless compression.

      1: Source address compression uses stateful, context-based
         compression.

   SAM: Source Address Mode:

      If SAC=0:

         00:  128 bits.  The full address is carried in-line.

         01:  64 bits.  The first 64-bits of the address are elided.
            The value of those bits is the link-local prefix padded with
            zeros.  The remaining 64 bits are carried in-line.

         10:  16 bits.  The first 112 bits of the address are elided.
            The value of the first 64 bits is the link-local prefix
            padded with zeros.  The following 64 bits are 0000:00ff:
            fe00:XXXX, where XXXX are the 16 bits carried in-line.

         11:  0 bits.  The address is fully elided.  The first 64 bits
            of the address are the link-local prefix padded with zeros.
            The remaining 64 bits are computed from the encapsulating
            header (e.g., 802.15.4 or IPv6 source address) as specified
            in Section 3.2.2.

      If SAC=1:

         00:  The UNSPECIFIED address, ::

         01:  64 bits.  The address is derived using context information
            and the 64 bits carried in-line.  Bits covered by context
            information are always used.  Any IID bits not covered by
            context information are taken directly from the
            corresponding bits carried in-line.  Any remaining bits are
            zero.

         10:  16 bits.  The address is derived using context information
            and the 16 bits carried in-line.  Bits covered by context
            information are always used.  Any IID bits not covered by
            context information are taken directly from their
            corresponding bits in the 16-bit to IID mapping given by
            0000:00ff:fe00:XXXX, where XXXX are the 16 bits carried in-
            line.  Any remaining bits are zero.

         11:  0 bits.  The address is fully elided and is derived using
            context information and the encapsulating header (e.g.,
            802.15.4 or IPv6 source address).  Bits covered by context
            information are always used.  Any IID bits not covered by
            context information are computed from the encapsulating
            header as specified in Section 3.2.2.  Any remaining bits
            are zero.
	*/

	/* need to add context-based address compression */
	switch (ihc1 >> 4 & 7) {
	case 0: /* SAC == 0, SAM == 0 */
	  memcpy (nptr->netpkt_ip6hdr.net_ipsrc, bptr, 16);
	  bptr += 16;
	  break;

	case 1: /* SAC == 0, SAM == 1 */
	  memcpy (nptr->netpkt_ip6hdr.net_ipsrc, link_local_pref, 8);
	  memcpy (&nptr->netpkt_ip6hdr.net_ipsrc[8], bptr, 8);
	  bptr += 8;
	  break;

	case 2: /* SAC == 0, SAM == 2 */
	  memcpy (nptr->netpkt_ip6hdr.net_ipsrc, link_local_pref, 14);
	  memcpy (&nptr->netpkt_ip6hdr.net_ipsrc[14], bptr, 2);
	  bptr += 2;
	  break;

	case 3: /* SAC == 0, SAM == 3 */
	  /* section 3.2 UID from encapsulating header */
	  /* there's a byte-ordering issue here somewhere */
	  memcpy (nptr->netpkt_ip6hdr.net_ipsrc, link_local_pref, 8);
	  memcpy (&nptr->netpkt_ip6hdr.net_ipsrc[8], nptr->netpkt_radsrc, 8);
	  break;

	case 4: /* SAC == 1, SAM == 0 */
	  memset (nptr->netpkt_ip6hdr.net_ipsrc, 0, 16);
	  break;

	case 5: /* SAC == 1, SAM == 1 */
	  /* Context and 64 bits inline */
	  printf("no context-based address compression\n");
	  bptr += 8;
	  break;

	case 6: /* SAC == 1, SAM == 2 */
	  /* Context and 16 bits inline */
	  printf("no context-based address compression\n");
	  bptr += 2;
	  break;

	case 7: /* SAC == 1, SAM == 3 */
	  /* Context and encapsulating header */
	  printf("no context-based address compression\n");
	  break;
	}

	/* M, DAC, DAM */
	/*
   DAM: Destination Address Mode:

      If M=0 and DAC=0  This case matches SAC=0 but for the destination
         address:

         00:  128 bits.  The full address is carried in-line.

         01:  64 bits.  The first 64-bits of the address are elided.
            The value of those bits is the link-local prefix padded with
            zeros.  The remaining 64 bits are carried in-line.

         10:  16 bits.  The first 112 bits of the address are elided.
            The value of the first 64 bits is the link-local prefix
            padded with zeros.  The following 64 bits are 0000:00ff:
            fe00:XXXX, where XXXX are the 16 bits carried in-line.

         11:  0 bits.  The address is fully elided.  The first 64 bits
            of the address are the link-local prefix padded with zeros.
            The remaining 64 bits are computed from the encapsulating
            header (e.g., 802.15.4 or IPv6 destination address) as
            specified in Section 3.2.2.

      If M=0 and DAC=1:

         00:  Reserved.

         01:  64 bits.  The address is derived using context information
            and the 64 bits carried in-line.  Bits covered by context
            information are always used.  Any IID bits not covered by
            context information are taken directly from the
            corresponding bits carried in-line.  Any remaining bits are
            zero.

         10:  16 bits.  The address is derived using context information
            and the 16 bits carried in-line.  Bits covered by context
            information are always used.  Any IID bits not covered by
            context information are taken directly from their
            corresponding bits in the 16-bit to IID mapping given by
            0000:00ff:fe00:XXXX, where XXXX are the 16 bits carried in-
            line.  Any remaining bits are zero.

         11:  0 bits.  The address is fully elided and is derived using
            context information and the encapsulating header (e.g.
            802.15.4 or IPv6 destination address).  Bits covered by
            context information are always used.  Any IID bits not
            covered by context information are computed from the
            encapsulating header as specified in Section 3.2.2.  Any
            remaining bits are zero.

      If M=1 and DAC=0:

         00:  128 bits.  The full address is carried in-line.

         01:  48 bits.  The address takes the form ffXX::00XX:XXXX:XXXX.

         10:  32 bits.  The address takes the form ffXX::00XX:XXXX.

         11:  8 bits.  The address takes the form ff02::00XX.

      If M=1 and DAC=1:

         00:  48 bits.  This format is designed to match Unicast-Prefix-
            based IPv6 Multicast Addresses as defined in [RFC3306] and
            [RFC3956].  The multicast address takes the form ffXX:XXLL:
            PPPP:PPPP:PPPP:PPPP:XXXX:XXXX. where the X are the nibbles
            that are carried in-line, in the order in which they appear
            in this format.  P denotes nibbles used to encode the prefix
            itself.  L denotes nibbles used to encode the prefix length.
            The prefix information P and L is taken from the specified
            context.

         01:  reserved

         10:  reserved

         11:  reserved
	*/

	switch (ihc1 & 15) {
	case 0: /* M == 0, DAC == 0, DAM == 0; address in line */
	  memcpy(nptr->netpkt_ip6hdr.net_ipdst, bptr, 16);
	  bptr += 16;
	  break;

	case 1: /* M == 0, DAC == 0, DAM == 1; 64 bits in line */
	  memcpy(nptr->netpkt_ip6hdr.net_ipdst, link_local_pref, 8);
	  memcpy(&nptr->netpkt_ip6hdr.net_ipdst[8], bptr, 8);
	  bptr += 8;
	  break;

	case 2: /* M == 0, DAC == 0, DAM == 2; 16 bits in line */
	  memcpy(nptr->netpkt_ip6hdr.net_ipdst, link_local_pref, 14);
	  memcpy(&nptr->netpkt_ip6hdr.net_ipdst[14], bptr, 2);
	  bptr += 2;
	  break;

	case 3: /* M == 0, DAC == 0, DAM == 3; 64 bits from encapsulation */
	  /* section 3.2 UID from encapsulating header */
	  memcpy (nptr->netpkt_ip6hdr.net_ipdst, link_local_pref, 8);
	  memcpy (&nptr->netpkt_ip6hdr.net_ipdst[8], nptr->netpkt_raddst, 8);
	  break;

	case 4: /* M == 0, DAC == 1, DAM == 0; reserved */
	  break;
	  
	case 5: /* M == 0, DAC == 1, DAM == 1; context and 64 bits in line */
	  printf("No context address compression\n");
	  bptr += 8;
	  break;

	case 6: /* M == 0, DAC == 1, DAM == 2; context and 16 bits in line */
	  printf("No context address compression\n");
	  bptr += 2;
	  break;

	case 7: /* M == 0, DAC == 1, DAM == 3; context and encap */
	  printf("No context address compression\n");
	  break;

	case 8: /* M == 1, DAC == 0, DAM == 0; full address in line */
	  memcpy(nptr->netpkt_ip6hdr.net_ipdst, bptr, 16);
	  bptr += 16;
	  break;

	case 9: /* M == 1, DAC == 0, DAM == 1; 48 bits, ffXX::00XX:XXXX:XXXX */
	  memset(nptr->netpkt_ip6hdr.net_ipdst, 0, 16);
	  nptr->netpkt_ip6hdr.net_ipdst[0] = 0xff;
	  nptr->netpkt_ip6hdr.net_ipdst[1] = bptr[0];
	  bptr++;
	  memcpy(&nptr->netpkt_ip6hdr.net_ipdst[11], bptr, 5);
	  bptr += 5;
	  break;

	case 11: /* M == 1, DAC == 0, DAM == 3; 8 bits, ff02::00XX */
	  memset(nptr->netpkt_ip6hdr.net_ipdst, 0, 16);
	  nptr->netpkt_ip6hdr.net_ipdst[0] = 0xff;
	  nptr->netpkt_ip6hdr.net_ipdst[1] = 0x02;
	  nptr->netpkt_ip6hdr.net_ipdst[15] = *bptr++;
	}

	/* Next header decompression if necessary */
	if (lowpan_nhc) {
		printf("lowpan_nhc 0x%02x\n", *bptr);
		nptr->netpkt_ip6hdr.net_ipnxthdr = 17;	/* UDP */
		if ((*bptr & 0xf8) == 0xf0) {
			printf("UDP header compression\n");
			cksum_c = (*bptr >> 2) & 1;
			uptr = (struct udphdr *)(nptr->netpkt_ip6nxthdr);
			switch (*bptr++ & 3) {
			case 0:
				memcpy(&uptr->net_udpsport, bptr, 2);
				memcpy(&uptr->net_udpdport, bptr+2, 2);
				bptr += 4;
				break;

			case 1:
				memcpy(&(uptr->net_udpsport), bptr, 2);
				uptr->net_udpdport = 0xf000 | *(bptr + 2);
				bptr += 3;
				break;

			case 2:
				uptr->net_udpsport = 0xf000 | *bptr++;
				memcpy(&(uptr->net_udpdport), bptr, 2);
				bptr += 2;
				break;

			case 3:
				uptr->net_udpsport = 0xf0b0 | (*bptr >> 4);
				uptr->net_udpdport = 0xf0b0 | (*bptr++ & 15);
			}
			/* now go get the checksum */
			if (cksum_c) {
				/* recompute the checksum */
				printf("recompute UDP checksum\n");
			} else {
				memcpy((char *)&uptr->net_udpcksum, bptr, 2);
				bptr += 2;
			}
			/* compute UDP length from what's left over */
			uptr->net_udplen = length - (bptr - bstrt);
			printf("UDP len %d bstrt 0x%08x bptr 0x%08x udplen %d\n",
			       length, bstrt, bptr, uptr->net_udplen);
			memcpy(uptr->net_udpdata, bptr, uptr->net_udplen);
			nptr->netpkt_ip6hdr.net_ip6len = 
				uptr->net_udplen + 8;
		} else {
			printf("unknown header compression\n");
		}
	} else {
		/* no next header compression; just copy payload */
		nptr->netpkt_ip6hdr.net_ip6len = length - (bptr - bstrt);
		memcpy(nptr->netpkt_ip6nxthdr, bptr, nptr->netpkt_ip6hdr.net_ip6len);
	}
	if_tab[0].if_iqueue[if_tab[0].if_ihead] = nptr;
	if_tab[0].if_ihead = (if_tab[0].if_ihead + 1) % IF_IQUEUESIZE;
	signal(if_tab[0].if_isem);
	/* Longer term:
	 * add fragmentation
	 */

	return;
}

void rad_in(void) {
	int n;
	unsigned char buf[128];

	while (TRUE) {
		n = read(RADIO0, (char *)buf, 128);
		printf("\nread returns %d @ clktime %d\n", n, clktime);
		if ((buf[1] & 3) == 1)	/* data packet */
		  packet_decode(buf, n);
	}
	return;
}
