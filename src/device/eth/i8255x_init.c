/* i8255x_init.c - i8255x_init */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_init - initialize Intel Hub 82559ER Ethernet NIC
 *------------------------------------------------------------------------
 */
void 	i8255x_init(
	struct 	ether *ethptr
	)
{
	//uint16  command;
	int32	i;
        unsigned short read_mac_buf;

	/* Read PCI configuration information */

	/* Read I/O base address */

	pci_bios_read_config_dword(ethptr->pcidev, I8255X_PCI_IOBASE,
			(uint32 *)&ethptr->iobase);
	
	//FIXME : Is this the right way to enable IO MAPPED ACCESS ?
	ethptr->iobase &= ~1;
	ethptr->iobase &= 0xffff; /* the low bit is set to indicate I/O */



	/* Read flash base address */

	ethptr->flashbase = 0;
	pci_bios_read_config_dword(ethptr->pcidev, I8255X_PCI_FLASHBASE,
			(uint32 *)&ethptr->flashbase);

	/* Read memory base address */

	pci_bios_read_config_dword(ethptr->pcidev, I8255X_PCI_MEMBASE,
			(uint32 *)&ethptr->membase);
	ethptr->membase &= ~2; 	/* if mem address is below 1 MB */

	/* Read interrupt line number */

	pci_bios_read_config_byte (ethptr->pcidev, I8255X_PCI_IRQ,
			(byte *)&(ethptr->dev->dvirq));

	/* Enable PCI bus master, memory access and I/O access */

        /*
	pci_bios_read_config_word(ethptr->pcidev, I8255X_PCI_COMMAND, 
			&command);
	command |= I8255X_PCI_CMD_MASK;
	pci_bios_write_config_word(ethptr->pcidev, I8255X_PCI_COMMAND, 
			command);

                        */
	/* Read the MAC address */
	for(i = 0; i < 3; i++){
		read_mac_buf = 0;
		read_mac_buf = i8255x_romread(ethptr->iobase, i); /*read the ith word of mac Addr*/
		memcpy((char *)ethptr->devAddress + 2*i, (char *)(&read_mac_buf), sizeof(unsigned short));
	}


	kprintf("MAC address obtained: %02x:%02x:%02x:%02x:%02x:%02x\n",
			0xff&ethptr->devAddress[0],
			0xff&ethptr->devAddress[1],
			0xff&ethptr->devAddress[2],
			0xff&ethptr->devAddress[3],
			0xff&ethptr->devAddress[4],
			0xff&ethptr->devAddress[5]);

}
