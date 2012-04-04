/* ethInit.c - ethInit */

#include <xinu.h>

struct	ether	ethertab[Neth];		/* Ethernet control blocks 	*/

/*------------------------------------------------------------------------
 * ethInit - Initialize Ethernet device structures
 *------------------------------------------------------------------------
 */
devcall	ethInit (
		struct dentry *devptr
		)
{
	struct	ether 	*ethptr;
	int32	dinfo;			/* device information		*/

	/* Initialize structure pointers */
	ethptr = &ethertab[devptr->dvminor];
	
	memset(ethptr, '\0', sizeof(struct ether));
	ethptr->dev = devptr;
	ethptr->csr = devptr->dvcsr;	/*Nothing is getting assigned at this point*/ //FIXME
	ethptr->state = ETH_STATE_DOWN;
	ethptr->mtu = ETH_MTU;
	ethptr->errors = 0;
	ethptr->addrLen = ETH_ADDR_LEN;
	ethptr->rxHead = ethptr->rxTail = 0;
	ethptr->txHead = ethptr->txTail = 0;
	
	/*dinfo is the index on PCI bus at which the NIC controller is located*/
	if ((dinfo = find_pci_device(INTEL_82559ER_DEVICE_ID,
					INTEL_VENDOR_ID, 0))
			!= SYSERR) {
		kprintf("Found Intel 82559ER Fast Ethernet controller NIC\n");

		ethptr->type = ETH_TYPE_I82559ER;
		ethptr->pcidev = dinfo;

		/* Initialize function pointers */
		
		ethptr->ethInit = i8255x_init;
                ethptr->ethOpen = i8255x_open;
	        ethptr->ethInterrupt = i8255x_intr;
		ethptr->ethWrite = i8255x_write;
		ethptr->ethControl = i8255x_cntl;
		//FIXME
                ethptr->ethClose = (void *)ioerr;
                ethptr->ethRead = i8255x_read;
	} 
        else if ((dinfo = find_pci_device(INTEL_82567LM_DEVICE_ID,
					    INTEL_VENDOR_ID, 0))
			  != SYSERR) {
		kprintf("Found Intel 82567LM Ethernet NIC\n");
                
                ethptr->type = ETH_TYPE_E1000E;
		ethptr->pcidev = dinfo;

		/* Initialize function pointers */
		
		ethptr->ethInit = e1000e_init;
                ethptr->ethOpen = e1000e_open;
	        ethptr->ethInterrupt = e1000e_intr;
		ethptr->ethWrite = e1000e_write;
		ethptr->ethControl = e1000e_cntl;
		//FIXME
                ethptr->ethClose = (void *)ioerr;
                ethptr->ethRead = e1000e_read;

        }
        else if ((dinfo = find_pci_device(INTEL_82545EM_DEVICE_ID,
				     INTEL_VENDOR_ID, 0))
		   != SYSERR) {
		kprintf("Found Intel 82545EM Ethernet NIC\n");
                 
                ethptr->type = ETH_TYPE_E1000E;
		ethptr->pcidev = dinfo;

		/* Initialize function pointers */
		
		ethptr->ethInit = e1000e_init;
                ethptr->ethOpen = e1000e_open;
	        ethptr->ethInterrupt = e1000e_intr;
		ethptr->ethWrite = e1000e_write;
		ethptr->ethControl = e1000e_cntl;
		//FIXME
                ethptr->ethClose = (void *)ioerr;
                ethptr->ethRead = e1000e_read;
               


        }
        else {
		kprintf("No recognized PCI Ethernet NIC found\n");
		return SYSERR;
	}
	/* Initialize control block */
		
	ethptr->ethInit(ethptr);

	/* Allocate descriptors and buffers, active the NIC */

	if (ethptr->ethOpen(ethptr) != OK)
		return SYSERR;

	/* Ethernet interface is active from here */

	ethptr->state = ETH_STATE_UP;

	return OK;
}
