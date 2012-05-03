#include <xinu.h>

void encodedio(struct icmpv6_sim_packet *rpldiomsg) {

	struct 	rpl_dio_msg		diomsg;
	//struct	rpl_opt_routeinfo	rtinfo;
	struct	rpl_opt_dodag_conf	dagconf;
	int				len = 0;

#ifdef DEBUG
        kprintf("Encoding DIO Message \r\n");
#endif
	diomsg.rpl_instance_id 	= RPL_DEFAULT_INSTANCE;
	diomsg.version		= RPL_MYINFO.version; 
	diomsg.rank		= RPL_MYINFO.rank;
	diomsg.grounded_mop_pref = 0;

	diomsg.dtsn		 = RPL_MYINFO.dtsn;
	diomsg.flags		 = 0;
	diomsg.reserved		 = 0;

	memcpy (diomsg.dodagid, RPL_MYINFO.dodagid, RPL_DODAGID_LEN);
#ifdef DEBUG
        kprintf("The dodagid being sent is : [%04x]\r\n", *((uint32 *)(diomsg.dodagid)));
#endif
	
	//encode options here
	//Route information
	/*rtinfo.type	 	= RPL_OPT_TYPE_ROUTEINFO;
	rtinfo.len		= sizeof (struct rpl_opt_routeinfo);
	rtinfo.prefix_len	= RPL_MAX_PREFIX_LEN;
	rtinfo.reserved		= 0;
	rtinfo.route_lifetime	= RPL_MAX_ROUTE_LIFETIME;
	memcpy (rtinfo.prefix, RPL_MYINFO.mydodagid, RPL_MAX_PREFIX_LEN);
	*/

	//DAG Configuration
	dagconf.type		= RPL_OPT_TYPE_DODAG_CONF;
	dagconf.len		= sizeof (struct rpl_opt_dodag_conf);
	dagconf.flags		= 0;
	dagconf.diointdouble	= RPL_MYINFO.diointdouble;
	dagconf.diointmin	= RPL_MYINFO.diointmin;
	dagconf.dioredundancy 	= RPL_MYINFO.dioredundancy;
	dagconf.maxrankincrease = RPL_MYINFO.maxrankincrease;
	dagconf.minhoprankinc	= RPL_MYINFO.minhoprankinc;
	dagconf.ocp		= 0;
	dagconf.reserved	= 0;
	dagconf.lifetime	= RPL_MYINFO.pathlifetime;
	dagconf.lifetime_unit	= RPL_OPT_LIFETIME_UNIT;

	/*	Encode the diomsg into the ICMP packet here 	*/
	rpldiomsg->net_ictype = 155;
	rpldiomsg->net_iccode = RPL_DIO_MSGTYPE;
	rpldiomsg->net_iccksum = 0;

	len += sizeof (struct rpl_dio_msg);
	memcpy (rpldiomsg->net_icdata, &diomsg, sizeof (struct rpl_dio_msg));

	//copy options here
	//DAG Configuration
	memcpy (&rpldiomsg->net_icdata[len], &dagconf, sizeof (struct rpl_opt_dodag_conf));
#ifdef DEBUG
        kprintf("Encoding DIO Message done\r\n");
#endif
}


void processdio (struct icmpv6_sim_packet *rpldiomsg, uint32 sender, byte *parent_changed) {

	struct 	rpl_dio_msg		*diomsg;
	struct	rpl_opt_dodag_conf	*dagconf;
	int				pos = 0;
        int i;

#ifdef DEBUG
        kprintf(" Inside processdio \r\n");
#endif
	if (rpldiomsg->net_iccode ==  RPL_DIO_MSGTYPE) {
		diomsg = (struct rpl_dio_msg *) rpldiomsg->net_icdata;
                if(RPL_MYINFO.rank == 0){
                        RPL_MYINFO.rank = diomsg->rank + 1;
#ifdef DEBUG
                        kprintf("My Rank is 0 and I set it to : [%d]\r\n", RPL_MYINFO.rank);
#endif
                        //set sender as parent
                }
                else if(RPL_MYINFO.rank > diomsg->rank + 1){

			//base message
			RPL_MYINFO.rank = diomsg->rank + 1;
#ifdef DEBUG
                        kprintf("My Rank is greater than the senders [%d] and I set it to : [%d]\r\n", diomsg->rank, RPL_MYINFO.rank);
#endif
                        //set sender as parent
                }
                else{
#ifdef DEBUG
			kprintf (" Ignoring the dio message, since the rank need not be changed\r\n");
#endif
                        *parent_changed = 0;
                        return ;
                }
                /*
                 * Setting the sender as our parent
                 */
                if(RPL_MYINFO.parent_index >= 0){
                        rpl_link_local_neighbors[RPL_MYINFO.parent_index].is_parent = 0;
                        RPL_MYINFO.parent_index = -1;
                }
                for(i=0; i<LOWPAN_MAX_NODES; i++){
                       if(rpl_link_local_neighbors[i].iface_addr == sender){
                              rpl_link_local_neighbors[i].is_parent = 1;
                              break;
                       }
                }
                if(i >= LOWPAN_MAX_NODES){
#ifdef DEBUG
                        kprintf("We could not locate the sender [%04x] to set it as parent, this should not HAPPEN\r\n", sender);
#endif
                        return;
                }
                else{
                        RPL_MYINFO.parent_index = i;
                        *parent_changed = 1;
                }
                        
                
                RPL_MYINFO.dtsn = diomsg->dtsn;
                memcpy(RPL_MYINFO.dodagid, diomsg->dodagid, RPL_DODAGID_LEN);

                //Check for the options here
                pos += sizeof (struct rpl_dio_msg);
                if (rpldiomsg->net_icdata[pos] == RPL_OPT_TYPE_DODAG_CONF) {
                        dagconf = (struct rpl_opt_dodag_conf *) &rpldiomsg->net_icdata[pos];

                        RPL_MYINFO.diointdouble = dagconf->diointdouble;
                        RPL_MYINFO.diointmin = dagconf->diointmin;
                        RPL_MYINFO.dioredundancy = dagconf->dioredundancy;
                        RPL_MYINFO.maxrankincrease = dagconf->maxrankincrease;
                        RPL_MYINFO.minhoprankinc = dagconf->minhoprankinc;
                        RPL_MYINFO.pathlifetime = dagconf->lifetime;
#ifdef DEBUG
                        kprintf("The dodagid was set to : [%04x]\r\n", *((uint32 *)(RPL_MYINFO.dodagid)));
#endif
                } else {
                        kprintf ("WARN : Unexpected option in the DIO Message --> %02x <--\r\n", rpldiomsg->net_icdata[pos]);
                }

	} else {
		kprintf ("WARN : This message is NOT DIO message \r\n");
	}
}

void decodedio(struct icmpv6_sim_packet *rpldiomsg) {

	struct 	rpl_dio_msg		*diomsg;
	struct	rpl_opt_dodag_conf	*dagconf;
	int				pos = 0;

	//1. icmp type
#ifdef DEBUG
	kprintf (" ICMP Message Type %02x \n", rpldiomsg->net_ictype);
	kprintf (" ICMP Code %02x \n", rpldiomsg->net_iccode);
	kprintf (" ICMP Checksum %04x \n", rpldiomsg->net_iccksum);
#endif

	if (rpldiomsg->net_iccode ==  RPL_DIO_MSGTYPE) {
		diomsg = (struct rpl_dio_msg *) rpldiomsg->net_icdata;

#ifdef DEBUG
		kprintf(" DIO Msg Inst ID %02x \n", diomsg->rpl_instance_id);
		kprintf (" DIO Msg Version %02 \n", diomsg->version);
		kprintf (" DIO Msg Rank %02x \n", diomsg->rank);
		kprintf (" DIO Msg g/m/p %02x \n", diomsg->grounded_mop_pref);
		kprintf (" DIO Msg DTSN %04x \n", diomsg->dtsn);
		kprintf (" DIO Msg flags %02x \n", diomsg->flags);
		kprintf (" Dio Msg Reserved %02x \n", diomsg->reserved);

		kprintf (" DIO Msg dodagid %04x: %04x: %04x %04x \n", 
			(uint32)diomsg->dodagid, (uint32)diomsg->dodagid[4], (uint32)diomsg->dodagid[8], (uint32)diomsg->dodagid[12]);
#endif

		//check for the options here.
		pos += sizeof (struct rpl_opt_dodag_conf);
		dagconf = (struct rpl_opt_dodag_conf *)&rpldiomsg->net_icdata[pos];

#ifdef DEBUG
		kprintf (" DIO Msg Type %02x \n", dagconf->type);
		kprintf (" DIO Msg Len %02x \n", dagconf->len);
		kprintf (" DIO Msg Flags %02x \n", dagconf->flags);
		kprintf (" DIO Msg dio intravel %04x \n", dagconf->diointdouble);
		kprintf (" DIO Msg dio int min %04x \n", dagconf->diointmin);
		kprintf (" DIO Msg dio red %04x \n", dagconf->dioredundancy );
		kprintf (" DIO Msg rank increase %04x\n", dagconf->maxrankincrease);
		kprintf (" DIO Msg min hop rank %04x \n", dagconf->minhoprankinc);
		kprintf (" DIO Msg OCP %04x\n", dagconf->ocp);
		kprintf (" DIO Msg reserved %02x \n", dagconf->reserved);
		kprintf (" DIO Msg lifetime %04x \n", dagconf->lifetime);
		kprintf (" DIO Msg lifetime unit %04x \n", dagconf->lifetime_unit);
#endif

	} else {
		kprintf ("WARN : This is not a DIO message \n");
	}
}
