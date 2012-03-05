/* radioWrite.c  - radioWrite */

#include <xinu.h>
#include <packet.h>
#include <maca.h>

/*------------------------------------------------------------------------
 * radioWrite - write a packet from an Radioernet device
 *------------------------------------------------------------------------
 */
devcall	radioWrite (
	struct	dentry	*devptr,	/* entry in device switch table	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
		)
{
	volatile packet_t *p;
	struct radio *rptr;
	uint32 i;
	char *from, *to;

	rptr = &radiotab[devptr->dvminor];

	/*
	 * Return immediately if a buffer is available
	 */

	p = get_free_packet();

	if(p) {
		kprintf("radioWrite preparing 0x%08x\n", p);
		p->length = len;
		p->offset = 0;
		for (from = buf, to = (char *)p->data, i = 0; i < len; i++)
		  *to++ = *from++;
		//kprintf("radioWrite semcount %d\n", semcount(rptr->osem));
		//wait(rptr->osem);
		tx_packet(p);
		return len;
	} else {
        	kprintf("radioWrite: can't get free packet\n");
		return SYSERR;
	}
}
