#ifndef __REGISTER_H_
#define __REGISTER_H_

#define __REG_TYPE      volatile uint32_t
#define __REG           __REG_TYPE *

#define GPIO_BASE      ((__REG_TYPE) 0x3F200000) 
#define GPPUD          ((__REG) GPIO_BASE + 0x94)
#define GPPUDCLK0      ((__REG) GPIO_BASE + 0x98)

#define UART0_BASE     ((__REG_TYPE) 0x3F201000)
#define UART0_DR       ((__REG) UART0_BASE + 0x00)
#define UART0_RSRECR   ((__REG) UART0_BASE + 0x04)
#define UART0_FR       ((__REG) UART0_BASE + 0x18)   
#define UART0_ILPR     ((__REG) UART0_BASE + 0x20)
#define UART0_IBRD     ((__REG) UART0_BASE + 0x24)
#define UART0_FBRD     ((__REG) UART0_BASE + 0x28)
#define UART0_LCRH     ((__REG) UART0_BASE + 0x2C)
#define UART0_CR       ((__REG) UART0_BASE + 0x30)
#define UART0_IFLS     ((__REG) UART0_BASE + 0x34)
#define UART0_IMSC     ((__REG) UART0_BASE + 0x38)
#define UART0_RIS      ((__REG) UART0_BASE + 0x3C)
#define UART0_MIS      ((__REG) UART0_BASE + 0x40)
#define UART0_ICR      ((__REG) UART0_BASE + 0x44)
#define UART0_DMACR    ((__REG) UART0_BASE + 0x48)
#define UART0_ITCR     ((__REG) UART0_BASE + 0x80)
#define UART0_ITIP     ((__REG) UART0_BASE + 0x84)
#define UART0_ITOP     ((__REG) UART0_BASE + 0x88)
#define UART0_TDR      ((__REG) UART0_BASE + 0x8C)

#endif

