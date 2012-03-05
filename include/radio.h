/* radio.h */
#define RAD_ADDR_NVM 0x1E000
#define RAD_ADDR_LEN 8
typedef	uint8	radioaddr[RAD_ADDR_LEN];

struct radio {
	radioaddr	radio_long_addr;
	sid32	isem;
	sid32	osem;
};

extern	struct radio	radiotab[1];
