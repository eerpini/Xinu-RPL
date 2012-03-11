/* nd.h - definitions for Neighbor Discovery protocol */

/* ND cache entry states */

#define ND_EMPTY 0

struct ndcache_entry {
	IP6addr		nd_nghbrIP;
	radioaddr	nd_nghbrMAC64;
	uint16	 	nd_nghbrMAC16;
	struct netpacket *nd_pending;	/* for now, just queue one packet */
	struct {
		uint8	nd_isrouter:1;
		uint8	nd_state:4;
	};
	uint8		nd_timer;	/* number of secs to next state */
        /* likely needs fields for number of resolution retries */
	/* likely needs fields for NUD */
};

#define NNDCACHE 16
extern struct ndcache_entry ndcache[];

/* to be coded:
 * Duplicate Address Detection
 * Address resolution
 * NUD
 */
