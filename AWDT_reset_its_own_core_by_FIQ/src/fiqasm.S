/*
 * fiqasm.S
 *
 *  Created on: Sep 30, 2014
 *      Author: pallavj
 *    Modified: Jun 15, 2016
 *          By: rickys
 *
 * Note:
 * This is an abstraction of https://github.com/Xilinx/linux-xlnx/blob/master/arch/arm/kernel/fiqasm.S
 */


#define FIQ_MODE        0x00000011
#define PSR_F_BIT       0x00000040
#define PSR_I_BIT       0x00000080

.text
.global __set_fiq_regs
.align 4

__set_fiq_regs:
        mov     r2, #PSR_I_BIT | PSR_F_BIT | FIQ_MODE
        mrs     r1, cpsr
        msr     cpsr_c, r2      @ select FIQ mode
        mov     r0, r0          @ avoid hazard prior to ARMv4
        ldmia   r0!, {r8 - r12}
        ldr     sp, [r0], #4
        ldr     lr, [r0]
        msr     cpsr_c, r1      @ return to SVC mode
        mov     r0, r0          @ avoid hazard prior to ARMv4
        mov     pc, lr
