#include <xinu.h>

void maca_rx_callback(volatile packet_t *p) {
	struct radio *rptr;

	rptr = &radiotab[0];

	signal(rptr->isem);
}

void maca_tx_callback(volatile packet_t *p) {
	struct radio *rptr;

	rptr = &radiotab[0];
	signal(rptr->osem);
}

