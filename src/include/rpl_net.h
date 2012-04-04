#ifndef RPL_FORWARDING_GATEWAY
        #define RPL_FORWARDING_GATEWAY
#endif


#ifndef ETH_HDR_LEN
        #define ETH_HDR_LEN     14
#endif

#ifndef ICMP_HDR_LEN
        #define ICMP_HDR_LEN     8
#endif
#ifndef ICMPV6_HDR_LEN
        #define ICMPV6_HDR_LEN     4
#endif

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


/*      ICMPV4 Packet Structure */
#pragma pack(2)
struct icmpv4_packet{
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        t; 	/* ICMP identifier		*/
        uint16	        net_icseq;	/* ICMP sequence number		*/
        byte		net_icdata[1500-ETH_HDR_LEN-IPV4_HDR_LEN-ICMP_HDR_LEN];/* ICMP payload (1500-above)*/
};

/*      ICMPV6 Packet Structure */
#pragma pack(2)
struct icmpv6_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        byte		net_icdata[1500-ETH_HDR_LEN-IPV6_HDR_LEN-ICMPV6_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()

#pragma pack(2)
struct icmpv6_sim_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        byte		net_icdata[1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()


/*
 * All these definitions are purely for simulation purposes and will not be used later on
 */


#ifndef RPL_SIM_HDR_LEN
        #define RPL_SIM_HDR_LEN 13
#endif


#ifndef RPL_NODE_PHY_ADDR_LEN
        #define RPL_NODE_PHY_ADDR_LEN 8
#endif

#ifndef RPL_SIM_RECV_QUEUE_LEN
        #define RPL_SIM_RECV_QUEUE_LEN 200
#endif

#ifndef RPL_SIM_NODES 
        #define RPL_SIM_NODES 10
#endif

struct  rpl_sim_packet{
        byte dest_node[RPL_NODE_PHY_ADDR_LEN];
        byte   msg_type;
        uint32 msg_len;
        byte   data[1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN];
};


struct rpl_sim_packet sim_queue[RPL_SIM_RECV_QUEUE_LEN];
sid32  rpl_sim_read_sem;
sid32  rpl_sim_write_sem;



