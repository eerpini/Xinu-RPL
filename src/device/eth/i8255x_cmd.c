/* i8255x_cmd.c  -  i8255x_irq_enable, i8255x_irq_disable */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_irq_disable - Mask off interrupt generation on the NIC
 *------------------------------------------------------------------------
 */
void i8255x_irq_disable(
	struct 	ether *ethptr
	)
{
	unsigned short intmask;

	intmask = 0; 
        intmask  = inw(ethptr->iobase + I8255X_SCB_COMMAND_LOW);
        //kprintf("Read in the SCB COMMAND : %x\r\n", intmask);
	intmask |= I8255X_INTR_M_MASK;
	//kprintf("irq disable: writing %x to command word\n\r", intmask);
	/* set the M bit in the command word*/
	outw(ethptr->iobase + I8255X_SCB_COMMAND_LOW, intmask);
	
	//kprintf("irq disable SUCCESS\n\r");

}

/*------------------------------------------------------------------------
 * i8255x_irq_enable - Enable default interrupt generation settings
 *------------------------------------------------------------------------
 */
void i8255x_irq_enable(
	struct 	ether *ethptr
	)
{
	unsigned short intmask;

	//intmask = 0xff00;//All the interrupts disabled 
        /* 
         * Writing 1 to the SWI bit with cause a Software interrupt
         * we should not do this 
         * Hence the command word to be written (interrupt mask) is : 
         * 1111110100000000/0xFD00
         */
	intmask = 0xff00;
	intmask = intmask &  ~(I8255X_INTR_M_MASK );
	intmask = intmask & ~(I8255X_INTR_SI_MASK);
	intmask = intmask &  ~(I8255X_INTR_ENABLE_MASK); /* set the interrupts to be enabled to '0' here */

        //kprintf("irq enable: the current interrupt mask is %x\r\n", inw(ethptr->iobase + I8255X_SCB_COMMAND_LOW));
	//kprintf("irq enable: writing %x to command word\n\r", intmask);
	/* unset the M bit in the command word and also the required interrupt bits*/
	outw(ethptr->iobase + I8255X_SCB_COMMAND_LOW, intmask);
        //kprintf("irq enable: The command workd should now be 0x1C00 : %x\r\n",
          //              inw(ethptr->iobase + I8255X_SCB_COMMAND_LOW));

	//kprintf("irq enable SUCCESS\n\r");
}
