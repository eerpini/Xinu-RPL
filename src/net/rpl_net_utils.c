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

status rpl_send(char * node_phy_addr, char *src_node, byte msg_type, char *msg, uint32 msg_len){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;
        uint32 remip;
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};
        dot2ip(RPL_FORWARDING_GATEWAY, &remip);

        kprintf("Simulator is : %04x dest : %04x source : %04x my addr : %04x\r\n", remip, *((uint32*)node_phy_addr), *((uint32*)src_node), NetData.ipaddr);

        if ( ! NetData.ipvalid){
                getlocalip();
        }
        if(msg_len > 1500-ETH_HDR_LEN-RPL_SIM_HDR_LEN){
                kprintf("Simulator : Message too big \r\n");
                return SYSERR;
        }
 
	memcpy(pkt.net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        //kprintf("Copied ethernet source : %06x\r\n", *(pkt.net_ethsrc+4));
        /* FIXME : Needs to be changed to something that is valid */
        pkt.net_ethtype = 0x1000;	
        //kprintf("The eth type in current packet is : %02x\r\n", pkt.net_ethtype);
	if (remip == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(pkt.net_ethdst, ethbcast, ETH_ADDR_LEN);

	/* If destination isn't on the same subnet, send to router	*/

	} else if ((NetData.ipaddr & NetData.addrmask)
			!= (remip & NetData.addrmask)) {
		if (arp_resolve(NetData.routeraddr, pkt.net_ethdst)!=OK) {
		    kprintf("rpl_send: cannot resolve router %08x\n\r",
				NetData.routeraddr);
		    return SYSERR;
		}
	} else {
		/* Destination is on local subnet -  get MAC address */
		if (arp_resolve(remip, pkt.net_ethdst) != OK) {
			kprintf("rpl_send: cannot resolve %08x\n\r",remip);
			return SYSERR;
		}
	}

        rpl_sim_pkt = (struct rpl_sim_packet *)pkt.net_ethdata;
        //kprintf("Copied physical node address before from [%04x] to [%04x]\r\n",*node_phy_addr, *(rpl_sim_pkt->dest_node));
        memcpy((char *)rpl_sim_pkt->dest_node, (char *)node_phy_addr, RPL_NODE_PHY_ADDR_LEN);
        //kprintf("Copied physical node address from [%04x] to [%04x]\r\n",*((uint32 *)node_phy_addr), *((uint32 *)(rpl_sim_pkt->dest_node)));
        /*
         * FIXME Change this to my_phsical_address which is 64 bits
         */
        memcpy((char *)rpl_sim_pkt->src_node, (char *)(src_node), RPL_NODE_PHY_ADDR_LEN);
        //kprintf("Copied source physical node address from [%04x] to [%04x]\r\n", *((uint32 *)src_node), *((uint32 *)(rpl_sim_pkt->src_node)));
        rpl_sim_pkt->msg_type = msg_type;
        rpl_sim_pkt->msg_len = msg_len;
        memcpy(rpl_sim_pkt->data, msg, msg_len);
        //kprintf("Copied the payload to rpl_sim_pkt message\r\n");

        /*
         * FIXME : Perform host to network order translations
         */

        kprintf("The packet is destined for : %06x with length : %d\r\n", *(pkt.net_ethdst+4), msg_len);

        eth_hton(&pkt);

        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msg_len); 

        return OK;


}

status rpl_send_with_ip(char * node_phy_addr, char *src_node, byte msg_type, char *msg, uint32 msg_len, uint32 remip){

        struct eth_packet pkt;
        struct rpl_sim_packet *rpl_sim_pkt  = NULL;
	byte	ethbcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};
        kprintf("Simulator is : %04x dest : %04x source : %04x my addr : %04x\r\n", remip, *((uint32*)node_phy_addr), *((uint32*)src_node), NetData.ipaddr);

        if ( ! NetData.ipvalid){
                getlocalip();
        }
        if(msg_len > 1500-ETH_HDR_LEN-RPL_SIM_HDR_LEN){
                kprintf("Simulator : Message too big \r\n");
                return SYSERR;
        }
 
	memcpy(pkt.net_ethsrc, NetData.ethaddr, ETH_ADDR_LEN);
        //kprintf("Copied ethernet source : %06x\r\n", *(pkt.net_ethsrc+4));
        /* FIXME : Needs to be changed to something that is valid */
        pkt.net_ethtype = 0x1000;	
        //kprintf("The eth type in current packet is : %02x\r\n", pkt.net_ethtype);
	if (remip == IP_BCAST) {	/* set mac address to b-cast	*/
		memcpy(pkt.net_ethdst, ethbcast, ETH_ADDR_LEN);

	/* If destination isn't on the same subnet, send to router	*/

	} else if ((NetData.ipaddr & NetData.addrmask)
			!= (remip & NetData.addrmask)) {
		if (arp_resolve(NetData.routeraddr, pkt.net_ethdst)!=OK) {
		    kprintf("rpl_send: cannot resolve router %08x\n\r",
				NetData.routeraddr);
		    return SYSERR;
		}
	} else {
		/* Destination is on local subnet -  get MAC address */
		if (arp_resolve(remip, pkt.net_ethdst) != OK) {
			kprintf("rpl_send: cannot resolve %08x\n\r",remip);
			return SYSERR;
		}
	}

        rpl_sim_pkt = (struct rpl_sim_packet *)pkt.net_ethdata;
        //kprintf("Copied physical node address before from [%04x] to [%04x]\r\n",*node_phy_addr, *(rpl_sim_pkt->dest_node));
        memcpy((char *)rpl_sim_pkt->dest_node, (char *)node_phy_addr, RPL_NODE_PHY_ADDR_LEN);
        //kprintf("Copied physical node address from [%04x] to [%04x]\r\n",*((uint32 *)node_phy_addr), *((uint32 *)(rpl_sim_pkt->dest_node)));
        /*
         * FIXME Change this to my_phsical_address which is 64 bits
         */
        memcpy((char *)rpl_sim_pkt->src_node, (char *)(src_node), RPL_NODE_PHY_ADDR_LEN);
        //kprintf("Copied source physical node address from [%04x] to [%04x]\r\n", *((uint32 *)src_node), *((uint32 *)(rpl_sim_pkt->src_node)));
        rpl_sim_pkt->msg_type = msg_type;
        rpl_sim_pkt->msg_len = msg_len;
        memcpy(rpl_sim_pkt->data, msg, msg_len);
        //kprintf("Copied the payload to rpl_sim_pkt message\r\n");

        /*
         * FIXME : Perform host to network order translations
         */

        kprintf("The packet is destined for : %06x with length : %d\r\n", *(pkt.net_ethdst+4), msg_len);

        eth_hton(&pkt);

        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msg_len); 

        return OK;


}
status rpl_send_with_ip_another(char * node_phy_addr, char *src_node, byte msg_type, char *msg, uint32 msg_len, uint32 remip){

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
        rpl_sim_pkt->msg_len= msg_len;
        memcpy(rpl_sim_pkt->data, msg, msg_len);

        /*
         * FIXME : Perform host to network order translations
         */
        kprintf("The packet is destined for : %06x with length : %d\r\n", *(pkt.net_ethdst+4), msg_len);

        eth_hton(&pkt);
        write(ETHER0, (char *)&pkt, ETH_HDR_LEN + RPL_SIM_HDR_LEN + msg_len); 

        return OK;


}

status rpl_receive(){

        int j = 19990;
        int i = 0;
        struct rpl_sim_packet * pkt = NULL;
        uint32 remip ;
        while(1){
                remip = 0xffffffff;
                wait(rpl_sim_read_sem);
                kprintf("Resuming in rpl_receive\r\n");
                kprintf("After resuming in rpl_receive i is [%d] j is [%d]\r\n", i, j);
                pkt = &sim_queue[i];
                
                /*
                 * Lookup map and see if we have a mapping
                 * Send the packet
                 * Use the rpl_sim_packet header as such
                 */
                dot2ip(RPL_FORWARDING_GATEWAY, &remip);
                if(remip == NetData.ipaddr){
                        kprintf("Simulator working with iter [%d] and j [%d] message type : %d dest : %04x source : %04x\r\n",i,j, pkt->msg_type, *((uint32 *)(pkt->dest_node)), *((uint32 *)(pkt->src_node)));
                        remip = *((uint32 *)(pkt->dest_node));
                        if(remip == 0xffffffff || remip == 0x00000000){
                                kprintf("Could not find a mapping for the given 64bit physical address\r\n");
                                //return SYSERR;
                        }
                        else{
                                kprintf("Forwarding rpl message to : [%04x] and i is : %d\r\n", remip, i);
                                rpl_send_with_ip((char *)pkt->dest_node, (char *)pkt->src_node, pkt->msg_type, (char *)pkt->data, pkt->msg_len, remip);
                                //kprintf("The value of i after rpl_send_with_ip is : %d\r\n", i);
                        }
                }
                else{
                        struct icmpv6_sim_packet rpkt;
                        byte parent_changed = 0;
                        switch(pkt->msg_type){
                                case RPL_DIS_MSGTYPE:
                                        kprintf("DIS Received\r\n");
#ifdef LOWPAN_NODE
                                        if(RPL_MYINFO.parent_index < 0){
                                                kprintf("WARN : Ignoring DIS Message since I don't know my parent\r\n");
                                                break;
                                        }
#endif
                                        decodedis((struct icmpv6_sim_packet *)(pkt->data));
                                        encodedio(&rpkt);
                                        rpl_send((char *)(pkt->src_node), (char *)(&NetData.ipaddr), RPL_DIO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);

                                        break;
                                case RPL_DIO_MSGTYPE:
                                        kprintf("DIO Received\r\n");
                                        parent_changed = 0;
                                        processdio((struct icmpv6_sim_packet *)(pkt->data), *((uint32 *)(pkt->src_node)), &parent_changed);
                                        if(parent_changed != 0){
                                                encodedao(&rpkt);
                                                kprintf("Sending DAO to [%04x]\r\n", *((uint32 *)(RPL_MYINFO.dodagid)));
                                                if(RPL_MYINFO.parent_index > -1 && RPL_MYINFO.parent_index < LOWPAN_MAX_NODES){
                                                        rpl_send((char *)(&(rpl_link_local_neighbors[RPL_MYINFO.parent_index].iface_addr)), (char *)(&NetData.ipaddr), RPL_DAO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
                                                }
                                                else{
                                                        kprintf("WARN : Cannot send the DAO message after receiving DIO message since we do not know the parent\r\n");
                                                }
                                        }
                                        else{
                                                kprintf("INFO : Parent did not change so not doing anything\r\n");
                                        }
                                        break;
                                case RPL_DAO_MSGTYPE:
                                        kprintf("DAO Received\r\n");
#ifdef LOWPAN_BORDER_ROUTER
                                        kprintf("**********DAO RECEIVED from %04x ", *((uint32 *)(pkt->src_node)));
                                        processdao((struct icmpv6_sim_packet *)(pkt->data));
#endif
#ifdef LOWPAN_NODE
                                        if(RPL_MYINFO.parent_index > -1 && RPL_MYINFO.parent_index < LOWPAN_MAX_NODES){
                                                kprintf("**********FORWARDING DAO MESSAGE TO %04x from %04x\r\n",rpl_link_local_neighbors[RPL_MYINFO.parent_index].iface_addr,  *((uint32 *)(pkt->src_node)));
                                                rpl_send((char *)(&(rpl_link_local_neighbors[RPL_MYINFO.parent_index].iface_addr)), (char *)(&NetData.ipaddr), RPL_DAO_MSGTYPE, (char *)(pkt->data), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
                                        }
                                        else{
                                                kprintf("WARN : Cannot forward the DAO message since we do not know the parent\r\n");
                                        }
#endif
                                        break;
                                default:
                                        kprintf("Received an unknown RPL message type\r\n");
                                        break;
                        }

                }
                //kprintf("Before signal i is [%d] j is [%d]\r\n", i, j);
                signal(rpl_sim_write_sem);
                //kprintf("Before incrementing i is [%d] j is [%d]\r\n", i, j);
                i = (i+1)%RPL_SIM_RECV_QUEUE_LEN;
                //kprintf("After handling packet i is [%d] j is [%d]\r\n", i, j);
        }

        return OK;
}
