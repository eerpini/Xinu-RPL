/* i8255x_open.c - i8255x_oepn, i8255x_irq_disable, i8255x_irq_enable */

#include <xinu.h>

#define RETRIES 20

/* allocate descriptor ring statically */

local 	status 	i8255x_reset(struct ether *ethptr);
local 	status 	i8255x_configure(struct ether *ethptr);
local 	status 	i8255x_init_hw(struct ether *ethptr);
local 	status 	i8255x_reset_hw(struct ether *ethptr);
local 	void 	i8255x_configure_rx(struct ether *ethptr);
local 	void 	i8255x_configure_tx(struct ether *ethptr);

/*------------------------------------------------------------------------
 * i8255x_open - allocate resources and prepare hardware for transmit and 
 *                receive
 *------------------------------------------------------------------------
 */
status i8255x_open(
	struct 	ether 	*ethptr
	)
{
	struct	i82559_tx_desc* txRingptr;
	struct	i82559_rx_desc*	rxRingptr;
	int32	i;
//	uint32  bufptr;

	

	/* Initialize structure pointers */

	ethptr->rxRingSize = I8255X_RX_RING_SIZE;
	ethptr->txRingSize = I8255X_TX_RING_SIZE;
	ethptr->isem = semcreate(0);
	ethptr->osem = semcreate(ethptr->txRingSize);
	
	 /*
	  * Rings must be aligned on a 4-byte boundary for I82559 
	  * Only Simplified Frame descriptors can be used, as that is the only one supported.
	  *
	  */
	
	
	ethptr->stats	= getmem(I82559_DUMP_STATS_SIZE + 4); /*4 extra bytes assigned to make sure the address is dword alligned*/
	ethptr->rxRing = (void *)getmem((ethptr->rxRingSize + 1)
			* (  I82559_RFDSIZE + ETH_BUF_SIZE ) );
	ethptr->txRing = (void *)getmem((ethptr->txRingSize + 1)
			* (  I82559_TFDSIZE + ETH_BUF_SIZE ));

	if ( (SYSERR == (uint32)ethptr->rxRing) || 
	     (SYSERR == (uint32)ethptr->txRing) || SYSERR == (uint32) ethptr->stats ) {
		//kprintf("i8255x_open: fail to allocate tx/rx Bufs\n\r");
		return SYSERR;
	}

	
	/*16-byte allignment. Not needed, but to be on safe side*/
	ethptr->rxRing = (void *)(((uint32)ethptr->rxRing + 0xf) & ~0xf);
	ethptr->txRing = (void *)(((uint32)ethptr->txRing + 0xf) & ~0xf);
	ethptr->stats = (void *) (((uint32)ethptr->stats + 3) & ~0x3); /*D-word alligned*/

	ethptr->rxBufs = NULL;
	ethptr->txBufs = NULL;
	
        //kprintf("The rings are tx : %u rx: %u \r\n", ethptr->txRing, ethptr->rxRing);
	

	/* Set buffer pointers and rings to zero */
	
	memset(ethptr->rxRing, '\0', ( I82559_RFDSIZE + ETH_BUF_SIZE )* ethptr->rxRingSize);
	memset(ethptr->txRing, '\0', ( I82559_TFDSIZE + ETH_BUF_SIZE )* ethptr->txRingSize);
	
	
	txRingptr = (struct i82559_tx_desc *)ethptr->txRing;
	for(i=0; i < ethptr->txRingSize; ++i ){
                txRingptr = (struct i82559_tx_desc *)((char *)(ethptr->txRing)+i*(I82559_TFDSIZE + ETH_BUF_SIZE));
		/*NOTE: The following statment assumes that segmented addressing mode is used*/
                /* Set the link to the next CB  in the ring */
		txRingptr->link_addr = ((i+1)%ethptr->txRingSize)*(I82559_TFDSIZE + ETH_BUF_SIZE);
                /* Set the entire status word to 0x0000 , FIXME, check if this is OK    */
                txRingptr->status = 0x0000;
		/**
                 * Command Word Description(from Bit 16-31) :
                 * Command = 100 (Transmit Command)
                 * SF = 0  (Simplified Mode)
                 * NC = 0  (CRC and address inserted by device)
                 * 000  (Reserved and set to zeros)
                 * CID = 00000 (Delay for CNA Interrupt, set to 0 for now, TODO more work required)
                 * I = 1 (Generate interrupt after the execution of CB is complete)
                 * S = 1 (Put CU to sleep after execution of CB and more .. Check manual)
                 * EL = 1 (Indicates this CB is the last one in the CBL, is set to 1 for all CBs initially)
                 * Command Word : 1110000000000100 / E004
		 */
		//txRingptr->command = 0xA030;
		txRingptr->command = 0xE004;
		txRingptr->tbd_addr = 0xFFFFFFFF;	
                /* Transmit threshold, represents the number of bytes in the 
                 * transmit FIFO for transmission to happen, is multiplied by 
                 * 8 to give the bytes, should be between 1 and 0E0h
                 */
                /* 16 bytes required in the trasnmit FIFO */
		txRingptr->tx_treshold = 0x02;
                /* This is not used in the simplified structure, set it to 0, TBD number */
		txRingptr->tbd_no = 0x00;
		/*These parameters should be changed when packet is ready to be transmitted
		 * Also, EL bit should be unset*/
		txRingptr->byte_count = 0; 
	}

        /* RFA initialization */

        rxRingptr = (struct i82559_rx_desc *)ethptr->rxRing;
        for(i=0; i < ethptr->rxRingSize; ++i){
                rxRingptr = (struct i82559_rx_desc *)((char *)(ethptr->rxRing)+i*(I82559_RFDSIZE + ETH_BUF_SIZE));
                /* Set the pointer to the next Receive frame.*/
                rxRingptr->link_addr = ((i+1)%ethptr->rxRingSize)*(I82559_RFDSIZE + ETH_BUF_SIZE);
                //kprintf("Set link address to %u for [%d]\r\n", rxRingptr->link_addr, i);
                
                /* Set the entire status word to 0x0000 since this is set by the device later FIXME*/
                rxRingptr->status = 0x0000;
                /*
                 * Command Word Description( from Bit 16-31):
                 * Command = 000 (command opcode for receive)
                 * SF = 0 (for simplified mode)
                 * H = 0 (decides if the RFD is a header RFD or not, is disregarded
                   if load HDS was not run before
                 * 000000000  (Reserved)
                 * S = 0 ( Suspend the RU after receiving the frame)
                 * EL = 0 (Indicates this is the last RFD in the RFA, set to 0 for all RFDs initially,
                   except the last RFD)
                 * Command Word : 1000000000000000 / 8000
                 */
                rxRingptr->command = 0x0000;
                /*FIXME : Change these later */
                /* Set the actual count to 0x0000 (this will set the EOF and F bits too) */
                rxRingptr->ac_count = 0x0000;
                /*
                 * Set the size to ETH_BUF_SIZE Later
                 */
                rxRingptr->size = ETH_BUF_SIZE;
        }
        /*
         * For the last RFD in the ring, we need to set the EL Bit and the S bit for the last block
         * this changes the command word to 0xC000
         */
        rxRingptr->command = 0x4000;

	/* Reset the NIC to bring it into a known state and initialize it */

	if (OK != i8255x_reset(ethptr)) {
		//kprintf("i8255x_open: fail to reset I8255X device\n");
		return SYSERR;
	}

	/* Configure the NIC */

	if (OK != i8255x_configure(ethptr)) {
		//kprintf("i8255x_open: fail to configure I8255X device\n");
		return SYSERR;
	}

	/* Enable interrupt */
	
	set_evec(ethptr->dev->dvirq + IRQBASE, (uint32)ethDispatch);
        //kprintf("Calling the first IRQ Enable \r\n");
        //kprintf("The interrupt mask is : %x\r\n", inw(ethptr->iobase +I8255X_SCB_COMMAND_LOW)); 
	i8255x_irq_enable(ethptr);

        /* Start the receive unit */
        i8255x_exec_ru(ethptr, (uint32) ethptr->rxHead*(I82559_RFDSIZE + ETH_BUF_SIZE)); 

        //kprintf("All user processes have completed \r\n");
	return OK;
}

/*------------------------------------------------------------------------
 * i8255x_reset - bring the hardware into a known good state
 *------------------------------------------------------------------------
 */
local status i8255x_reset(
	struct ether *ethptr
	)
{
	/* reset Packet Buffer Allocation to default  */

/*
	i8255x_io_writel(PBA, I8255X_PBA_DEFAULT);
	*/

	/* Allow time for pending master requests to run */
	
	if (i8255x_reset_hw(ethptr) != OK)
		return SYSERR;

	if (i8255x_init_hw(ethptr) != OK)
		return SYSERR;


	return OK;
}

/*------------------------------------------------------------------------
 * i8255x_configure - configure the hardware for Rx and Tx
 *------------------------------------------------------------------------
 */
local status i8255x_configure(
	struct 	ether *ethptr
	)
{
	byte command;
	uint32 address;
	
	i8255x_configure_rx(ethptr);
	i8255x_configure_tx(ethptr);

	/*run the Load Dump Counters address command*/
	command = 0x04;
	command = command << I8255X_SCB_SHIFT_CUC;
	address  = (uint32)ethptr->stats;
	/*Write the address to SCB General Register*/
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);	
	/*Load CUC-Base opcode 0100 (23:20) */
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		//kprintf("load Dump Stats Counter Address Failed\n\r");
		return SYSERR;
	}
	return OK;
}

/*
 *------------------------------------------------------------------------
 * i8255x_reset_hw - Reset the hardware 
 *------------------------------------------------------------------------
 */
local status i8255x_reset_hw(
	struct 	ether *ethptr
	)
{
        /*
	uint32 ctrl, kab;
	uint32 dev_status;
	uint32 data, loop = I8255X_ICH_LAN_INIT_TIMEOUT;
	int32 timeout = MASTER_DISABLE_TIMEOUT;
        */

	//FIXME
	/*This fn just issues a Port Software reset command*/
	int32 retval =  outl(ethptr->iobase + 0x08 , 0x0000);
	DELAY(10); //suggested wait acc to the manual
        
	if(retval == 0){
                return OK;
        }
        else{
                //kprintf("i8255x_reset_hw failed. returned error val : %d\n\r", retval);
		return SYSERR;
        }
}

/*------------------------------------------------------------------------
 * i8255x_init_hw - Initialize the hardware
 *------------------------------------------------------------------------
 */
local status i8255x_init_hw(
	struct 	ether *ethptr
	)
{
//FIXME
	return OK; 
}

/*------------------------------------------------------------------------
 * i8255x_configure_rx - Configure Receive Unit after Reset
 *------------------------------------------------------------------------
 */
local void i8255x_configure_rx(
	struct 	ether *ethptr
	)
{	
	
	byte command;
	int32	address;
	
	
	command = 0x06;
	command = command << I8255X_SCB_SHIFT_RUC;
	address = 0;
	address += ((int32)ethptr->rxRing);
	/*Write the address to SCB General Register*/
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address);	
	/*Load RU-Base opcode 110 (18:16) */
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		//kprintf("load RU-Base FAILED\n\r");
	}
	
	/*
	 * //FIXME THIS IS NOT NEEDED.
	command = 0x0005;
	command = command << I8255X_SCB_SHIFT_RUC;
	address = 0;
	address += I8255X_RDSIZE;
	kprintf("RFD size : %d\n\r",address);
	/ *Write the size of RFD to SCB General Register* /
	outl(ethptr->iobase + I8255X_SCB_GENPTR, address );	
	/ *Load HDS opcode 101 (18:16)* /
	outw(ethptr->iobase + I8255X_SCB_COMMAND, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		kprintf("load HDS FAILED\n\r");
	}
*/

	//kprintf("config-rx SUCCESS\n\r");
	
}

/*------------------------------------------------------------------------
 * e1000_configure_tx - Configure Transmit Unit after Reset
 *------------------------------------------------------------------------
 */
local void i8255x_configure_tx(
	struct 	ether *ethptr
	)
{

	byte command = 0x0006;
	command = command << I8255X_SCB_SHIFT_CUC;
	/*Write the address to SCB General Register*/
	outl(ethptr->iobase + I8255X_SCB_GENPTR, (int32)ethptr->txRing);
	/*Load CU-Base opcode 0110 (23:20)*/
	outb(ethptr->iobase + I8255X_SCB_COMMAND_LOW, command);
	
	if(OK != i8255x_cmd_accept(ethptr)){
		//kprintf("load CU-Base FAILED\n\r");
	}

	//kprintf("config-tx SUCCESS\n\r");

}

