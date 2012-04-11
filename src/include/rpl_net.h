#ifndef RPL_FORWARDING_GATEWAY
        #define RPL_FORWARDING_GATEWAY "128.10.133.127"
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
#ifndef IPV6_HDR_LEN
        #define IPV6_HDR_LEN    40
#endif

#ifndef RPL_SIM_HDR_LEN
        #define RPL_SIM_HDR_LEN 13
#endif


#pragma pack(2)
struct icmpv6_sim_packet       {
        byte		net_ictype;	/* ICMP message type		*/
        byte		net_iccode;	/* ICMP code field (0 for ping)	*/
        uint16	        net_iccksum;	/* ICMP message checksum	*/
        byte		net_icdata[1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN - ICMPV6_HDR_LEN];/* ICMP payload (1500-above)*/
};
#pragma pack()


/*
 * All these definitions are purely for simulation purposes and will not be used later on
 */



#ifndef RPL_NODE_PHY_ADDR_LEN
        #define RPL_NODE_PHY_ADDR_LEN 4
#endif

#ifndef RPL_SIM_RECV_QUEUE_LEN
        #define RPL_SIM_RECV_QUEUE_LEN 200
#endif

#ifndef RPL_SIM_NODES 
        #define RPL_SIM_NODES 10
#endif

struct  rpl_sim_packet{
        byte dest_node[RPL_NODE_PHY_ADDR_LEN];
        byte src_node[RPL_NODE_PHY_ADDR_LEN];
        byte   msg_type;
        uint32 msg_len;
        byte   data[1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN];
};


struct rpl_sim_packet sim_queue[RPL_SIM_RECV_QUEUE_LEN];
sid32  rpl_sim_read_sem;
sid32  rpl_sim_write_sem;



