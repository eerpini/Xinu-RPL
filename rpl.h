/*
 *  RPL Implementation specifics
 */

/* RPL Messages
 * DIO
 * DIS
 * DAO
 * (Optional)DAO-ACK
 */
#define         IPV6_ADDR_LEN   128
#define         DODAGID_LEN     IPV6_ADDR_LEN 
#ifndef         LOWPAN_MAX_NODES
        #define LOWPAN_MAX_NODES        64
#endif

/*
 * RPL DIO  - DODAG Information Object Message Options
      0x00 Pad1
      0x01 PadN
      0x02 Metric Container
      0x03 Routing Information
      0x04 DODAG Configuration
      0x08 Prefix Information
*/
struct rpl_dio_msg{
        byte    rpl_instance_id;
        byte    version;
        int16    rank;
        /*
         *  Next byte
         *   |G|0|MOP|Prf|
         *   |1|1|3  |3  |
         */
        byte    grounded_mop_pref;
        /*
         * Destination advertisement trigger sequence number
         */
        byte    dtsn;
        byte    flags;
        byte    reserved;
        byte    dodagid[DODAGID_LEN];
        int32   options;
};


/*
 * RPL DIS MESSAGE OPTIONS - DODAG Information Solicitation Message
 * 0x00 Pad1
 * 0x01 PadN
 * 0x07 Solicited Information
 */

struct rpl_dis_msg{
        byte    flags;
        byte    reserved;
        int32   options;
};

/*
 * RPL DAO Message - Destination Advertisement Object
 * OPtions
      0x00 Pad1
      0x01 PadN
      0x05 RPL Target
      0x06 Transit Information
      0x09 RPL Target Descriptor

 */
struct  rpl_dao_msg{
        byte    rpl_instance_id;
        /*
         * Next Byte
         * |K|D|     FLAGS   |
         * |1|1|    6    |
         */
        byte    flags;
        byte    reserved;
        /*  DAO Sequence  */
        byte    dao_seq;
        int32 options;
};

/*      RPL Trickle Timer Constants/Defs     */

#define RPL_DEFAULT_DIO_INTERVAL_MIN        3
#define RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS  20
#define RPL_DEFAULT_DIO_REDUNDANCY_CONSTANT 10

/*      Values incremented on every DIO message */
struct rpl_dio_timer_const{
        uint32  imin;
        uint32  imax;
        uint32  k;
};

/*
 * Node Specific Definitions and Data Structures
 */

#ifdef LOWPAN_NODE

/*      Neighbor List - Link Local Addresses    */
struct n_list{

        /*
         * We need to differentiate between neighbors/parents/preferred_parent
         * in the list
         */
        byte    is_parent;
        /*      Link Local Address      */
        int16   iface_addr;
};
#endif

/*      Definitions/Data Structures on the border router        */
#ifdef  LOWPAN_BORDER_ROUTER

/*      Information about the routes in the network graph       */
struct route_info{
        byte    adj_matrix[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES/8];
        /* 
         * List of link local address of the nodes in the network
         * maps directly to the rows of the adj_matrix
         */
        int16   iface_addr[LOWPAN_MAX_NODES]; 
        /*      
         *      Pre-computed Source Route info for the nodes, this is a sparse matrix
         *      the values are indices into the above iface_addr array, we can improve 
         *      this, just a place holder for now.
         */
        byte    source_route_info[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

};


#endif

/*      Constants for RPL       */
#define         RPL_BASE_RANK   0
#define         RPL_ROOT_RANK   1
#define         RPL_INFINITE_RANK       0XFFFF
#define         RPL_DEFAULT_INSTANCE    0
#define         RPL_DEFAULT_PATH_CONTROL_SIZE   0
#define         RPL_DEFAULT_MIN_HOP_RANK_INCREASE       256
#define         RPL_DEFAULT_DAO_DELAY   1
#define         RPL_DIO_TIMER_MAX       RPL_DEFAULT_DIO_INTERVAL_MIN*pow(RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS,2)
#define         RPL_DAG_VERSION_INC_TIMER       0xff
#define         RPL_DELAY_DIO_TIMER             0xff

#ifdef  LOWPAN_BORDER_ROUTER
#define         RPL_REMOVE_TIMER                0xff
#endif

