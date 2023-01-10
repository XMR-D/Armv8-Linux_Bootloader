#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

//at offset 0x0
struct UART_DR
{
    uint8_t dr_reserved : 4;
    uint8_t dr_oe : 1;
    uint8_t dr_be : 1;
    uint8_t dr_pe : 1;
    uint8_t dr_fe : 1;
    uint8_t dr_data : 8;
}__attribute__((packed));

//at 0x018
struct UART_FR
{
    uint8_t fr_reserved : 7;
    uint8_t fr_ri : 1;
    uint8_t fr_txfe : 1;
    uint8_t fr_rxff : 1;
    uint8_t fr_txff : 1;
    uint8_t fr_rxfe : 1;
    uint8_t fr_busy : 1;
    uint8_t fr_dcd : 1;
    uint8_t fr_dsr : 1;
    uint8_t fr_cts : 1;
}__attribute__((packed));

//at 0x030
struct UART_CR
{
    uint8_t cr_ctse : 1;
    uint8_t cr_rtse : 1;
    uint8_t cr_out2 : 1;
    uint8_t cr_out1 : 1;
    uint8_t cr_rts : 1;
    uint8_t cr_dtr : 1;
    uint8_t cr_rxe : 1;
    uint8_t cr_txe : 1;
    uint8_t cr_lbe : 1;
    uint8_t cr_reserved : 4;
    uint8_t cr_sirlp : 1;
    uint8_t cr_siren : 1;
    uint8_t cr_uarten : 1;
}__attribute__((packed));

//at 0x02C
struct UARTLCR_H
{
    uint8_t lcr_reserved : 8;
    uint8_t lcr_sps : 1;
    uint8_t lcr_wlen : 2;
    uint8_t lcr_fen : 1;
    uint8_t lcr_stp2 : 1;
    uint8_t lcr_eps : 1;
    uint8_t lcr_pen : 1;
    uint8_t lcr_brk : 1;
}__attribute__((packed));

void putc(uint8_t c);
void puts(volatile char *str);
void wait_unbusy(void);

uint8_t getc(void);
void putint(uint64_t nb);
void puthex(uint64_t nb, uint8_t padding_mode);
void puthex_padded(char *str, uint32_t padding);

void Send_file(volatile uint8_t *p);
void put_pika(void);
void put_epika(void);


#endif
