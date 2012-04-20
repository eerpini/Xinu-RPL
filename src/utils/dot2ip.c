/* dot2ip.c - dot2ip */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define NULLCH  0
/*------------------------------------------------------------------------
 * dot2ip - convert a string of dotted decimal to an unsigned integer
 *------------------------------------------------------------------------
 */
int main( int argc, char **argv)
{
	int     seg;			/* counts segments		*/
	int     nch;			/* counts chars within segment	*/
	char	ch;			/* next character		*/
	unsigned int ipaddr;			/* IP address in binary		*/
	int	val;			/* binary value of one segment	*/
        char    *dotted = NULL;

	/* input must have the form  X.X.X.X, where X is 1 to 3 digits	*/

        if(argc < 2){
                printf("Usage : %s <X.X.X.X>\n", argv[0]);
                return EXIT_FAILURE;
        }
        dotted = argv[1];
	ipaddr = 0;
	for (seg=0 ; seg<4 ; seg++) {	/* for each segment */
	    val = 0;
	    for (nch=0 ; nch<4; nch++) { /* up to four chars per segment*/
		ch = *dotted++;
		if ( (ch==NULLCH) || (ch == '.') ) {
			if (nch == 0) {
				return EXIT_FAILURE;
			} else {
				break;
			}
		}

		/* too many digits or non-digit is an error */

		if ( (nch>=3) || (ch<'0') || (ch>'9') ) {
			return EXIT_FAILURE;
		}
		val = 10*val + (ch-'0');
	    }

	    if (val > 255) {
		return EXIT_FAILURE;
	    }
	    ipaddr = (ipaddr << 8) | val;

	    if (ch == NULLCH) {
		break;
	    }
	}
	if ( (seg >= 4) || (ch != NULLCH) ) {
		return EXIT_FAILURE;
	}
        printf("Input : [%s] Output : [%04x]\n", argv[1], ipaddr);
	return EXIT_SUCCESS;
}
