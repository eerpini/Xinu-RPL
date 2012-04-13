#include <xinu.h>
/*
 * Utilities for sending and receiving rpl packets to the forwarding machine
 */

extern struct rpl_sim_packet sim_queue[RPL_SIM_RECV_QUEUE_LEN];
extern sid32  rpl_sim_read_sem;
extern sid32  rpl_sim_write_sem;

status rpl_sim_init(){

        //int i = 0;
        rpl_sim_read_sem  = semcreate(0);
        rpl_sim_write_sem = semcreate(RPL_SIM_RECV_QUEUE_LEN);
        return OK;
}

status rpl_send(char * node_phy_addr, char *src_node, byte msg_type, char *msg, uint32 msglen){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;
        uint32 remip;
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};
        dot2ip(RPL_FORWARDING_GATEWAY, &remip);

        if ( ! NetData.ipvalid){
                getlocalip();
        }
        if(msglen > 1500-ETH_HDR_LEN-RPL_SIM_HDR_LEN){
                kprintf("Simulator : Message too big \r\n");
                return SYSERR;
        }
 
	memcpy(pkt.net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        kprintf("Copied ethernet source\r\n");
        /* FIXME : Needs to be changed to something that is valid */
        pkt.net_ethtype = 0x1000;	
	if (remip == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(pkt.net_ethdst, ethbcast, ETH_ADDR_LEN);

	/* If destination isn't on the same subnet, send to router	*/

	} else if ((NetData.ipaddr & NetData.addrmask)
			!= (remip & NetData.addrmask)) {
		if (arp_resolve(NetData.routeraddr, pkt.net_ethdst)!=OK) {
		    kprintf("udp_send: cannot resolve router %08x\n\r",
				NetData.routeraddr);
		    return SYSERR;
		}
	} else {
		/* Destination is on local subnet -  get MAC address */
		if (arp_resolve(remip, pkt.net_ethdst) != OK) {
			kprintf("udp_send: cannot resolve %08x\n\r",remip);
			return SYSERR;
		}
	}

        rpl_sim_pkt = (struct rpl_sim_packet *)pkt.net_ethdata;
        memcpy(rpl_sim_pkt->dest_node, node_phy_addr, RPL_NODE_PHY_ADDR_LEN);
        kprintf("Copied physical node address : %04x\r\n", rpl_sim_pkt->dest_node);
        /*
         * FIXME Change this to my_phsical_address which is 64 bits
         */
        memcpy(rpl_sim_pkt->src_node, (char *)(src_node), RPL_NODE_PHY_ADDR_LEN);
        kprintf("Copied source physical node address : %04x\r\n", rpl_sim_pkt->src_node);
        rpl_sim_pkt->msg_type = msg_type;
        rpl_sim_pkt->msg_len = msglen;
        memcpy(rpl_sim_pkt->data, msg, msglen);
        kprintf("Copied the payload to rpl_sim_pkt message\r\n");

        /*
         * FIXME : Perform host to network order translations
         */

        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msglen); 

        return OK;


}

status rpl_send_with_ip(char * node_phy_addr, char *src_node, byte msg_type, char *msg, uint32 msglen, uint32 remip){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

        if ( ! NetData.ipvalid){
                getlocalip();
        }
 
	memcpy(pkt.net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        /* FIXME : Needs to be changed to something that is valid */
        pkt.net_ethtype = 0x1000;	
	if (remip == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(pkt.net_ethdst, ethbcast, ETH_ADDR_LEN);

	/* If destination isn't on the same subnet, send to router	*/

	} else if ((NetData.ipaddr & NetData.addrmask)
			!= (remip & NetData.addrmask)) {
		if (arp_resolve(NetData.routeraddr, pkt.net_ethdst)!=OK) {
		    kprintf("udp_send: cannot resolve router %08x\n\r",
				NetData.routeraddr);
		    return SYSERR;
		}
	} else {
		/* Destination is on local subnet -  get MAC address */
		if (arp_resolve(remip, pkt.net_ethdst) != OK) {
			kprintf("udp_send: cannot resolve %08x\n\r",remip);
			return SYSERR;
		}
	}

        rpl_sim_pkt = (struct rpl_sim_packet *)pkt.net_ethdata;
        memcpy(rpl_sim_pkt->dest_node, node_phy_addr, RPL_NODE_PHY_ADDR_LEN);
        /*
         * FIXME Change this to my_phsical_address which is 64 bits
         */
        memcpy(rpl_sim_pkt->src_node, (char *)(src_node), RPL_NODE_PHY_ADDR_LEN);
        rpl_sim_pkt->msg_type = msg_type;
        memcpy(rpl_sim_pkt->data, msg, msglen);

        /*
         * FIXME : Perform host to network order translations
         */

        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msglen); 

        return OK;


}

status rpl_receive(){

        int i = 0;
        struct rpl_sim_packet * pkt = NULL;
        uint32 remip = 0xffffffff;
        while(1){
                remip = 0xffffffff;
                wait(rpl_sim_read_sem);
                kprintf("Resuming in rpl_receive\r\n");
                pkt = &sim_queue[i];
                
                /*
                 * Lookup map and see if we have a mapping
                 * Send the packet using the ip stack 
                 * Use the rpl_sim_packet header as such
                 */
                dot2ip(RPL_FORWARDING_GATEWAY, &remip);
                if(remip == NetData.ipaddr){
                        kprintf("Simulator working with message type : %d\r\n",pkt->msg_type);
                        remip = (uint32) (pkt->dest_node);
                        if(remip == 0xffffffff || remip == 0x00000000){
                                kprintf("Could not find a mapping for the given 64bit physical address\r\n");
                                return SYSERR;
                        }
                        else{
                                rpl_send_with_ip((char *)pkt->dest_node, (char *)pkt->src_node, pkt->msg_type, (char *)pkt->data, pkt->msg_len, remip);
                        }
                }
                else{
                        struct icmpv6_sim_packet rpkt;
                        switch(pkt->msg_type){
                                case RPL_DIS_MSGTYPE:
                                        /*
                                         */
                                        kprintf("DIS Received\r\n");
                                        decodedis((struct icmpv6_sim_packet *)(pkt->data));
                                        encodedio(&rpkt);
                                        rpl_send((char *)(pkt->src_node), (char *)(NetData.ipaddr), RPL_DIO_MSGTYPE, 
                                                        (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);

                                        break;
                                case RPL_DIO_MSGTYPE:
                                        kprintf("DIO Received\r\n");
                                        processdio((struct icmpv6_sim_packet *)(pkt->data));
                                        encodedao(&rpkt);
                                        rpl_send((char *)(RPL_MYINFO.parent), (char *)(NetData.ipaddr), RPL_DIO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);

                                        break;
                                case RPL_DAO_MSGTYPE:
                                        kprintf("DAO Received\r\n");
#ifdef LOWPAN_BORDER_ROUTER
                                        processdao((struct icmpv6_sim_packet *)(pkt->data));
#endif
#ifdef LOWPAN_NODE
                                        rpl_send((char *)(RPL_MYINFO.parent), (char *)(NetData.ipaddr), RPL_DIO_MSGTYPE, (char *)(&pkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
#endif


                                        break;
                                default:
                                        kprintf("Received an unknown RPL message type\r\n");
                                        break;
                        }

                }
                signal(rpl_sim_write_sem);
                i = (i+1)%RPL_SIM_RECV_QUEUE_LEN;
        }

        return OK;
}
