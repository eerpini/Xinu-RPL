/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	uint32 retval;
        char buf[NBPG] = {0};
        did32 val = 0;

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
         
        //psinit();
                
        kprintf("Done with psinit\r\n");
        
        if(get_bs(0, 100) != SYSERR ){

                kprintf("Opened the disk\r\n");
                memcpy(buf, "abcdef", 6);
                if(write_bs(buf, 0, 0) == OK){
                        kprintf("Successfully written \r\n");
                }
                else{
                        kprintf("Something went wrong\r\n");
                }
                memcpy(buf, "abcdefghijklmnopqr", 18);
                if(write_bs(buf, 0, 17) == OK){
                        kprintf("Successfully written \r\n");
                }
                else{
                        kprintf("Something went wrong\r\n");
                }
        }
        if(release_bs(0) != SYSERR){
                kprintf("Releasing the backing store succeeded\r\n");
        }
        else
                kprintf("Releasing the backing store failed\r\n");
        memset(buf, NULLCH, NBPG);
        if(get_bs(0, 50) != SYSERR){
                if(read_bs(buf, 0, 0) != SYSERR){
                        kprintf("Successfully read\r\n");
                }

                
                kprintf("The buffer is %s\r\n", buf);

                if(read_bs(buf, 0, 0) != SYSERR){
                        kprintf("Successfully read\r\n");
                }
                kprintf("The buffer is %s\r\n", buf);

        }

        if(release_bs(0) != SYSERR){
                kprintf("Releasing the backing store succeeded\r\n");
        }
        else
                kprintf("Releasing the backing store failed\r\n");

        return OK;

}
