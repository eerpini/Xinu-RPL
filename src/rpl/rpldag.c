#include <xinu.h>
#ifdef LOWPAN_BORDER_ROUTER

struct nodeinfo state [ LOWPAN_MAX_NODES];

void computepaths(void) {
	
	int i = 0, j = 0, k = 0;

	for (i = 1; i < LOWPAN_MAX_NODES; i++) {

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

        for (i = 1; i < LOWPAN_MAX_NODES; i++) {
                kprintf (" Path from %d to 0 is : ", i);
                k = state[i].dist;

                for (j = k; j >= 0; j--)
                        kprintf (" %d ", rplpath[i][j]);

                kprintf ("\n");
        }
}

int getpath (uint32 target, uint32 *arr) {

        int j = 0, k = 0;
	int index = 0;

        k = state[target].dist;
        for (j = k; j >= 0; j--)
		arr[index ++] = rplpath[target][j];

	return index;
}

int getsourceroutehdr (uint32 target, uint32 *arr) {

	struct sourceroute 	srcheader;
	int			index = 0;

	srcheader.startmarker = 0xFFFFFFFF;
	srcheader.endmarker = 0xFFFFFFFF;
	srcheader.len = 0;
	
	index = getpath (target, &arr[2]);
	
	srcheader.len = index;

	arr[0] = srcheader.startmarker;
	arr[1] = srcheader.len;
	arr[index + 2] = srcheader.endmarker;

	return (index + 3);
}

void shortestpath (void) {

	int i, k, min;
	//struct state *ptr;

	//initialite the node info structures
	for (i = 0; i < LOWPAN_MAX_NODES; i++){
		state[i].pred = -1;	
                /*
                 * FIXME 13 for Sudhir, 
                 * We have dist defined as a byte ( for optimization reasons I guess)
                 * and have RPL_INF_DIST defined as 9999
                 * this throws a warning during compilation, but might be more harmful
                 */
		state[i].dist = RPL_INF_DIST;
		state[i].label = TENTATIVE;
	}

	state[RPL_ROOT].dist = 0; 
	state[RPL_ROOT].label = PERMANENT;
	state[RPL_ROOT].pred = RPL_NO_PRED;

	k = RPL_ROOT;

	while (TRUE) {

		//Relax the edges to the neighbors
		for (i = 0; i < LOWPAN_MAX_NODES; i++) {
			
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
		for (i = 0; i < LOWPAN_MAX_NODES; i++){
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
#endif
