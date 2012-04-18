#define		RPL_NO_PRED		-1
#define		RPL_ROOT		0
//#define		RPL_INF_DIST		9999
#define		RPL_INF_DIST		255

//distance to each nodes
extern byte		rplpath[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

//adjacency list
extern byte		rpladjlist[LOWPAN_MAX_NODES][LOWPAN_MAX_NODES];

// list of interface addresses
uint32   	iface_addr[LOWPAN_MAX_NODES];

extern byte		iface_freshness[LOWPAN_MAX_NODES];

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
extern struct nodeinfo state[LOWPAN_MAX_NODES];

struct sourceroute {
	uint32	startmarker;
	uint32	len;
	uint32	endmarker;
};
