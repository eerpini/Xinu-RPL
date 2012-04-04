#include <xinu.h>

void encodedao (struct netpacket *rpldaomsg) {

	struct rpl_dao_msg		daomsg;
	struct rpl_opt_target		opttarget;
	struct rpl_opt_transitinf	opttransit;
	int				pos = 0;

	daomsg.rpl_instance_id = RPL_MYINFO.instance_id;
	daomsg.flags = 0;
	daomsg.reserved = 0;
	daomsg.dao_seq = 0;//RPL_MYINFO.dao_seq;
	memcpy (daomsg.dodagid, RPL_MYINFO.dodagid, RPL_DODAGID_LEN);

	// encode rpl target for reachability
	opttarget.type = RPL_OPT_TYPE_TARGET;
	opttarget.len = sizeof (struct rpl_opt_target);
	opttarget.flags = 0;
	opttarget.prefixlen = RPL_DODAGID_LEN;
	memcpy (opttarget.target, RPL_MYINFO.myaddr, RPL_DODAGID_LEN);


	//encode rpl transit info for parent information
	opttransit.type = RPL_OPT_TYPE_TRANSIT;
	opttransit.len = sizeof (struct rpl_opt_transitinf);
	opttransit.pathctl = 0;
	opttransit.pathseq = 0;
	opttransit.pathlifetime = 0;//RPL_MYINFO.lifetime;
	memcpy (opttransit.parent, RPL_MYINFO.parent, RPL_DODAGID_LEN);

	//build the message into the netpacket
        rpldaomsg->net_ictype = RPL_CONTROL_MSGTYPE_ICMP;
        rpldaomsg->net_iccode = RPL_DAO_MSGTYPE;
        rpldaomsg->net_iccksum = 0;

	memcpy (rpldaomsg->net_icdata, &daomsg, sizeof (struct rpl_dao_msg));
	pos += sizeof (struct rpl_dao_msg);

	memcpy (&rpldaomsg->net_icdata[pos], &opttarget, sizeof (struct rpl_opt_target));
	pos += sizeof (struct rpl_opt_target);

	memcpy (&rpldaomsg->net_icdata[pos], &opttransit, sizeof (struct rpl_opt_transitinf));
}

void decodedao (struct netpacket *rpldaomsg) {

	struct	rpl_dao_msg		*daomsg;
	struct	rpl_opt_target		*opttarget;
	struct	rpl_opt_transitinf	*opttransit;
	int				pos = 0;
	
	if (rpldaomsg->net_ictype != RPL_CONTROL_MSGTYPE_ICMP){
		kprintf (" This is NOT a RPL Control message %02x \n", rpldaomsg->net_ictype);
		return;
	}

	if (rpldaomsg->net_iccode != RPL_DAO_MSGTYPE){
		kprintf (" This is NOT a RPL DAO message %02x \n", rpldaomsg->net_iccode);
		return;
	}
	
	daomsg = (struct rpl_dao_msg *)rpldaomsg->net_icdata;
	kprintf (" DAO Msg Instance ID %02x \n", daomsg->rpl_instance_id);
	kprintf (" DAO Msg Dodag ID %02x:%02x:%02x:%02x\n", 
		daomsg->dodagid[0], daomsg->dodagid[1], daomsg->dodagid[2], daomsg->dodagid[3]);
		

	// Decode the options here -- RPL option Target
	pos += sizeof (struct rpl_dao_msg);
	opttarget = (struct rpl_opt_target *) &rpldaomsg->net_icdata[pos];
	if (opttarget->type != RPL_OPT_TYPE_TARGET){
		kprintf (" DAO Message: DAO message does not have an RPL OPTION TARGET %02x\n", opttarget->type);
		return;
	}

	kprintf (" DAO Message option type %02x \n", opttarget->type);
	kprintf (" DAO Message target len %02x \n", opttarget->len);
	kprintf (" DAO Message prefix len %02x \n", opttarget->prefixlen);
	kprintf (" DAO Message target ID %02x:%02x:%02x:%02x\n", 
			opttarget->target[0], opttarget->target[1], opttarget->target[2], opttarget->target[3]);


	// RPL OPTION TRANSIT INFO
	pos += sizeof (struct  rpl_opt_target);
	opttransit = (struct  rpl_opt_transitinf *)&rpldaomsg->net_icdata[pos];
	if (opttransit->type != RPL_OPT_TYPE_TRANSIT){
		kprintf (" DAO Message: DAO message does not have an RPL OPTION TRANSIT INFO %02x\n", opttransit->type);
		return;
	}

	kprintf (" DAO Message option type %02x \n",opttransit->type);
	kprintf (" DAO Message transitinfo len %02x \n", opttransit->len);
	kprintf (" DAO Message parent ID %02x:%02x:%02x:%02x\n", 
		opttransit->parent[0], opttransit->parent[1], opttransit->parent[2], opttransit->parent[3]);
}
