/* i8255x_read.c - i8255x_read */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_read - read a packet from an I8255X device
 *------------------------------------------------------------------------
 */
devcall	i8255x_read(
	struct	ether *ethptr,		/* ptr to entry in ethertab	*/
	void	*buf,			/* buffer to hold packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct	i82559_rx_desc *descptr;/* ptr to ring descriptor	*/
	char	*pktptr;		/* ptr used during packet copy	*/
	uint32	head;			/* head of ring buffer 		*/
	unsigned short status;			/* status of entry		*/
	uint32	length;			/* packet length		*/
	int32 	retval;
	//uint32 	rdt;
        byte    ru_status;                 /* Store the status of the RU unit      */

	/* Wait for a packet to arrive */

        //kprintf("Before wait in _read() ru_status[%x]\r\n", i8255x_get_ru_status(ethptr));
	wait(ethptr->isem);
        //kprintf("After wait in _read()\r\n");

	/* Find out where to pick up the packet */

	head = ethptr->rxHead; /* should iterate the whole list to find this */
	descptr = (struct i82559_rx_desc *)((char *)ethptr->rxRing + head *(I82559_RFDSIZE + ETH_BUF_SIZE));
	status = descptr->status; 

        //FIXME : We also need to check EOF and F bits here
	if (!(status & I8255X_RX_STATUS_C) || !(status & I8255X_RX_STATUS_OK)) { 	/* check for error */
		kprintf("i8255x_read: packet recieved with an error!\n");
                //printbytes(descptr, I82559_RFDSIZE+512);
                //kprintf("Ac_count is 0x%x\r\n", descptr->ac_count);
		retval = SYSERR;
	} else { 	/* pick up the packet */			
		pktptr = (char *)((uint32)((char *)descptr+I82559_RFDSIZE));
                /* Get the actual number of bytes in the buffer written by device
                 * AND the lower 16 bits with 0x3FFF to get the Actual Count (low 14 bits)
                 */
		length = (uint32)(descptr->ac_count & 0x3FFF);
		memcpy(buf, pktptr, length);
		retval = length;
	}
        //printbytes(descptr, I82559_RFDSIZE+16);
	/* Clear up the descriptor and the buffer */

        /* Setting ac_count to 0x0000 actually clears the EOF and F bits too */
	descptr->ac_count = 0x0000;
	descptr->size = ETH_BUF_SIZE;
	descptr->status = 0x0000;
        /* Set the EL and S bits in the current free RFD and clear them in the previous one */
        descptr->command = 0x4000;
        /* Clear the EL and S bits in the last RFD in which they were set */
        /*
         * SET THE EL BIT OF THE LAST RFD IN THE LIST
         * We just need to unset the S and EL bits in the previous RFD in the ring 
         * and set the S and EL bits for the current RFD 
         * So we AND the previous RFDs command word with 3FFF/0011111111111111 and OR the current RFDs command
         * word with C000 / 1100000000000000
         * FIXME : Not sure if this is right
         */
        /*
         * If we are at the beginning of the ring, we cannot do simple subtraction 
         * to reach the earlier node in the ring.
         */
        if(head== 0){
               ((struct i82559_rx_desc *)((char *)(ethptr->rxRing) + (ethptr->rxRingSize-1)*(I82559_RFDSIZE  + ETH_BUF_SIZE)))->command &= 0x3FFF;
        }
        else{
                ((struct i82559_rx_desc *)((char *)descptr - (I82559_RFDSIZE + ETH_BUF_SIZE)))->command &= 0x3FFF;
        }
	memset((char *)descptr + I82559_RFDSIZE, 
			'\0', ETH_BUF_SIZE); 

	/* Add newly reclaimed descriptor to the ring */

        /* FIXME : Not sure what this is for
	if (ethptr->rxHead % I8255X_RING_BOUNDARY == 0) {
		rdt = i8255x_io_readl(RDT(0));
		rdt = (rdt + I8255X_RING_BOUNDARY) % ethptr->rxRingSize;
		i8255x_io_writel(RDT(0), rdt);
	}
        */

	/* Advance the head pointing to the next ring descriptor which 	*/
	/*  	will be ready to be picked up 				*/
	ethptr->rxHead = (ethptr->rxHead + 1) % ethptr->rxRingSize;

        /* Restart the RU unit if it is dead */
        ru_status = i8255x_get_ru_status(ethptr);
        if((ru_status == 0x08) | (ru_status == 0x04)){
                i8255x_resume_ru(ethptr);
                //kprintf("Resuming CU, the status was [%x] now is [%x]\r\n", ru_status, i8255x_get_ru_status(ethptr)); 
        }
        else if( ru_status != I8255X_STATUS_RU_READY){
                i8255x_exec_ru(ethptr, (uint32) ethptr->rxTail);
                //kprintf("Starting RU Again, the status was : %x\r\n", ru_status);
        }

	return retval;
}
