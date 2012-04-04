/*
 * Utilities for sending and receiving rpl packets to the forwarding machine
 */

extern struct rpl_sim_recv_queue sim_queue[RPL_SIM_RECV_QUEUE_LEN];
extern sid32  rpl_sim_read_sem;
extern sid32  rpl_sim_write_sem;

status rpl_init(){

        int i = 0;
        rpl_sim_read_sem  = semcreate(0);
        rpl_sim_write_sem = semcreate(RPL_SIM_RECV_QUEUE_LEN);
        return OK;
}

status rpl_send(char * node_phy_addr, byte msg_type, char *msg, uint32 msglen){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;
        uint32 remip = RPL_FORWARDING_GATEWAY;

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
        rpl_sim_pkt->dest_node = node_phy_addr;
        rpl_sim_pkt->msg_type = msg_type;
        rpl_sim_pkt->msg_len = msg_len;
        memcpy(rpl_sim_pkt->data, msg, msglen);

        /*
         * FIXME : Perform host to network order translations
         */

        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msglen); 

        return OK;


}

status rpl_send_with_ip(char * node_phy_addr, byte msg_type, char *msg, uint32 msglen, utin32 remip){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;

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
        rpl_sim_pkt->dest_node = node_phy_addr;
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
        struct rpl_sim_rcv_queue * pkt = NULL;
        while(1){
                wait(rpl_sim_read_sem);
                pkt = &sim_queue[i];
                uint32 remip = 0xffffffff;
                
                /*
                 * Lookup map and see if we have a mapping
                 * Send the packet using the ip stack 
                 * Use the rpl_sim_packet header as such
                 */

                remip = (uint32) (pkt->dest_node & 0x00000000ffffffff);
                if(remip == 0xffffffff){
                        kprintf("Could not find a mapping for the given 64bit physical address\r\n");
                        return SYSERR;
                }
                else{
                        rpl_send_with_ip(pkt->dest_node, pkt->msg_type, pkt->data, pkt->msg_len, remip);
                }
                signal(rpl_sim_write_sem);
                i = (i+1)%RPL_SIM_REC_QUEUE_LEN;
        }

        return OK;
}


        

        


}
