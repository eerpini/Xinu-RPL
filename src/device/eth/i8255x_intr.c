/* i8255x_intr.c - i8255x_intr */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_rxPackets - handler for receiver interrupts
 *------------------------------------------------------------------------
 */
void 	i8255x_rxPackets(
	struct 	ether 	*ethptr 	/* ptr to control block		*/
	)
{
	struct	i82559_rx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32	tail;			/* pos to insert next packet	*/
	unsigned short status;			/* status of ring descriptor 	*/
	int numdesc; 			/* num. of descriptor reclaimed	*/

        //kprintf("Packet received, rxInterrupt\r\n");

	for (numdesc = 0; numdesc < ethptr->rxRingSize; numdesc++) {

		/* Insert new arrived packet to the tail */
                if(numdesc >0 && ethptr->rxTail == ethptr->rxHead)
                        break;
		tail = ethptr->rxTail;
		descptr = (struct i82559_rx_desc *)((char *)ethptr->rxRing + tail*(I82559_RFDSIZE + ETH_BUF_SIZE));
		status = descptr->status;
                
                /*
                kprintf("Number of packets received : %d tail %d head %d numdesc %d ru_status %x RFD Status %x RFD Command %x semvalue : %d\r\n",
                                i8255x_rx_count(ethptr, TRUE), ethptr->rxTail, ethptr->rxHead, numdesc, i8255x_get_ru_status(ethptr), descptr->status, descptr->command, semtab[ethptr->isem].scount+numdesc);

                                */
                //kprintf("%d\t", semtab[ethptr->isem].scount+numdesc);
                if(semtab[ethptr->isem].scount+numdesc == ethptr->rxRingSize-1){
                        //kprintf("WARN: rxRing is Full\r\n");
                }
                //printbytes((char *)(descptr), I82559_RFDSIZE + 32);
		if (status == 0) {
                        //kprintf("Got status 0, breaking out here, tail[%d] head[%d] ru_status[%x]\r\n", ethptr->rxTail, ethptr->rxHead, i8255x_get_ru_status(ethptr));
                        //kprintf("Printing some details %u should be what RU has %u \r\n", (char *)ethptr->rxRing+ethptr->rxTail*(I82559_RFDSIZE + ETH_BUF_SIZE), 
                                        //(char *)ethptr->rxRing+((struct i82559_rx_desc *)((char *)ethptr->rxRing + (tail-1)*(I82559_RFDSIZE + ETH_BUF_SIZE)))->link_addr);
			break;
		}

                /*
                if((ethptr->rxTail + 1)%ethptr->rxRingSize == ethptr->rxHead)
                        break;
                        */
		ethptr->rxTail 
			= (ethptr->rxTail + 1) % ethptr->rxRingSize;
	}
        
        /*
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, TRUE));
        kprintf("Number of packets received : %d\r\n",i8255x_rx_count(ethptr, FALSE));
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, FALSE));
        */
	signaln(ethptr->isem, numdesc);

        //kprintf("I82559ER_RECV_INTR Received a packet %d\r\n", numdesc);

	return;
}

/*------------------------------------------------------------------------
 * i8255x_txPackets - handler for transmitter interrupts
 *------------------------------------------------------------------------
 */
void 	i8255x_txPackets(
	struct	ether *ethptr		/* ptr to control block		*/
	)
{
	struct	i82559_tx_desc *descptr;/* ptr to ring descriptor 	*/
	uint32 	head; 			/* pos to reclaim descriptor	*/
	char 	*pktptr; 		/* ptr used during packet copy  */
	int 	numdesc; 		/* num. of descriptor reclaimed	*/

        //kprintf("Packet Transferred, txInterrupt\r\n");

	for (numdesc = 0; numdesc < ethptr->txRingSize; numdesc++) {
		head = ethptr->txHead;
		descptr = (struct i82559_tx_desc *)( (char *)ethptr->txRing +  head * ( I82559_TFDSIZE + ETH_BUF_SIZE));
		
		
		if((descptr->status & I8255X_TX_STATUS_C) == 0){
                        //kprintf(" Breaks here , the head of the txRing is %d and status is %x\r\n", head, descptr->status);
			break;
                //FIXME
		}else if( (descptr->status & I8255X_TX_STATUS_OK) <= 0){
			/*Transmision completed with error*/
			kprintf("WARN: Packet sent with an error\n\r");
		}
		

		/* Clear the write-back descriptor and buffer */

		descptr->byte_count = 0;
		pktptr = (char *)((uint32)descptr + I82559_TFDSIZE);
		memset(pktptr, '\0', ETH_BUF_SIZE);

		ethptr->txHead 
			= (ethptr->txHead + 1) % ethptr->txRingSize;
                //kprintf("Iteration %d in tx_intr()\r\n", numdesc);
	}
        /*
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, TRUE));
        kprintf("Number of packets received : %d\r\n",i8255x_rx_count(ethptr, FALSE));
        kprintf("Number of packets transferred : %d\r\n",i8255x_tx_count(ethptr, FALSE));
        */
	signaln(ethptr->osem, numdesc);
        //kprintf("Signalled and done with txInterrupt : %d\r\n", numdesc);
	return;
}


/*------------------------------------------------------------------------
 * i8255x_intr - decode and handle interrupt from an I8255X device
 *------------------------------------------------------------------------
 */
interrupt i8255x_intr(
	struct 	ether *ethptr
	)
{
	unsigned short ack = 0x0000; /*To store the interrupt acknowledge byte*/
	unsigned short	status;

	/* Disable device interrupt */
	i8255x_irq_disable(ethptr);

	/* Obtain status bits from device */
	status  = inw(ethptr->iobase + I8255X_SCB_STATUS);
        //kprintf("Status of the interrupt : %x\r\n", status);
        /* FIXME : Handle the software interrupt better */



	status  &= I8255X_INTR_ENABLE_MASK; 
	
	/* Not our interrupt */
	if( status == 0 ){
                //kprintf("Not our interrupt %d\r\n", status);
		i8255x_irq_enable(ethptr);
		return;
	}

	sched_cntl(DEFER_START);

        if(status & 0x0400){
                //kprintf("Got a software interrupt, acknowledging and ignoring\r\n");
                ack |= 0x0400;
        }

        if(status & 0x1000){
                //kprintf("RECEIVED A RNR INTERRUPT\r\n");
                ack |= 0x1000;
        }


	if( (status & I8255X_INTR_CNA_MASK)  > 0 ){
	/*indicates that CU left the active state*/
		//kprintf("NOTE: CU has left the active state! \n\r");
		ack |= I8255X_INTR_CNA_MASK;
	}
	
	if((status & I8255X_INTR_FR_MASK)  > 0 ){
	/*indicates a frame is ready to be picked up*/
		ethptr->rxIrq++;
		i8255x_rxPackets(ethptr);
		ack |= I8255X_INTR_FR_MASK;

	}
	
	if( (status & I8255X_INTR_CX_MASK) > 0 ){
		ethptr->txIrq++;
		i8255x_txPackets(ethptr);
		ack |= I8255X_INTR_CX_MASK;
		
	}
	
	/*acknowledge the serviced interrupts*/
	outw(ethptr->iobase + I8255X_SCB_STATUS, ack);

	/* Enable device interrupt */
	i8255x_irq_enable(ethptr);
	
	sched_cntl(DEFER_STOP);

	return;
}
