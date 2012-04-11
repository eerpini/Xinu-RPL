/*
 *  RPL Implementation specifics
 */

/* RPL Messages
 * DIO
 * DIS
 * DAO
 * (Optional)DAO-ACK
 */
/*
 * FIXME : The following length definition should be in bytes
 */
#define         RPL_DODAGID_LEN     	32

#ifndef         LOWPAN_MAX_NODES
       #define LOWPAN_MAX_NODES        	64
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
        byte    dodagid[RPL_DODAGID_LEN];
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
	byte	dodagid[32];
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
/*
struct route_info{
        byte    adj_matrix[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES/8];
        int32   iface_addr[LOWPAN_MAX_NODES]; 
        byte    source_route_info[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

};

*/

#endif

/*      Constants for RPL       */
#define         RPL_BASE_RANK   		0
#define         RPL_ROOT_RANK   		1
#define         RPL_INFINITE_RANK       	0XFFFF
#define         RPL_DEFAULT_INSTANCE    	0
#define         RPL_DEFAULT_PATH_CONTROL_SIZE   0
#define         RPL_DEFAULT_MIN_HOP_RANK_INCREASE       256
#define         RPL_DEFAULT_DAO_DELAY   	1
#define         RPL_DIO_TIMER_MAX       	RPL_DEFAULT_DIO_INTERVAL_MIN*pow(RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS,2)
#define         RPL_DAG_VERSION_INC_TIMER       0xff
#define         RPL_DELAY_DIO_TIMER             0xff

#ifdef  LOWPAN_BORDER_ROUTER
#define         RPL_REMOVE_TIMER                0xff
#endif



/*
 * Temporary definitions for testing puposes
 */

uint32   rpl_link_local_neighbors[LOWPAN_MAX_NODES];


/* 		RPL Message Types 		*/

#define		RPL_CONTROL_MSGTYPE_ICMP	155
#define		RPL_DIS_MSGTYPE			0x00
#define		RPL_DIO_MSGTYPE			0x01
#define		RPL_DAO_MSGTYPE			0x02
#define		RPL_DAO_ACK_MSGTYPE		0x03
#define		RPL_TIMER_EXPIRY                0x04

struct		rpl_info {

	byte	instance_id;
	uint16 	rank;
	uint8	version;
	byte	dtsn;
	byte	dodagid[RPL_DODAGID_LEN];
	byte	parent[RPL_DODAGID_LEN];
	byte	myaddr[RPL_DODAGID_LEN];

	//Trickle constants
	byte	trickle_imin;
	byte	trickle_imax;
	byte	trickle_k;

	//Dag Conf
	byte	diointdouble;
	byte	diointmin;
	byte	dioredundancy;
	uint16	maxrankincrease;
	uint16	minhoprankinc;
        byte    pathlifetime;
};
struct	rpl_info	RPL_MYINFO;


/*		RPL OPTION Messages	*/

#define		RPL_OPT_TYPE_PAD1		0x00
struct	rpl_opt_pad1 {
	byte	type;
};


#define		RPL_OPT_TYPE_PADN		0x01
struct	rpl_opt_padn {
	byte	type;
	byte	len;
};

#define		RPL_MAX_PREFIX_LEN		128
#define		RPL_OPT_TYPE_ROUTEINFO		0x03
#define		RPL_MAX_ROUTE_LIFETIME		0xFFFFFFFF
struct	rpl_opt_routeinfo{
	byte	type;
	byte	len;
	byte	prefix_len;
	byte	reserved;

	uint32	route_lifetime;
	byte	prefix[RPL_MAX_PREFIX_LEN];
};

#define		RPL_OPT_TYPE_DODAG_CONF		0x04
#define		RPL_OPT_DEFAULT_PATH_LIFETIME   20
#define		RPL_OPT_LIFETIME_UNIT		1
struct	rpl_opt_dodag_conf {
	byte	type;
	byte	len;
	byte	flags;
	byte	diointdouble;

	byte	diointmin;
	byte	dioredundancy;
	uint16	maxrankincrease;

	uint16	minhoprankinc;
	uint16	ocp;

	byte	reserved;
	byte	lifetime;
	uint16	lifetime_unit;
};

#define		RPL_OPT_TYPE_TARGET		0x05
struct	rpl_opt_target {
	byte	type;
	byte	len;
	byte	flags;
	byte	prefixlen;

	byte	target[RPL_MAX_PREFIX_LEN];
};

#define		RPL_OPT_TYPE_TRANSIT		0x06
struct	rpl_opt_transitinf {
	byte	type;
	byte	len;
	byte	flags;
	byte	pathctl;

	byte	pathseq;
	byte	pathlifetime;

	byte	parent[RPL_MAX_PREFIX_LEN];
};
