#define		RPL_INF_DIST		9999
#define		RPL_MAX_NODES		64
#define		RPL_NO_PRED		-1

#define		RPL_ROOT		0

//distance to each nodes
byte		rplpath[RPL_MAX_NODES][RPL_MAX_NODES];

//adjacency list
byte		rpladjlist[RPL_MAX_NODES][RPL_MAX_NODES];

enum{
	TENTATIVE, 
	PERMANENT
	};

//structure for each node.
struct nodeinfo {
	byte pred;
	byte dist;
	byte label;
}state[RPL_MAX_NODES];
