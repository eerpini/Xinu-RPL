#include <xinu.h>

void encodedis (struct icmpv6_sim_packet *rpldis) {

	struct	rpl_dis_msg	dis;
	dis.flags = 0;
	dis.reserved = 0;

	/* build an ICMP packet with the above information */
	rpldis->net_ictype 		= RPL_CONTROL_MSGTYPE_ICMP;
	rpldis->net_iccode		= RPL_DIS_MSGTYPE;
	rpldis->net_iccksum		= 0;

	memset ( rpldis->net_icdata, 0, (1500-ETH_HDR_LEN - RPL_SIM_HDR_LEN));

	memcpy (rpldis->net_icdata, &dis, sizeof (struct rpl_dis_msg));
}

void decodedis (struct icmpv6_sim_packet *rpldis) {

	struct	rpl_dis_msg	*dismsg;

	dismsg = (struct rpl_dis_msg *) rpldis->net_icdata;
#ifdef DEBUG
	kprintf (" ICMP Type %02x \n", rpldis->net_ictype);
	kprintf (" ICMP code %02x \n", rpldis->net_iccode);

	//1. Flags
	kprintf (" DIS Flags %02x \n", dismsg->flags);

	//2. Reserved
	kprintf (" DIS Reserved %02x \n", dismsg->reserved );

	//3. Options -- Ignore the options for the time being
#endif
}
