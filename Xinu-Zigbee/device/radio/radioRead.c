/* radioRead.c  - radioRead */

#include <xinu.h>

/*------------------------------------------------------------------------
 * radioRead - read a packet from an Radioernet device
 *------------------------------------------------------------------------
 */
devcall	radioRead (
	struct	dentry	*devptr,	/* entry in device switch table	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
		)
{
	volatile packet_t *p;
	struct radio *rptr;
	int i;
	char *from, *to;

	rptr = &radiotab[devptr->dvminor];

	i = 0;
	while (i == 0) {
		/* Wait on input packet queue semaphore */
		wait(rptr->isem);

		/* Get packet form queue and confirm packet OK */
		if ((p = rx_packet())) {
			/* Copy payload to caller buffer */
			/* p->data[0] == number of bytes in payload */
			/* Should check input parameter len to confirm caller
			 * buffer has room for payload
			 */
			*(char *)buf++ = p->lqi;
			for (i = 0, from = (char *)&p->data[1], to = (char *)buf;
			     i < p->length; i++)
				*to++ = *from++;
			free_packet(p);
		}
	}
	return (i + 1);	/* Include LQI byte */
}
