/*
 * awdt.c: simple awdt test application
 *
 * Setup private Watchdog to reset the cpu it's on 
 * when counter counts down to zero.
 * Program is alloated to address 0 so that after being
 * reset, the same program will run again.
 *
 * The main workflows are
 * - unlock SLCR
 * - set slcr.RS_AWDT_CTRL
 * - configure AWDT
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <xtime_l.h>
#include <xil_io.h>

/* Register defination */
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


/* Global variable Defination */
/* To check how global variable will be affect after AWDT reset*/
int val_global = 0;

/*
 * function awdt_status()
 * 
 * Print AWDT related register info.
 */
void awdt_status(void)
{
	printf("0xF8F00628, WDT_CONTROL = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_CONTROL));
	printf("0xF8F0062C, WDT_ISR = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_ISR));
	printf("0xF8F00630, WDT_RST_STS = 0x%08x\n\r", (unsigned int)Xil_In32(WDT_RST_STS));

}

/*
 * function awdt_kick_seconds()
 *
 * set AWDT load register.
 * The load register should be set periodically to prevent AWDT reset
 *
 * parameters
 * - seconds: How many seconds AWDT load register should be set to
 */
int awdt_kick_seconds(unsigned int seconds)
{
	Xil_Out32(WDT_LOAD, seconds*1300781);
	// How 1300781 is calculated?
	// awdt clock frequency is half of cpu = 333000000
	// load = awdt_frequency / prescale
	// load = 333000000 / 256 = 1300781

	return 0;
}

/*
 * function awdt_init()
 * 
 * initialize AWDT settings to let AWDT reset its own core 
 * but not the whole system.
 */
int awdt_init(void)
{
	// - unlock slcr first
	Xil_Out32(SLCR_UNLOCK, UNLOCK_KEY);
	// set 2'b11 to RS_AWDT_CTRL to make AWDT reset its own core only when count to zero
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
	awdt_kick_seconds(10);

	// set watchdog control
	unsigned int wdt_control = 1 << WD_ENABLE_BIT | 0 << AUTO_RELOAD_BIT | 1 << WD_MODE_BIT | 0xFF << PRESCALER_BIT;
	Xil_Out32(WDT_CONTROL, wdt_control);

	return 0;
}


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

int main()
{

    printf("\n\rEntry to the program\n\r\n\r");

    awdt_reset_status_check();

    // initialize awdt
    awdt_init();

    // looping and feed awdt
    int i;
    for(i=0;i<3;i++)
    {
    	loop_body();
    	//awdt_status();
    	sleep(1);

    }

    printf("Stopped to kick AWDT, waiting for reset happen.\n\r");
    // awdt_status(); // check awdt reg status

    sleep(4); // exceeds load number
    awdt_status(); // should never hit

    return 0;
}
