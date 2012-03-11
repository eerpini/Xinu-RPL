/* netiface.c - netiface_init */

#include <xinu.h>

/* Functions used to initialize interfaces and create an input process	*/
/*	for each interface						*/

struct	ifentry	if_tab[NIFACES];	/* Array of network interfaces	*/

IP6addr ula_prefix = {0xFD, 0x44, 0x52, 0x4f, 0x40, 0x53, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

IP6addr ll_prefix =  {0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

extern uint8 unspec[16];

/*------------------------------------------------------------------------
 * netiface_init - initialize the network interface data structures
 *------------------------------------------------------------------------
 */
void	netiface_init (void) {

	struct	ifentry	*ifptr = &if_tab[0];	/* ptr to interface table entry	*/

	/* Initialize interface data structures */


	ifptr->if_state = IF_DOWN;	/* interface is down	*/
	ifptr->if_ip6valid = FALSE;	/* IP fields are invalid*/

	/* radio address */
	memcpy(ifptr->if_macucast, radiotab[0].radio_long_addr, 8);

	/* ULA prefix */
	memcpy(ifptr->if_ip6prefix, ula_prefix, 16);

	/* Build global address from ULA prefix */
	memcpy(ifptr->if_ip6ucast[0], ifptr->if_ip6prefix, 8);
	memcpy(&ifptr->if_ip6ucast[0][8], radiotab[0].radio_long_addr, 8);

	/* Build link-scoped address */
	memcpy(ifptr->if_ip6ucast[1], ll_prefix, 8);
	memcpy(&ifptr->if_ip6ucast[1][8], radiotab[0].radio_long_addr, 8);

	/* No default router */
	memcpy(ifptr->if_ip6router, unspec, 16);

	/* create the input buffer queue synchronization semaphore */

	ifptr->if_isem = semcreate(0);	/* no packets in buffer	*/
	if (ifptr->if_isem == SYSERR) {
		panic("netiface_init: cannot create interface input queue semaphore");
	}

	/* head and tail of queue are at 0 */
	ifptr->if_ihead = ifptr->if_itail = 0;

	/* buffer holds a netpacket */
	ifptr->if_ipool = mkbufpool(sizeof(struct netpacket), 3);

	/* create the buffer synchronization semaphore */

	ifptr->if_osem = semcreate(0);	/* no packets in buffer	*/
	if (ifptr->if_osem == SYSERR) {
		panic("netiface_init: cannot create semaphore");
	}

	/* head and tail of queue are at 0 */
	ifptr->if_ohead = ifptr->if_otail = 0;

	/* buffer holds a netpacket */
	ifptr->if_opool = mkbufpool(sizeof(struct netpacket), 3);

	/* Enable radio interface */

	ifptr->if_state = IF_UP;
	ifptr->if_ip6valid = TRUE;
	return;
}
