/* net.h */

#define NETSTK		8192 		/* stack size for network setup */
#define NETPRIO		500    		/* network startup priority 	*/

/* Constants used in the networking code */

/* Format of a radio buffer carrying 802.15.4 addresses, IPv6, UDP/ICMP */

struct	netpacket	{
	struct {
#define RAD_SHORT_ADDR 1
#define RAD_LONG_ADDR  0
		byte netpkt_dstfmt:1;	/* destination address 16/64 bits    */
		byte netpkt_srcfmt:1;	/* source address 16/64 bits         */
		byte netpkt_dstvalid:1;	/* destination address valid         */
		byte netpkt_srcvalid:1;	/* source address valid	             */
	};
	byte	netpkt_lqi;		/* link quality index for this frame */
	byte	netpkt_raddst[RAD_ADDR_LEN];/* Radio dest. MAC address	     */
	byte	netpkt_radsrc[RAD_ADDR_LEN];/* Radio source MAC address      */
	uint8	netpkt_dstpan[2];	/* destination PAN ID                */
	uint8	netpkt_srcpan[2];	/* source PAN ID                     */

	/* IPv6 header */
	struct ip6hdr	netpkt_ip6hdr;
 
	/* next header starts here */
	byte	netpkt_ip6nxthdr[1360 - sizeof(struct ip6hdr)];
};

#define	PACKLEN	sizeof(struct netpacket)

extern	uint8	link_local_prefix[14];

