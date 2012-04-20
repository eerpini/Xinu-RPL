#include <xinu.h>

extern struct rpl_info RPL_MYINFO;
#ifdef LOWPAN_BORDER_ROUTER
        //distance to each nodes
        extern byte		rplpath[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

        //adjacency list
        extern byte		rpladjlist[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

        // list of interface addresses
        extern uint32   	iface_addr[LOWPAN_MAX_NODES];

        //structure for each node
        extern struct nodeinfo state[LOWPAN_MAX_NODES];

#endif

struct n_list   rpl_link_local_neighbors[LOWPAN_MAX_NODES];
//uint32   rpl_link_local_neighbors[LOWPAN_MAX_NODES];
int32    rpl_dao_timeout;
int32    rpl_dis_timeout;

void    rpl_node_init();


/*
 * Function for filling in the link local addresses,
 * this is just for testing and will be replaced by the radio layer ND Code 
 * when ready
 */

void generate_link_local_neighbors(){

        /*
        rpl_link_local_neighbors[0] = (NetData.ipaddr + 1);
        rpl_link_local_neighbors[1] = (NetData.ipaddr - 1);
        */
        //dot2ip("128.10.3.113", &rpl_link_local_neighbors[0]);
        dot2ip("128.10.3.114", &(rpl_link_local_neighbors[0].iface_addr));
        dot2ip("128.10.3.112", &(rpl_link_local_neighbors[1].iface_addr));
        dot2ip("128.10.3.113", &(rpl_link_local_neighbors[2].iface_addr));
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[0].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[1].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[2].iface_addr);
        rpl_link_local_neighbors[0].is_parent = 0;
        rpl_link_local_neighbors[1].is_parent = 0;
        rpl_link_local_neighbors[2].is_parent = 0;
        rpl_link_local_neighbors[3].iface_addr = -1;
        /*
        int i=0;
        for(i=0; i < LOWPAN_MAX_NODES; i++){
                rpl_link_local_neighbors[i] = i;
        }
        */

}


/*
 * Rpl initialization function, called when RPL has to be started
 */
/*
 * FIXME : Check the return status of the functions and accordingly
 * return error code from rpl_init()
 */

void rpl_init(){

        if(!NetData.ipvalid){
                getlocalip();
        }
        generate_link_local_neighbors();


        rpl_node_init();

#ifdef  LOWPAN_BORDER_ROUTER
        rpl_border_router_init();
#endif

        rpl_dao_timeout = RPL_MYINFO.pathlifetime*1000;
        rpl_dis_timeout = 10*1000;

        send_init_messages();

}

void rpl_node_init(){

        int i= 0;
        for(i=0; i< LOWPAN_MAX_NODES; i++){
                rpl_link_local_neighbors[i].is_parent = 0;
                rpl_link_local_neighbors[i].iface_addr = -1;
        }
 
        RPL_MYINFO.instance_id = 0;
        RPL_MYINFO.rank = RPL_INFINITE_RANK;
        RPL_MYINFO.version = 0;
        RPL_MYINFO.dtsn = 0;
        memset(RPL_MYINFO.dodagid, NULLCH, RPL_DODAGID_LEN);
        //memset(RPL_MYINFO.parent, NULLCH, RPL_DODAGID_LEN);
        //memset(RPL_MYINFO.myaddr, NULLCH, RPL_DODAGID_LEN);
        memcpy(RPL_MYINFO.myaddr, &(NetData.ipaddr), RPL_DODAGID_LEN);
        RPL_MYINFO.trickle_imin =  0;
        RPL_MYINFO.trickle_imax =  0;
        RPL_MYINFO.trickle_k =  0;

        RPL_MYINFO.diointdouble =  RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
        RPL_MYINFO.diointmin =  RPL_DEFAULT_DIO_INTERVAL_MIN;
        RPL_MYINFO.dioredundancy =  RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
        RPL_MYINFO.maxrankincrease =  0;
        RPL_MYINFO.minhoprankinc =  0;
        RPL_MYINFO.parent_index = -1;
        /*
         * FIXME Should the starting value not be the same as DEFAULT ?
         */
#ifdef LOWPAN_NODE
        RPL_MYINFO.pathlifetime = RPL_OPT_DEFAULT_PATH_LIFETIME;
#endif
#ifdef LOWPAN_BORDER_ROUTER
        RPL_MYINFO.pathlifetime = RPL_OPT_DEFAULT_PATH_LIFETIME;
#endif

}

#ifdef  LOWPAN_BORDER_ROUTER

void rpl_border_router_init(){

        int my_index = -1;
        int i, j;
        for(i = 0; i < LOWPAN_MAX_NODES; i++){
                iface_freshness[i] = 0;
                for(j=0; j< LOWPAN_MAX_NODES; j++){
                        rplpath[i][j] = 0;
                        rpladjlist[i][j] = 0;
                }
        }
        //memset(iface_addr, NULLCH, (RPL_DODAGID_LEN)*LOWPAN_MAX_NODES ); 

        for(my_index = 0; my_index < LOWPAN_MAX_NODES; my_index ++){
                iface_addr[my_index] = 0;

        }
        my_index = -1;

        if(!NetData.ipvalid){
               if( getlocalip() == SYSERR){
                       kprintf("rpl_border_router_init :getting the IP address failed, init will fail !!\r\n");
               }
               my_index = assignindex(NetData.ipaddr);
               
        }
        else{
               my_index = assignindex(NetData.ipaddr);
        }
        kprintf("I was assigned the index : %d\r\n", my_index);
        memcpy(RPL_MYINFO.dodagid, &(NetData.ipaddr), RPL_DODAGID_LEN);

}
#endif

void send_init_messages(){

#ifdef LOWPAN_NODE
        
        struct icmpv6_sim_packet pkt;
        encodedis( &pkt );

        kprintf("Done encoding the dis packet\r\n");
        int i = 0;
        do{
                kprintf("[%d] Sending a packet to [%04x] from me[%04x]\r\n", i, rpl_link_local_neighbors[i].iface_addr, NetData.ipaddr);
                if(rpl_link_local_neighbors[i].iface_addr != NetData.ipaddr){
                        rpl_send((char *) &(rpl_link_local_neighbors[i].iface_addr), (char *)(&(NetData.ipaddr)), RPL_DIS_MSGTYPE, (char *)(&pkt),  
                                        1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN);
                }
                i++;
        }while(rpl_link_local_neighbors[i].iface_addr != -1);
        kprintf("We stopped at i [%d] and the corressponding iface_addr [%04x]\r\n",i, rpl_link_local_neighbors[i].iface_addr);



#endif
}

void rpl_process_path_timeout(){

#ifdef LOWPAN_NODE
        rpl_dao_timeout += RPL_MYINFO.pathlifetime*1000;
        struct icmpv6_sim_packet rpkt;
        encodedao(&rpkt);
        if(NetData.ipvalid && (*((uint32 *)(RPL_MYINFO.dodagid)) != 0)){
                if(RPL_MYINFO.parent_index > -1 && RPL_MYINFO.parent_index < LOWPAN_MAX_NODES){
                        rpl_send((char *)(&(rpl_link_local_neighbors[RPL_MYINFO.parent_index].iface_addr)), (char *)(&(NetData.ipaddr)), RPL_DAO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
                }
                else{
                        kprintf("WARN : Cannot send DAO on path life time timeout since we don't know our parent\r\n");
                }
        }
#endif
#ifdef LOWPAN_BORDER_ROUTER
        rpl_dao_timeout += (RPL_MYINFO.pathlifetime + 5)*1000;
        //processPathlifetimeTimeout();

       kprintf("In Border router timeout value is : %d\r\n", rpl_dao_timeout); 

#endif

}

void rpl_process_dis_timeout(){

#ifdef LOWPAN_BORDER_ROUTER
        return;
#endif
#ifdef LOWPAN_NODE
        return;
#endif
        rpl_dis_timeout += 10*1000;
        struct icmpv6_sim_packet pkt;
        int i = 0;
        encodedis( &pkt );
        do{
                kprintf("[%d] Sending a packet to [%04x] from me[%04x]\r\n", i, rpl_link_local_neighbors[i].iface_addr, NetData.ipaddr);
                if(rpl_link_local_neighbors[i].iface_addr != NetData.ipaddr){
                        rpl_send((char *) &(rpl_link_local_neighbors[i].iface_addr), (char *)(&(NetData.ipaddr)), RPL_DIS_MSGTYPE, (char *)(&pkt),  
                                        1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN);
                }
                i++;
        }while(rpl_link_local_neighbors[i].iface_addr != -1);
}
