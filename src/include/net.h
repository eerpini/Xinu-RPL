/* net.h */

#define NETSTK      8192 		/* stack size for network setup */
#define NETPRIO     500     		/* network startup priority 	*/

/* Constants used in the networking code */

#define	ETH_ARP     0x0806		/* Ethernet type for ARP	*/
#define	ETH_IP      0x0800		/* Ethernet type for IP		*/

#define	IP_BCAST    0xffffffff		/* IP local broadcast address	*/
#define	IP_THIS     0xffffffff		/* "this host" src IP address	*/

#define	IP_ICMP	    1			/* ICMP protocol type for IPv4 	*/
#define	IP_ICMPV6	    58			/* ICMP protocol type for IPv6 	*/
#define	IP_UDP      17			/* UDP protocol type for IP 	*/
#define IP_TCP      6                   /* TCP protocol type for IP     */

#define	IP_ASIZE    4			/* bytes in an IP address	*/
#define	IP_HDR_LEN  20			/* bytes in an IP header	*/
#define IP_VH 	    0x45 		/* IP version and hdr length 	*/

/* Format of an Ethernet packet carrying IPv4 and UDP */

#ifndef ETH_HDR_LEN
        #define ETH_HDR_LEN     14
#endif

#pragma pack(2)
struct	netpacket	{
	byte	net_ethdst[ETH_ADDR_LEN];/* Ethernet dest. MAC address	*/
	byte	net_ethsrc[ETH_ADDR_LEN];/* Ethernet source MAC address	*/
	uint16	net_ethtype;		/* Ethernet type field		*/
	byte	net_ipvh;		/* IP version and hdr length	*/
	byte	net_iptos;		/* IP type of service		*/
	uint16	net_iplen;		/* IP total packet length	*/
	uint16	net_ipid;		/* IP datagram ID		*/
	uint16	net_ipfrag;		/* IP flags & fragment offset	*/
	byte	net_ipttl;		/* IP time-to-live		*/
	byte	net_ipproto;		/* IP protocol (actually type)	*/
	uint16	net_ipcksum;		/* IP checksum			*/
	uint32	net_ipsrc;		/* IP source address		*/
	uint32	net_ipdst;		/* IP destination address	*/
	union {
	 struct {
	  uint16 	net_udpsport;	/* UDP source protocol port	*/
	  uint16	net_udpdport;	/* UDP destination protocol port*/
	  uint16	net_udplen;	/* UDP total length		*/
	  uint16	net_udpcksum;	/* UDP checksum			*/
	  byte		net_udpdata[1500-42];/* UDP payload (1500-above) */
	 };
	 struct {
	  byte		net_ictype;	/* ICMP message type		*/
	  byte		net_iccode;	/* ICMP code field (0 for ping)	*/
	  uint16	net_iccksum;	/* ICMP message checksum	*/
	  uint16	net_icident; 	/* ICMP identifier		*/
	  uint16	net_icseq;	/* ICMP sequence number		*/
	  byte		net_icdata[1500-42];/* ICMP payload (1500-above)*/
	 };
	};
};
#pragma pack()

extern	struct	eth_packet *currpkt;	/* ptr to current input packet	*/
extern	bpid32	netbufpool;		/* ID of net packet buffer pool	*/

struct	network {
	uint32	ipaddr;			/* IP address			*/
	uint32	addrmask;		/* Subnet mask			*/
	uint32	routeraddr;		/* Address of default router	*/
	bool8	ipvalid;		/* Is IP address valid yet?	*/
	byte	ethaddr[ETH_ADDR_LEN];	/* Ethernet address		*/
};

extern	struct	network NetData;	/* Local network interface	*/

#pragma pack(2)
/* Separate structures for each data packet type */

struct eth_packet        {
	byte	net_ethdst[ETH_ADDR_LEN];/* Ethernet dest. MAC address	*/
	byte	net_ethsrc[ETH_ADDR_LEN];/* Ethernet source MAC address	*/
	uint16	net_ethtype;		/* Ethernet type field		*/
        byte    net_ethdata[1500-ETH_HDR_LEN];   /* Ethernet payload (1500-above)*/
};
#pragma pack()

#ifndef IPV4_HDR_LEN
        #define IPV4_HDR_LEN    20
#endif

#pragma pack(2)
struct ipv4_packet {
 	byte	net_ipvh;		/* IP version and hdr length	*/
	byte	net_iptos;		/* IP type of service		*/
	uint16	net_iplen;		/* IP total packet length	*/
	uint16	net_ipid;		/* IP datagram ID		*/
	uint16	net_ipfrag;		/* IP flags & fragment offset	*/
	byte	net_ipttl;		/* IP time-to-live		*/
	byte	net_ipproto;		/* IP protocol (actually type)	*/
	uint16	net_ipcksum;		/* IP checksum			*/
	uint32	net_ipsrc;		/* IP source address		*/
	uint32	net_ipdst;		/* IP destination address	*/
        byte    net_ipdata[1500-ETH_HDR_LEN-IPV4_HDR_LEN];  /* IP payload (1500-ethernet_metadata-above)    */
};
#pragma pack()
#ifndef IPV6_ADDR_LEN
        #define IPV6_ADDR_LEN 16        /* IPv6 Address lenght , 16 bytes       */
#endif
#ifndef IPV6_HDR_LEN
        #define IPV6_HDR_LEN    40
#endif

#pragma pack(2)
struct  ipv6_packet      {
        uint32  net_ip_vtclassflabel;            /* IPv6 version(4), transport class(8) and Flow Label (20)             */
        uint16  net_ip_payload_length;          /* IPv6 Payload length          */
        byte    net_ip_nextheader;              /* IPv6 Next Header (Optional)  */
        byte    net_ip_hoplimit;                /* IPv6 Hop Limit               */
        byte    net_ip_v6src[IPV6_ADDR_LEN]; /* IPv6 Source Address          */
        byte    net_ip_v6dst[IPV6_ADDR_LEN];   /* IPv6 Destination address     */
        byte    net_ipdata[1500-ETH_HDR_LEN-IPV6_HDR_LEN];          /* IPv6 Payload (1500-ethernet_metadata-above)  */
};
#pragma pack()

#ifndef UDP_HDR_LEN
        #define UDP_HDR_LEN     8
#endif

#pragma pack(2)
struct  udp_packet       {
        uint16 	net_udpsport;	/* UDP source protocol port	*/
        uint16	net_udpdport;	/* UDP destination protocol port*/
        uint16	net_udplen;	/* UDP total length		*/
        uint16	net_udpcksum;	/* UDP checksum			*/
        byte		net_udpdata[1500-ETH_HDR_LEN-IPV4_HDR_LEN-UDP_HDR_LEN];/* UDP payload (1500-above) */
};
#pragma pack()

#pragma pack(2)
struct  udp_ipv6_packet       {
        uint16 	net_udpsport;	/* UDP source protocol port	*/
        uint16	net_udpdport;	/* UDP destination protocol port*/
        uint16	net_udplen;	/* UDP total length		*/
        uint16	net_udpcksum;	/* UDP checksum			*/
        byte		net_udpdata[1500-ETH_HDR_LEN-IPV6_HDR_LEN-UDP_HDR_LEN];/* UDP payload (1500-above) */
};
#pragma pack()

#pragma pack(2)
struct tcp_packet       {
       uint16   net_tcpsport;   /* TCP source protocol port     */
       uint16   net_tcpdport;   /* TCP destination protocol port*/
       uint32   net_tcpseq;     /* TCP sequence number          */
       uint32   net_tcpacknum;  /* TCP acknowledgement number   */
       uint16   net_tcpbits;    /* Bit Fields : Data OFfset (4), Reserved(3), ECN(3), Control Bits(6)   */
       uint16   net_tcpwin;     /* TCP Window                   */
       uint16   net_tcpcsum;    /* TCP Checksum                 */
       uint16   net_tcpurgp;    /* TCP Urgent Pointer           */
       /* Options : 40 Bytes, Variable length*/
       /* Data */
};
#pragma pack()

#pragma pack(2)
struct  tcp_ipv6_packet {
       uint16   net_tcpsport;   /* TCP source protocol port     */
       uint16   net_tcpdport;   /* TCP destination protocol port*/
       uint32   net_tcpseq;     /* TCP sequence number          */
       uint32   net_tcpacknum;  /* TCP acknowledgement number   */
       uint16   net_tcpbits;    /* Bit Fields : Data OFfset (4), Reserved(3), ECN(3), Control Bits(6)   */
       uint16   net_tcpwin;     /* TCP Window                   */
       uint16   net_tcpcsum;    /* TCP Checksum                 */
       uint16   net_tcpurgp;    /* TCP Urgent Pointer           */
       /* Options : 40 Bytes, Variable length*/
       /* Data */
};
#pragma pack(2)

#ifndef ICMP_HDR_LEN
        #define ICMP_HDR_LEN     8
#endif
#ifndef ICMPV6_HDR_LEN
        #define ICMPV6_HDR_LEN     4
#endif

#pragma pack(2)
struct icmp_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        uint16	        net_icident; 	/* ICMP identifier		*/
        uint16	        net_icseq;	/* ICMP sequence number		*/
        byte		net_icdata[1500-ETH_HDR_LEN-IPV4_HDR_LEN-ICMP_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()

#pragma pack(2)
struct icmp_ipv6_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        uint16	        net_icident; 	/* ICMP identifier		*/
        uint16	        net_icseq;	/* ICMP sequence number		*/
        byte		net_icdata[1500-ETH_HDR_LEN-IPV4_HDR_LEN-ICMP_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()
#pragma pack(2)
struct icmpv6_ipv6_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        byte		net_icdata[1500-ETH_HDR_LEN-IPV6_HDR_LEN-ICMPV6_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()


#define	ARP_HALEN	6		/* size of Ethernet MAC address	*/
#define	ARP_PALEN	4		/* size of IP address		*/

#pragma pack(2)
struct	arp_packet {			/* ARP packet for IP & Ethernet	*/
	uint16	arp_htype;		/* ARP hardware type		*/
	uint16	arp_ptype;		/* ARP protocol type		*/
	byte	arp_hlen;		/* ARP hardware address length	*/
	byte	arp_plen;		/* ARP protocol address length	*/
	uint16	arp_op;			/* ARP operation		*/
	byte	arp_sndha[ARP_HALEN];	/* ARP sender's Ethernet addr 	*/
	uint32	arp_sndpa;		/* ARP sender's IP address	*/
	byte	arp_tarha[ARP_HALEN];	/* ARP target's Ethernet addr	*/
	uint32	arp_tarpa;		/* ARP target's IP address	*/
};
#pragma pack()

#pragma pack(2)
/* Copied from dhcp.h */
struct	dhcp_packet{
	byte	dc_bop;			/* DHCP bootp op 1=req 2=reply	*/
	byte	dc_htype;		/* DHCP hardware type		*/
	byte	dc_hlen;		/* DHCP hardware address length	*/
	byte	dc_hops;		/* DHCP hop count		*/
	uint32	dc_xid;			/* DHCP xid			*/
	uint16	dc_secs;		/* DHCP seconds			*/
	uint16	dc_flags;		/* DHCP flags			*/
	uint32	dc_cip;			/* DHCP client IP address	*/
	uint32	dc_yip;			/* DHCP your IP address		*/
	uint32	dc_sip;			/* DHCP server IP address	*/
	uint32	dc_gip;			/* DHCP gateway IP address	*/
	byte	dc_chaddr[16];		/* DHCP client hardware address	*/
	byte	dc_bootp[192];		/* DHCP bootp area (zero)	*/
	uint32	dc_cookie;		/* DHCP cookie			*/
	byte	dc_opt[1024];		/* DHCP options area (large	*/
					/*  enough to hold more than	*/
					/*  reasonable options		*/
};
#pragma pack()



