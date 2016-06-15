/*
 * fiq.c
 *
 *  Created on: Jun 14, 2016
 *      Author: rickys
 */


#include <stdio.h>
#include "xil_exception.h"


extern void __set_fiq_regs(u32 *Regs);


void fiq_handler(void)
{
	printf("This is printed in fiq handler\n\r");

	// setup awdt
	awdt_init(5);

	printf("System will be reboot in 5 seconds\n\r");
	while(1);
}


void init_fiq(void)
{
	u32 fiq_regs[5];
	__set_fiq_regs(fiq_regs);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT, (Xil_ExceptionHandler) fiq_handler,(void *)0);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_FIQ);
}

void fiq_example(void)
{

	init_fiq();

	while(1);


}
