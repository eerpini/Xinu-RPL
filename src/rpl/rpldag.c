#include <xinu.h>

void computepaths(void) {
	
	int i = 0, j = 0, k = 0;

	for (i = 1; i < RPL_MAX_NODES; i++) {

		k = i;
		j = 0;
		while (k > RPL_ROOT) {
			rplpath[i][j++] = k;
			k = state[k].pred;
		}
	}
}

void printpaths (){

        int i = 0, j = 0, k = 0;

        for (i = 1; i < RPL_MAX_NODES; i++) {
                printf (" Path from %d to 0 is : ", i);
                k = state[i].dist;

                for (j = k; j >= 0; j--)
                        printf (" %d ", rplpath[i][j]);

                printf ("\n");
        }
}

void shortestpath (void) {

	int i, k, min;
	struct state *ptr;

	//initialite the node info structures
	for (i = 0; i < RPL_MAX_NODES; i++){
		state[i].pred = -1;	
		state[i].dist = RPL_INF_DIST;
		state[i].label = TENTATIVE;
	}

	state[RPL_ROOT].dist = 0; 
	state[RPL_ROOT].label = PERMANENT;
	state[RPL_ROOT].pred = RPL_NO_PRED;

	k = RPL_ROOT;

	while (TRUE) {

		//Relax the edges to the neighbors
		for (i = 0; i < RPL_MAX_NODES; i++) {
			
			if (rpladjlist[k][i] != 0 && state[i].label == TENTATIVE) {
				if (state[k].dist + rpladjlist[k][i] < state[i].dist){
					state[i].pred = k;
					state[i].dist = state[k].dist + rpladjlist[k][i];
				}
			}
		}

		//find the minimum distance node with tentative label
		k = 0;
		min = RPL_INF_DIST;
		for (i = 0; i < RPL_MAX_NODES; i++){
			if ((state[i].label == TENTATIVE) && (state[i].dist < min)){
				min = state[i].dist;
				k = i;
			}
		}

		if (k != RPL_ROOT) {
			//make the node permanent
			state[k].label = PERMANENT;
		} else {
			// looked at all nodes and all nodes are PERMANENT
			break;
		}
	}
}
