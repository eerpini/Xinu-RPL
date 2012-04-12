/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	uint32 retval;
        char buf[NBPG] = {0};
        //did32 val = 0;

	kprintf("main: calling getlocalip\n");
	retval = getlocalip();

	if (retval == SYSERR) {
		kprintf("I'm here to stop!\n");
	} else {
		kprintf("I'm here to continue!\n");
		kprintf("\n\n###########################################################\n\n");
		kprintf("IP address is %d.%d.%d.%d   %08x\n\r",
			(retval>>24)&0xff, (retval>>16)&0xff, (retval>>8)&0xff,
		 	retval&0xff,retval);

	    	kprintf("Subnet mask is %d.%d.%d.%d and router is %d.%d.%d.%d\n\r",
			(NetData.addrmask>>24)&0xff, (NetData.addrmask>>16)&0xff,
			(NetData.addrmask>> 8)&0xff,  NetData.addrmask&0xff,
			(NetData.routeraddr>>24)&0xff, (NetData.routeraddr>>16)&0xff,
			(NetData.routeraddr>> 8)&0xff, NetData.routeraddr&0xff);
	}
       
        dot2ip(RPL_FORWARDING_GATEWAY, &retval);
        if(retval == NetData.ipaddr){
                kprintf("I am the simulator gateway\r\n");
        }
        

        rpl_init();

        resume(create(shell, 4096, INITPRIO, "Shell", 1, CONSOLE));
        receive();

        getlocalip();
        resume(create(shell, 4096, INITPRIO, "Shell", 1, CONSOLE));
         
                
        return OK;

}
