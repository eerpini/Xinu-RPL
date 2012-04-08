
#include <xinu.h>

status	icmp_in(void){

        struct ipv4_packet *ippkt = (struct ipv4_packet *)(currpkt->net_ethdata);
        struct icmp_packet *icmppkt = (struct icmp_packet *)(ippkt->net_ipdata);
	
	icmp_ntoh(icmppkt);

	//kprintf("recd a ICMP packet\n\r");
	if(icmppkt->net_ictype == ICMP_ECHOREQST){
		icmppkt->net_ictype = ICMP_ECHOREPLY;
	}else{
		kprintf("Not Handled\n\r");
		return OK;
	}
	uint32 temp_ip;
	byte temp_mac[ETH_ADDR_LEN];

	/*swap ip-addresses in packet*/
	temp_ip = ippkt->net_ipsrc;
	ippkt->net_ipsrc = ippkt->net_ipdst;
	ippkt->net_ipdst = temp_ip;

	/*swap mac addresses in packet*/
	memcpy(temp_mac, currpkt->net_ethsrc, ETH_ADDR_LEN);
	memcpy(currpkt->net_ethsrc, currpkt->net_ethdst,  ETH_ADDR_LEN);
	memcpy(currpkt->net_ethdst, temp_mac, ETH_ADDR_LEN);

	

	icmp_hton(icmppkt);
	ip_hton(ippkt);
	eth_hton(currpkt);
	
	//kprintf("Sending ICMP response..\n\r");
	//pdump(currpkt);
	write(ETHER0, (char *)currpkt, sizeof(struct netpacket));

	return OK;
}

/*------------------------------------------------------------------------
 * icmp_ntoh - convert ICMP header fields from net to host byte order
 *------------------------------------------------------------------------
 */
void 	icmp_ntoh(
	struct icmp_packet *pktptr
	)
{
	pktptr->net_icident = ntohs(pktptr->net_icident);
	pktptr->net_icseq = ntohs(pktptr->net_icseq);
	return;
}

/*------------------------------------------------------------------------
 * icmp_hton - convert packet header fields from host to net byte order
 *------------------------------------------------------------------------
 */
void 	icmp_hton(
	struct icmp_packet *pktptr
	)
{
	pktptr->net_icident = htons(pktptr->net_icident);
	pktptr->net_icseq = htons(pktptr->net_icseq);
	return;
}
