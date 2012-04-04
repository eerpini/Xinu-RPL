#include <xinu.h>

int getindex (uint32 addr) {
	int index = 0;
	for (index = 0; index < LOWPAN_MAX_NODEX; index ++) {
		if (iface_addr[index] == addr)
			return index;
	}

	return SYSERR;
}

int assignindex (uint32 addr) {
	int index = 0;

	for (index = 0; index < LOWPAN_MAX_NODEX; index ++) {
		if (iface_addr[index] == 0){
			iface_addr[index] = addr;
			return index;
		}
	}
	return SYSERR;
}

void processroute (uint32 target, uint32 parent) {

	int pindex = SYSERR;
	int tindex = SYSERR;

	//check if the parent exists
	if (getindex (parent) == SYSERR) {
		kprintf (" DAO Message-> Parent does not exist in the map %04x \n", parent);
		return;
	}

	pindex = getindex (parent);

	if (getindex (target) == SYSERR) 
		tindex = assignindex (target);
	else 
		tindex = getindex (target);
	

	//check if the mapping already exists or it is a new mapping
	//ignore if the mapping already exists.
	if (rpladjlist[target][parent] || rpladjlist[parent][target]){
		kprintf (" DAO Message-> Mapping already exists (target - parent)%04x - %04x \n", target, parent);
		return;
	}

	rpladjlist[target][parent] = 1;
	rpladjlist[parent][target] = 1;

	//kickoff the Dijkstras shortest path calculation.
	shortestpath ();
	computepaths ();
	printpaths ();
}

void processdaomsg (struct netpacket *daomsg) {
        struct rpl_dao_msg              daomsg;
	struct rpl_opt_target           opttarget;
	struct rpl_opt_transitinf       opttransit;
	int                             pos = 0;

	uint32				target;
	uint32				parent;

	// get the target and its parent. 
	// target is present in the rpl target option
	// parent is present in the rpl transit option

        if (rpldaomsg->net_ictype != RPL_CONTROL_MSGTYPE_ICMP){
                kprintf (" This is NOT a RPL Control message %02x \n", rpldaomsg->net_ictype);
                return;
        }

        if (rpldaomsg->net_iccode != RPL_DAO_MSGTYPE){
                kprintf (" This is NOT a RPL DAO message %02x \n", rpldaomsg->net_iccode);
                return;
        }

	daomsg = (struct rpl_dao_msg *)rpldaomsg->net_icdata;
	pos += sizeof (struct rpl_dao_msg);
        if (opttarget->type != RPL_OPT_TYPE_TARGET){
                kprintf (" DAO Message: DAO message does not have an RPL OPTION TARGET %02x\n", opttarget->type);
                return;
        }


        // Decode the options here -- RPL option Target
        pos += sizeof (struct rpl_dao_msg);
        opttarget = (struct rpl_opt_target *) &rpldaomsg->net_icdata[pos];
        if (opttarget->type != RPL_OPT_TYPE_TARGET){
                kprintf (" DAO Message: DAO message does not have an RPL OPTION TARGET %02x\n", opttarget->type);
                return;
        }

	//NOTE THIS IS A 128 bit field, use only the first four bytes
	target = (uint32)opttarget->target;

        // RPL OPTION TRANSIT INFO
        pos += sizeof (struct  rpl_opt_target);
        opttransit = (struct  rpl_opt_transitinf *)&rpldaomsg->net_icdata[pos];
        if (opttransit->type != RPL_OPT_TYPE_TRANSIT){
                kprintf (" DAO Message: DAO message does not have an RPL OPTION TRANSIT INFO %02x\n", opttransit->type);
                return;
        }

	//NOTE THIS IS A 128 bit field, use only the first four bytes
	parent = (uint32) opttransit->parent;

	//compute the shortest paths if necessary
	processroute (target, parent);
}
