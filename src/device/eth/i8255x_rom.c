#include <xinu.h>

#define	ROMDELAY()	{ DELAY(1); }

#define EEPROM_READ_CMD (6 << 6)

/*------------------------------------------------------------------------
 * eep_romread - read data from the serial EEPROM
 * Note: the 82595 will be set to Bank 0 after the eeprom is read
 *------------------------------------------------------------------------
 */
unsigned short i8255x_romread(short iobase, short from)
{
    int   i;
    short loc = iobase + I8255X_EEPROM_REG;
    int   read_cmd = from | EEPROM_READ_CMD;
    unsigned short retval = 0;

    outb(loc, I8255X_EECS);

    /* Shift the read command bits out. */
    for (i = 8; i >= 0; i--) {
        short w = (read_cmd & (1 << i)) ? I8255X_EECS | I8255X_EEDI : I8255X_EECS;
        outb(loc, w);
        outb(loc, w | I8255X_EESK);   /* EEPROM clock tick. */
        ROMDELAY();
        outb(loc, w);		/* Finish EEPROM a clock tick. */
	ROMDELAY();
    }
    outb(loc, I8255X_EECS);

    for (i = 16; i > 0; i--) {
        outb(loc, I8255X_EECS | I8255X_EESK);
	ROMDELAY();
        retval = (retval << 1) | ((inb(loc) & I8255X_EEDO) ? 1 : 0);
        outb(loc, I8255X_EECS);
	ROMDELAY();
    }

    /* Terminate the EEPROM access. */
    outb(loc, I8255X_EESK);
    ROMDELAY();
    outb(loc, 0);
    ROMDELAY();
    return retval;
}
