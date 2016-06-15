/*
 * awdt.c
 *
 *  Created on: Jun 15, 2016
 *      Author: rickys
 */

#include <stdio.h>
#include <xtime_l.h>
#include <xil_io.h>
#include <sleep.h>

#define SLCR_UNLOCK 0xF8000008
#define UNLOCK_KEY 0xDF0D

#define RS_AWDT_CTRL 0xF800024C

#define WDT_CONTROL 0xF8F00628
#define WD_ENABLE_BIT 0
#define AUTO_RELOAD_BIT 1
#define WD_MODE_BIT 3
#define PRESCALER_BIT 8

#define WDT_COUNTER 0xF8F00624

#define WDT_LOAD 0xF8F00620

#define WDT_RST_STS 0xF8F00630

#define WDT_ISR 0xF8F0062C



int val_global = 0;

void awdt_status(void)
{
	printf("0xF8F00628, WDT_CONTROL = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_CONTROL));
	printf("0xF8F0062C, WDT_ISR = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_ISR));
	printf("0xF8F00630, WDT_RST_STS = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_RST_STS));

}

int awdt_kick_seconds(unsigned int seconds)
{
	Xil_Out32(WDT_LOAD, seconds*1300781);

	return 0;
}

int awdt_init(unsigned int seconds)
{

	// set 2'b11 to RS_AWDT_CTRL
	// - unlock slcr first
	Xil_Out32(SLCR_UNLOCK, UNLOCK_KEY);

	unsigned int awdt_ctrl;
	Xil_Out32(RS_AWDT_CTRL, 0x3);
	awdt_ctrl = Xil_In32(RS_AWDT_CTRL);
	if (awdt_ctrl != 0x3)
	{
		printf("error, RS_AWDT_CTRL set unsuccessful\n\r");
		sleep(2);
		exit(0);
	}

	// set initial load
	awdt_kick_seconds(seconds);

	// set watchdog control
	unsigned int wdt_control = 1 << WD_ENABLE_BIT | 0 << AUTO_RELOAD_BIT | 1 << WD_MODE_BIT | 0xFF << PRESCALER_BIT;
	Xil_Out32(WDT_CONTROL, wdt_control);

	return 0;
}

// awdt clock frequency is half of cpu = 333000000
// load = awdt_frequency / prescale
// load = 333000000 / 256 = 1300781



int loop_body(void)
{
	static int val_static = 0;

	val_static++;
	val_global++;

	printf("val_static: %d\n\r", val_static);
	printf("val_global: %d\n\r", val_global);

	awdt_kick_seconds(4);

	return 0;
}

void awdt_reset_status_check(void)
{
	unsigned int wdt_rst_sts;

	// read out last time reset status
	wdt_rst_sts = Xil_In32(WDT_RST_STS);
	// printf("reset status: %x\n\r", wdt_rst_sts);

	if (wdt_rst_sts & 0x1)
	{
		printf("CPU1 was reset by AWDT\n\r");

		printf("cleaning up reset status...");
		Xil_Out32(WDT_RST_STS, 1);

		// read back
		wdt_rst_sts = Xil_In32(WDT_RST_STS);
		// printf("reset status: %x\n\r", wdt_rst_sts);
		if ((wdt_rst_sts & 0x1) == 0)
		{
			printf("reset status cleaned up\n\r");
		}
		else
		{
			printf("reset status clean up failed\n\r");
		}

	}
	else
	{
		printf("This is a clean boot\n\r");
	}
}

//int main()
//{
//
//
//    init_platform();
//
//    printf("\n\rEntry to the program\n\r\n\r");
//
//    awdt_reset_status_check();
//
//    // initialize awdt
//    awdt_init();
//
//    // looping and feed awdt
//    int i;
//    for(i=0;i<3;i++)
//    {
//    	loop_body();
//    	//awdt_status();
//    	sleep(1);
//
//    }
//
//    printf("Stopped to kick AWDT, waiting for reset happen.\n\r");
//    // awdt_status(); // check awdt reg status
//
//    sleep(4); // exceeds load number
//    awdt_status(); // should never hit
//
//    cleanup_platform();
//    return 0;
//}
