#include <xinu.h>

extern struct rpl_info RPL_MYINFO;
#ifdef LOWPAN_BORDER_ROUTER
        //distance to each nodes
        extern byte		rplpath[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

        //adjacency list
        extern byte		rpladjlist[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

        // list of interface addresses
        extern int32   	iface_addr[LOWPAN_MAX_NODES];

        //structure for each node
        extern struct nodeinfo state[LOWPAN_MAX_NODES];

#endif

uint32   rpl_link_local_neighbors[LOWPAN_MAX_NODES];
int32    rpl_dao_timeout;



/*
 * Function for filling in the link local addresses,
 * this is just for testing and will be replaced by the radio layer ND Code 
 * when ready
 */

void generate_link_local_neighbors(){

        rpl_link_local_neighbors[0] = (NetData.ipaddr + 1);
        rpl_link_local_neighbors[1] = (NetData.ipaddr - 1);
        rpl_link_local_neighbors[2] = -1;
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

        send_init_messages();

}

void rpl_node_init(){

 
        RPL_MYINFO.instance_id = 0;
        RPL_MYINFO.rank = RPL_INFINITE_RANK;
        RPL_MYINFO.version = 0;
        RPL_MYINFO.dtsn = 0;
        memset(RPL_MYINFO.dodagid, NULLCH, RPL_DODAGID_LEN);
        memset(RPL_MYINFO.parent, NULLCH, RPL_DODAGID_LEN);
        memset(RPL_MYINFO.myaddr, NULLCH, RPL_DODAGID_LEN);
        RPL_MYINFO.trickle_imin =  0;
        RPL_MYINFO.trickle_imax =  0;
        RPL_MYINFO.trickle_k =  0;

        RPL_MYINFO.diointdouble =  RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
        RPL_MYINFO.diointmin =  RPL_DEFAULT_DIO_INTERVAL_MIN;
        RPL_MYINFO.dioredundancy =  RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT;
        RPL_MYINFO.maxrankincrease =  0;
        RPL_MYINFO.minhoprankinc =  0;
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

        memset(rplpath, NULLCH, LOWPAN_MAX_NODES*LOWPAN_MAX_NODES); 
        memset(rpladjlist , NULLCH, LOWPAN_MAX_NODES*LOWPAN_MAX_NODES); 
        memset(iface_addr, NULLCH, (RPL_DODAGID_LEN/8)*LOWPAN_MAX_NODES ); 

        if(!NetData.ipvalid){
                getlocalip();
        }
        else{
               assignindex(NetData.ipaddr);
        }

}
#endif

void send_init_messages(){

#ifdef LOWPAN_NODE
        
        struct icmpv6_sim_packet pkt;
        encodedis( &pkt );

        kprintf("Done encoding the dis packet\r\n");
        int i = 0;
        while(rpl_link_local_neighbors[i] != -1){
                rpl_send((char *) &rpl_link_local_neighbors[i], (char *)(&(NetData.ipaddr)), RPL_DIS_MSGTYPE, (char *)(&pkt),  
                                1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN);
                i++;
        }



#endif
}

void rpl_process_path_timeout(){

        /*
        struct icmpv6_sim_packet rpkt;
#ifdef LOWPAN_NODE
        encodedao(&rpkt);
        rpl_send((char *)(RPL_MYINFO.parent), (char *)(NetData.ipaddr), RPL_DIO_MSGTYPE, (char *)(&rpkt), 1500-ETH_HDR_LEN- RPL_SIM_HDR_LEN);
#endif
#ifdef LOWPAN_BORDER_ROUTER

       kprintf("In Border router\r\n"); 

#endif

        rpl_dao_timeout += RPL_MYINFO.pathlifetime*1000;
        */

}
