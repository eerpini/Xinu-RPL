/* i8255x_cntl.c - i8255x_cntl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * i8255x_cntl - implement control function for an E100 device
 *------------------------------------------------------------------------
 */
devcall	i8255x_cntl(
	struct	ether *ethptr,		/* entry in device switch table	*/
	int32	func,			/* control function		*/
	int32	arg1,			/* argument 1, if needed 	*/
	int32	arg2			/* argument 2, if needed	*/
	)
{
	switch (func) {

		/* Get MAC address */

		case ETH_CTRL_GET_MAC:
			memcpy((byte *)arg1, ethptr->devAddress, 
					ETH_ADDR_LEN);
			break;
		case ETH_CTRL_PRINT_STATS:
			i8255x_print_stats(ethptr, (bool8) arg1);

		default:
			return SYSERR;
	}

	return OK;
}
