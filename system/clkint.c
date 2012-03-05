#include <xinu.h>
#include <mc1322x.h>
#include <redbee-econotag.h>

int32 clkticks = 0;
#define CLKTICKS_PER_SECOND 500

void rtc_isr(void) {

	disable_rtc_wu_irq();
	disable_rtc_wu();

	if ((clkticks++) >= CLKTICKS_PER_SECOND) {
		clktime++;
		clkticks = 0;

		/* flash red LED on 1 second duty cycle */
		if (clktime % 2) {
			*GPIO_PAD_DIR1 |= (1UL << (LED_GREEN - 32));
			*GPIO_DATA_SET1 = (1UL << (LED_GREEN - 32));
		}
		else {
			*GPIO_PAD_DIR1 |= (1UL << (LED_GREEN - 32));
			*GPIO_DATA_RESET1 = (1UL << (LED_GREEN - 32));
		}
	}

	if(slnonempty) {
	  *sltop -= (1000/CLKTICKS_PER_SECOND);
	  if (*sltop <= 0) {
	    wakeup();
	  }
	}

	clear_rtc_wu_evt();
	while (rtc_wu_evt()) {
	  continue;
	}
	enable_rtc_wu();
	enable_rtc_wu_irq();

	preempt--;
	if (preempt <= 0)
	  resched();
}
