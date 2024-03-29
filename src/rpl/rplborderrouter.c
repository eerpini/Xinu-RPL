#include <xinu.h>
#ifdef LOWPAN_BORDER_ROUTER

byte		rplpath[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];
byte		rpladjlist[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];
byte		iface_freshness[LOWPAN_MAX_NODES];

int getindex (uint32 addr) {
	int index = 0;
	for (index = 0; index < LOWPAN_MAX_NODES; index ++) {
		if (iface_addr[index] == addr)
			return index;
	}

	return SYSERR;
}

uint32	getaddress (uint32 index) {
	
	return iface_addr[index];
}

int assignindex (uint32 addr) {
	int index = 0;

	for (index = 0; index < LOWPAN_MAX_NODES; index ++) {
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
	pindex = getindex (parent);
	if (pindex == SYSERR) {
#ifdef DEBUG
		kprintf (" DAO Message-> Parent does not exist in the map %05x \n", parent);
#endif
		return;
	}
        else{
#ifdef DEBUG
                kprintf("The index returned for parent : [%04x] is %d\r\n", parent, pindex);
#endif
        }


        tindex = getindex (target);
	if (tindex == SYSERR) {
		tindex = assignindex (target);
#ifdef DEBUG
                kprintf("Target [%04x] not already present, assigned the index : %d\r\n", target, tindex);
#endif
        }
        else{
#ifdef DEBUG
                kprintf("The index returned for target : [%04x] is %d\r\n", target, tindex);
#endif
        }
	

	//Update the freshness of the path here. 
#ifdef DEBUG
        kprintf("INFO : Incrmenting the freshness[%d] for index [%d]\r\n", iface_freshness[tindex], tindex);
#endif
	iface_freshness[tindex] = (iface_freshness[tindex] +1) ;

	//check if the mapping already exists or it is a new mapping
	//ignore if the mapping already exists.
	if (rpladjlist[tindex][pindex] || rpladjlist[pindex][tindex]){
#ifdef DEBUG
		kprintf (" DAO Message-> Mapping already exists (target - parent)%04x - %04x \n", target, parent);
#endif
		return;
	}

	rpladjlist[tindex][pindex] = 1;
	rpladjlist[pindex][tindex] = 1;

	//kickoff the Dijkstras shortest path calculation.
	shortestpath ();
	computepaths ();
	printpaths ();
}

void processdao(struct icmpv6_sim_packet *rpldaomsg) {
        //struct rpl_dao_msg              daomsg;
	struct rpl_opt_target           *opttarget;
	struct rpl_opt_transitinf       *opttransit;
	int                             pos = 0;

	uint32				target;
	uint32				parent;
	uint32				mask;

	// get the target and its parent. 
	// target is present in the rpl target option
	// parent is present in the rpl transit option

        if (rpldaomsg->net_ictype != RPL_CONTROL_MSGTYPE_ICMP){
#ifdef DEBUG
                kprintf (" This is NOT a RPL Control message %02x \n", rpldaomsg->net_ictype);
#endif
                return;
        }

        if (rpldaomsg->net_iccode != RPL_DAO_MSGTYPE){
#ifdef DEBUG
                kprintf (" This is NOT a RPL DAO message %02x \n", rpldaomsg->net_iccode);
#endif
                return;
        }

	// Decode the options here -- RPL option Target
        pos += sizeof (struct rpl_dao_msg);
        opttarget = (struct rpl_opt_target *) &rpldaomsg->net_icdata[pos];
        if (opttarget->type != RPL_OPT_TYPE_TARGET){
#ifdef DEBUG
                kprintf (" DAO Message: DAO message does not have an RPL OPTION TARGET %02x\n", opttarget->type);
#endif
                return;
        }

	//NOTE THIS IS A 128 bit field, use only the first four bytes
	target = *((uint32 *)(opttarget->target));

#ifdef DEBUG
        kprintf("***********PROCESSING DAO FROM %04x\r\n", target);
#endif

        // RPL OPTION TRANSIT INFO
        pos += sizeof (struct  rpl_opt_target);
        opttransit = (struct  rpl_opt_transitinf *)&rpldaomsg->net_icdata[pos];
        if (opttransit->type != RPL_OPT_TYPE_TRANSIT){
#ifdef DEBUG
                kprintf (" DAO Message: DAO message does not have an RPL OPTION TRANSIT INFO %02x\n", opttransit->type);
#endif
                return;
        }

	//NOTE THIS IS A 128 bit field, use only the first four bytes
	parent = *((uint32 *)( opttransit->parent));

#ifdef DEBUG
        kprintf("Calling process route with parent : [%04x] target : [%04x]\r\n", parent, target);
#endif
        

	//compute the shortest paths if necessary
	mask = disable ();
	processroute (target, parent);
	restore (mask);
}

void processPathlifetimeTimeout () {

	//1.  check if the freshness is more than 0, if yes reset it to 0
	//   else invalidate the path and recompute the paths if necessary
	
	byte	recompute = 0;
	int	index = 0, j = 0;

#ifdef DEBUG
	kprintf (" In the %s --> \n\r", __FUNCTION__);
#endif

        /*
         * NOTE : We only check the freshness for the nodes in the 
         * RPL Network and not for the other nodes
         */
	for (index = 1; index < LOWPAN_MAX_NODES; index ++){
		if (iface_freshness[index] != 0) {
			iface_freshness[index] = 0;
		} else {
			//invalidate here.
			iface_freshness[index] = 0;
			recompute = 1;
#ifdef DEBUG
			kprintf (" %s: did not receive DAO update from node %04x \n\r", __FUNCTION__, getaddress(index));
#endif

			for (j = 0; j < LOWPAN_MAX_NODES; j ++) {
				rpladjlist[index][j] = 0;
				rpladjlist[j][index] = 0;
			}
		}
	}

	if (recompute) {
		shortestpath ();
		computepaths ();
		printpaths ();
	}
}
#endif
