#define		RPL_NO_PRED		-1
#define		RPL_ROOT		0
#define		RPL_INF_DIST		9999

//distance to each nodes
byte		rplpath[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

//adjacency list
byte		rpladjlist[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

// list of interface addresses
int32   	iface_addr[LOWPAN_MAX_NODES];

enum{
	TENTATIVE, 
	PERMANENT
};

//structure for each node.
struct nodeinfo {
	byte pred;
	byte dist;
	byte label;

};
struct nodeinfo state[LOWPAN_MAX_NODES];

struct sourceroute {
	uint32	startmarker;
	uint32	len;
	uint32	endmarker;
};
