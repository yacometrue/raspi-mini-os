#include <stddef.h>
#include <stdint.h>

#include "register.h"

#define UART0_TEXT_FLAG ((uint16_t)0x10)

static inline void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                 : "=r"(count)
                 : [ count ] "0"(count)
                 : "cc");
}

void uart_init(void) {
    *UART0_CR = 0x00000000;
    *GPPUD = 0x00000000;
    delay(150);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 = 0x00000000;
    *UART0_ICR = 0x7FF;
    *UART0_IBRD = 1;
    *UART0_FBRD = 40;
    *UART0_LCRH = (1 << 4) | (1 << 5) | (1 << 6);
    *UART0_IMSC = (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) |
                  (1 << 8) | (1 << 9) | (1 << 10);
    *UART0_CR = 1 | (1 << 8) | (1 << 9);
}

void put_c(unsigned char c) {
    while (*(UART0_FR) & 0x10);
    *(UART0_DR) = c;
}

void put_str(const char *str) {
    while (*str) {
        put_c((unsigned char)*str);
        str++;
    }
}

unsigned char get_c() {
    while (*(UART0_FR) & 0x8);
    return *(UART0_DR);
}

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
    uart_init();
    put_str("Welcome to Raspi mini os !!! \r\n");
    while (1);
    return 0;
}
