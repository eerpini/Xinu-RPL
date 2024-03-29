#include <xinu.h>
//128.10.3.108
#define node8 0x800a036c
//128.10.3.107
#define node7 0x800a036b
//128.10.3.106
#define node6 0x800a036a
//128.10.3.105
#define node5 0x800a0369
//128.10.3.104
#define node4 0x800a0368
//128.10.3.103
#define node3 0x800a0367
//128.10.3.102 
#define node2 0x800a0366

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
         * Static assignment of the neighbors
         * This should be picked up from the neighbor list 
         * which will be supplied by the radio layer
         */

        //128.10.3.113 is 0x800a0371
        switch(NetData.ipaddr){
                /*
                 * node2 or 128.10.3.102 is the root, we
                 * don't really need neighbor information for it
                 */
                case node4:
                        rpl_link_local_neighbors[0].iface_addr = node3;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = node5;
                        rpl_link_local_neighbors[1].is_parent = 0;
                        rpl_link_local_neighbors[2].iface_addr = -1;
#ifdef DEBUG
                        kprintf("I am node 2 My neighbors are node3 and node5\r\n");
#endif
                        break;
                case node5:
                        rpl_link_local_neighbors[0].iface_addr = node6;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = node7;
                        rpl_link_local_neighbors[1].is_parent = 0;
                        rpl_link_local_neighbors[2].iface_addr = node4;
                        rpl_link_local_neighbors[2].is_parent = 0;
                        rpl_link_local_neighbors[3].iface_addr = -1;
#ifdef DEBUG
                        kprintf(" I am node 5 My neighbors are node6 and node7 and node4\r\n");
#endif
                        break;
                case node3:
                        rpl_link_local_neighbors[0].iface_addr = node4;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = node2;
                        rpl_link_local_neighbors[1].is_parent = 0;
                        rpl_link_local_neighbors[2].iface_addr = -1;
#ifdef DEBUG
                        kprintf("I am node 3 My neighbors are node4 and node2\r\n");
#endif
                        break;
                case node2:
                        rpl_link_local_neighbors[0].iface_addr = node3;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = -1;
#ifdef DEBUG
                        kprintf(" I am node 2 My neighbors are node3 \r\n");
#endif
                        break;
                case node6:
                        rpl_link_local_neighbors[0].iface_addr = node5;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = -1;
#ifdef DEBUG
                        kprintf(" I am node 6 My neighbors are node5 \r\n");
#endif
                        break;
                case node7:
                        rpl_link_local_neighbors[0].iface_addr = node5;
                        rpl_link_local_neighbors[0].is_parent = 0;
                        rpl_link_local_neighbors[1].iface_addr = -1;
#ifdef DEBUG
                        kprintf(" I am node 7 My neighbors are node5 \r\n");
#endif
                        break;
                default : 
                        kprintf("I did not find a neighbor assignment for myself, I must be the border router or the gateway\r\n");
        }

#ifdef DEBUG
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[0].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[1].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[2].iface_addr);
#endif



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


        rpl_node_init();

#ifdef  LOWPAN_BORDER_ROUTER
        rpl_border_router_init();
#endif

        generate_link_local_neighbors();
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
        //RPL_MYINFO.rank = RPL_INFINITE_RANK;
        RPL_MYINFO.rank = 0;
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
                       kprintf("WARN : rpl_border_router_init :getting the IP address failed, init will fail !!\r\n");
               }
               my_index = assignindex(NetData.ipaddr);
               
        }
        else{
               my_index = assignindex(NetData.ipaddr);
        }
#ifdef DEBUG
        kprintf("I was assigned the index : %d\r\n", my_index);
#endif
        memcpy(RPL_MYINFO.dodagid, &(NetData.ipaddr), RPL_DODAGID_LEN);
        /*
         * FIXME : Setting the rank here
         */
        RPL_MYINFO.rank = 10;

}
#endif

void send_init_messages(){

#ifdef LOWPAN_NODE
        
#ifdef DEBUG
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[0].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[1].iface_addr);
        kprintf("The first neighbor is : %04x\r\n", rpl_link_local_neighbors[2].iface_addr);
#endif
        struct icmpv6_sim_packet pkt;
        encodedis( &pkt );

#ifdef DEBUG
        kprintf("Done encoding the dis packet\r\n");
#endif
        int i = 0;
        do{
#ifdef DEBUG
                kprintf("[%d] Sending a packet to [%04x] from me[%04x]\r\n", i, rpl_link_local_neighbors[i].iface_addr, NetData.ipaddr);
#endif
                if(rpl_link_local_neighbors[i].iface_addr != NetData.ipaddr){
                        rpl_send((char *) &(rpl_link_local_neighbors[i].iface_addr), (char *)(&(NetData.ipaddr)), RPL_DIS_MSGTYPE, (char *)(&pkt),  
                                        1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN);
                }
                i++;
        }while(rpl_link_local_neighbors[i].iface_addr != -1);
#ifdef DEBUG
        kprintf("We stopped at i [%d] and the corressponding iface_addr [%04x]\r\n",i, rpl_link_local_neighbors[i].iface_addr);
#endif



#endif
}

void rpl_process_path_timeout(){

#ifdef LOWPAN_NODE
        rpl_dao_timeout += RPL_MYINFO.pathlifetime*1000;
        struct icmpv6_sim_packet rpkt;
        encodedao(&rpkt);
        if(NetData.ipvalid && (*((uint32 *)(RPL_MYINFO.dodagid)) != 0)){
                if(RPL_MYINFO.parent_index > -1 && RPL_MYINFO.parent_index < LOWPAN_MAX_NODES){
#ifdef DEBUG
                        kprintf("Sending a TIMEOUT DAO Message \r\n");
#endif
                        rpl_send((char *)(&(rpl_link_local_neighbors[RPL_MYINFO.parent_index].iface_addr)), (char *)(&(NetData.ipaddr)), RPL_DAO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
                }
                else{
                        kprintf("WARN : Cannot send DAO on path life time timeout since we don't know our parent\r\n");
                }
        }
#endif
#ifdef LOWPAN_BORDER_ROUTER
        rpl_dao_timeout += (RPL_MYINFO.pathlifetime + 5)*1000;
        processPathlifetimeTimeout();

#ifdef DEBUG
       kprintf("In Border router timeout value is : %d\r\n", rpl_dao_timeout); 
#endif

#endif

}

void rpl_process_dis_timeout(){

#ifdef LOWPAN_BORDER_ROUTER
        return;
#endif
#ifdef LOWPAN_NODE
        rpl_dis_timeout += 10*1000;
        struct icmpv6_sim_packet pkt;
        int i = 0;
        encodedis( &pkt );
        do{
#ifdef DEBUG
                kprintf("[%d] Sending a packet to [%04x] from me[%04x]\r\n", i, rpl_link_local_neighbors[i].iface_addr, NetData.ipaddr);
#endif
                if(rpl_link_local_neighbors[i].iface_addr != NetData.ipaddr){
                        rpl_send((char *) &(rpl_link_local_neighbors[i].iface_addr), (char *)(&(NetData.ipaddr)), RPL_DIS_MSGTYPE, (char *)(&pkt),  
                                        1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN);
                }
                i++;
        }while(rpl_link_local_neighbors[i].iface_addr != -1);
#endif
}
