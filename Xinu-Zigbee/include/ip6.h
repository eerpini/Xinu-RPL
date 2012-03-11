/* ip6.h  -  constants related to IPv6 */
#define ICMPV6_ECHOREQUEST		128
#define ICMPV6_ECHOREPLY		129
#define ICMPV6_ROUTERSOLICITATION	133	
#define ICMPV6_NEIGHBORSOLICITATION	135
#define ICMPV6_RPLCONTROL		155
#define RPL_DODAGINFOSOLICITATION	0x00
#define RPL_DIO				0x01
#define RPL_DAO				0x02
#define RPL_DAOACK			0x03
#define RPL_SECUREDODAGINFOSOLICITATION	0x80
#define RPL_SECUREDIO			0x81
#define RPL_SECUREDAO			0x82
#define RPL_SECUREDAOACK		0x83
#define RPL_CONSISTENCYCHECK		0x8a

typedef	uint8	IP6addr[16];

struct	ip6hdr {
	struct {
		uint32	net_ipvers:4;
		uint32	net_iptclass:8;
		uint32	net_ipflowlabel:20;
	};
 	uint16	net_ip6len;
	uint8	net_ipnxthdr;
	uint8	net_iphoplim;
	uint8	net_ipsrc[16];
	uint8	net_ipdst[16];
};

struct	udphdr	{
	uint16 	net_udpsport;	/* UDP source protocol port	*/
	uint16 	net_udpdport;	/* UDP destination protocol port*/
	uint16	net_udplen;	/* UDP total length		*/
	uint16	net_udpcksum;	/* UDP checksum			*/
	byte	net_udpdata[1];	/* UDP payload (1280-above) */
};

struct	icmphdr	{
	byte	net_ictype;	/* ICMP message type		*/
	byte	net_iccode;	/* ICMP code field (0 for ping)	*/
	uint16	net_iccksum;	/* ICMP message checksum	*/
	uint16	net_icident; 	/* ICMP identifier		*/
	uint16	net_icseq;	/* ICMP sequence number		*/
	byte	net_icdata[1];/* ICMP payload (1500-above)*/
};

#define	PROTO_UDP	17
#define	PROTO_ICMPV6	58


