/* rad_in.c - rad_init, rad_in */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * net_init - initialize network data structures and processes
 *------------------------------------------------------------------------
 */

void	rad_init (void)
{

  /* Set up radio input interface queue */

}

process	rad_in (void)
{

	while (1) {

		n = read(RADIO0, buf, 128);

		if (n == 0)
			continue;

		if (buf[0] != 0x41)
			continue;

	}
}
