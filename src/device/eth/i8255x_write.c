/* i8255x_write.c - i8255x_write */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_write - write a packet to an I8255X device
 *------------------------------------------------------------------------
 */
devcall	i8255x_write(
	struct	ether *ethptr, 		/* ptr to entry in ethertab 	*/
	void	*buf,			/* buffer that holds a packet	*/
	uint32	len			/* length of buffer		*/
	)
{
	struct 	i82559_tx_desc *descptr;/* ptr to ring descriptor 	*/
	char 	*pktptr; 		/* ptr used during packet copy  */
	uint32	tail;			/* index of ring buffer for pkt	*/
	byte 	status;			/*CU status*/

	/* If padding of short packet is enabled, the value in TX 	*/
	/* 	descriptor length feild should be not less than 17 	*/
	/* 	bytes */


        //kprintf("Ethernet write called\r\n");
	if (len < 17) //FIXME not sure why ?
		return SYSERR;

	/* Wait for a free ring slot */

	wait(ethptr->osem);

	/* Find the tail of the ring to insert packet */
	
	tail = ethptr->txTail;
	descptr = (struct i82559_tx_desc *)( (char *)ethptr->txRing +  tail*( I82559_TFDSIZE + ETH_BUF_SIZE));
 
	/* Copy packet to transmit ring buffer */
	
	pktptr = (char *)((uint32)descptr  + I82559_TFDSIZE);
	memcpy(pktptr, buf, len);

	/* update length in the tx command */
	descptr->byte_count = len;

        /*
         * SET THE EL BIT OF THE LAST TCB IN THE LIST
         * We just need to unset the S and EL bits in the previous TCB in the ring 
         * and set the S and EL bits for the current TCB 
         * So we AND the previous TCBs command word with 3FFF/0011111111111111 and OR the current TCBs command
         * word with C000 / 1100000000000000
         * FIXME : Not sure if this is right
         */
        /*
         * If we are at the beginning of the ring, we cannot do simple subtraction 
         * to reach the earlier node in the ring.
         */
        if(tail == 0){
               ((struct i82559_tx_desc *)(ethptr->txRing + (ethptr->txRingSize-1)*(I82559_TFDSIZE  + ETH_BUF_SIZE)))->command &= 0x3FFF;
        }
        else{
                ((struct i82559_tx_desc *)((struct i82559_tx_desc *)descptr - (I82559_TFDSIZE + ETH_BUF_SIZE)))->command &= 0x3FFF;
        }

        descptr->command |= 0xC000;         

        

	
	/* Advance the ring tail pointing to the next available ring 	*/
	/* 	descriptor 						*/
	
	ethptr->txTail = (ethptr->txTail + 1) % ethptr->txRingSize;
	
	//FIXME Is it to be done here?
	status = i8255x_get_cu_status(ethptr);
	if( (status != I8255X_STATUS_CU_LPQA) || (status != I8255X_STATUS_CU_HQPA) ){
		i8255x_exec_cu( ethptr, tail*(I82559_TFDSIZE + ETH_BUF_SIZE));
                //printbytes(descptr, I82559_TFDSIZE);
                //kprintf("exec_cu done, tail is %d\r\n", tail);
	}
        //kprintf("returning from _write()\r\n");
        /* 
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, TRUE));
        kprintf("Number of packets received : %d\r\n",i8255x_rx_count(ethptr, FALSE));
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, FALSE));
        */
	return len;
}
