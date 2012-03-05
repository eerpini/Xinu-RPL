#include <xinu.h>
#include <stdio.h>

void printhex(unsigned char *buf, int n)
{
#define PER_ROW 16
	int i, j, k;
	for(j=0, k=0; j <= ( n / PER_ROW ); j++) {
		for(i=0; i < PER_ROW; i++, k++) {
			if(k >= n ) { 
				printf("\n");
				break; 
			} 
			printf("%02x ",buf[j*PER_ROW + i]);
		}
		printf("\n");
	}
}
