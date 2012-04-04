#include <xinu.h>


/*
 * Function for filling in the link local addresses,
 * this is just for testing and will be replaced by the radio layer ND Code 
 * when ready
 */

void genereate_link_local_neighbors(){

        int i=0;
        for(i=0; i < LOWPAN_MAX_NODES; i++){
                link_local_neighbors[i] = i;
        }

}


/*
 * Rpl initialization function, called when RPL has to be started
 */

rpl_init(){

#ifdef  LOWPAN_BORDER_ROUTER
       /*
        * Send DIO Messages to immediate Link Local Neighbors
        */


#endif

}
