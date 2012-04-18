/* xsh_poweroff.c - xsh_poweroff*/

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_poweroff - shell command to delay for a specified number of seconds
 *------------------------------------------------------------------------
 */
shellcmd xsh_poweroff(int nargs, char *args[])
{


	struct	procent	*prptr;		/* pointer to process		*/
        pid32   i;

        /* Kill each process  not including the null process  */
	for (i = 1; i < NPROC; i++) {
		prptr = &proctab[i];
		if (prptr->prstate == PR_FREE || i == currpid) {  /* skip unused slots	*/
			continue;
		}
                printf("Killing process with pid [%d]\r\n", i);
                kill(i);
	}

        printf("Killing all the processes successfull, now killing shell and quitting...\r\n");
        kill(currpid);
        return 0;
}
