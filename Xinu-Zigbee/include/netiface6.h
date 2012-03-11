/* netiface.h - definintions for network interfaces used by IP */

#define	NIFACES	1		/* number of intefaces (the	*/
				/*   the external Internet	*/
				/*   plus two "backend" nets.	*/
#define	IF_UP		1	/* Interface is currently on line	*/
#define	IF_DOWN		0	/* Interface is currently offline	*/
#define	IF_IQUEUESIZE	3	/* Size of packet queue for interface	*/
#define	IF_OQUEUESIZE	3	/* Size of packet queue for interface	*/
#define MAX_IP6_ADDR	4	/* IPv6 addresses for this interface	*/	

/* Network interface */

struct	ifentry	{
	bool8	if_state;	/* interface is either up or down	*/
	bool8	if_ip6valid;	/* Are IP fields valid yet?		*/
	byte	if_macucast[RAD_ADDR_LEN]; /* MAC unicast address	*/
	IP6addr	if_ip6ucast[MAX_IP6_ADDR];	/* IP unicast address 	*/
	IP6addr	if_ip6prefix;	/* IP prefix for the network		*/
	IP6addr	if_ip6router;	/* IP address of default router         */
				/* Replace this entry with a routing	*/
				/*   table				*/

	sid32	if_isem;	/* semaphore counts incoming packets	*/
				/*    in the queue			*/
	struct	netpacket *if_iqueue[IF_IQUEUESIZE]; /* queue to hold	*/
				/*  incoming packets for this interface	*/
	int32	if_ihead;	/* next entry in packet queue to remove	*/
	int32	if_itail;	/* next slot in packet queue to insert	*/
	int32	if_ipool;	/* pool of input packet buffers		*/

	sid32	if_osem;	/* semaphore counts incoming packets	*/
				/*    in the queue			*/
	struct	netpacket *if_oqueue[IF_OQUEUESIZE]; /* queue to hold	*/
				/*  incoming packets for this interface	*/
	int32	if_ohead;	/* next entry in packet queue to remove	*/
	int32	if_otail;	/* next slot in packet queue to insert	*/
	int32	if_opool;	/* pool of input packet buffers		*/
	int32	if_oseq;	/* output frame sequence number		*/
};

extern	struct	ifentry	if_tab[];

extern	bool8	host;		/* TRUE if running a host		*/

/* neighbor cache */

/* routing table */

