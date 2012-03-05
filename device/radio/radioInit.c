/* radioInit.c - radioInit */

#include <xinu.h>
#include <maca.h>
#include <crm.h>
#include <redbee-econotag.h>
#include <nvm.h>

#define trim_xtal() pack_XTAL_CNTL(CTUNE_4PF, CTUNE, FTUNE, IBIAS)

unsigned char unspec[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct radio radiotab[Nradio];

/*------------------------------------------------------------------------
 *  radioInit - initialize MC1322x radio
 *------------------------------------------------------------------------
 */
int
get_radiolongaddr(radioaddr *addr) 
{
	nvmType_t type=0;
	nvmErr_t err;	
	volatile uint8_t buf[RAD_ADDR_LEN];
		
	if ((err = nvm_detect(gNvmInternalInterface_c, &type))) {
		kprintf("nvm_detect failed %d\n", err);
		return err;
	}

	if ((err = nvm_read(gNvmInternalInterface_c, type, (uint8_t *)buf, 
			    RAD_ADDR_NVM, RAD_ADDR_LEN))) {
		kprintf("nvm_read failed %d\n", err);
		return err;
	}

	memcpy(addr, (radioaddr *)buf, RAD_ADDR_LEN);

#ifdef NOTDEF
	/* put back check for correctness at some point... */
	if (memcmp(addr, &addr_ff, RIMEADDR_CONF_SIZE)==0) {

		//set addr to EUI64
#ifdef IAB		
   #ifdef EXT_ID
		PRINTF("address in flash blank, setting to defined IAB and extension.\n\r");
	  	iab_to_eui64(&eui64, OUI, IAB, EXT_ID);
   #else  /* ifdef EXT_ID */
		PRINTF("address in flash blank, setting to defined IAB with a random extension.\n\r");
		iab_to_eui64(&eui64, OUI, IAB, *MACA_RANDOM);
   #endif /* ifdef EXT_ID */

#else  /* ifdef IAB */

   #ifdef EXT_ID
		PRINTF("address in flash blank, setting to defined OUI and extension.\n\r");
		oui_to_eui64(&eui64, OUI, EXT_ID);
   #else  /*ifdef EXT_ID */
		PRINTF("address in flash blank, setting to defined OUI with a random extension.\n\r");
		oui_to_eui64(&eui64, OUI, ((*MACA_RANDOM << 32) | *MACA_RANDOM));
   #endif /*endif EXTID */

#endif /* ifdef IAB */

		rimeaddr_copy(addr, &eui64);		
#ifdef FLASH_BLANK_ADDR
		PRINTF("flashing blank address\n\r");
		err = nvm_write(gNvmInternalInterface_c, type, &(eui64.u8), RIMEADDR_NVM, RIMEADDR_NBYTES);		
#endif /* ifdef FLASH_BLANK_ADDR */
	} else {
		PRINTF("loading rime address from flash.\n\r");
	}
#endif
	return OK;
}

devcall	radioInit(
	  struct dentry	*devptr		/* entry in device switch table	*/
	)
{
	struct radio *rptr;

	rptr = &radiotab[devptr->dvminor];

	trim_xtal();
	vreg_init();
	maca_init();

	set_channel(13);  /* 802.15.4 channel 11 */
	set_power(0x12); /* 0x12 is the highest, 4.5dBm */

	get_radiolongaddr(&rptr->radio_long_addr);

	printf("radio address: ");
	printradaddr(&rptr->radio_long_addr);
	printf("\n");

	rptr->isem = semcreate(0);
	rptr->osem = semcreate(5); /* buffer at most 5 output buffers */

	return OK;
}

