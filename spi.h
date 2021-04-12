/*******************************************************************************
*                                                                              *
*                    SPI-интерфейс для подключения акселерометра               *
*                                                                              *
* Версия: 1.0                                                                  *
* Дата:   14.07.2019 г.                                                        *
*                                                                              *
*******************************************************************************/
#ifndef spi_h
#define spi_h

#include <stdint.h>
#include <stdbool.h>
#include "definitions.h"
//#include "functions.h"  //func for test


/** SPI интерфейс для настройки акселерометра */
/* формат посылки [R/W] [MB] [A5 A4 A3 A2 A1 A0] [D7 D6 D5 D4 D3 D2 D1 D0] */
#define cs_down()    clrbit(PORT_CS, CS)
#define cs_up()      setbit(PORT_CS, CS)
#define clk_down()   clrbit(PORT_CLK, CLK)
#define clk_up()     setbit(PORT_CLK, CLK)
#define sdi_down()   clrbit(PORT_SDI, SDI)
#define sdi_up()     setbit(PORT_SDI, SDI)
#define sdi_rx()    {clrbit(DIR, SDI); setbit(REN, SDI); spiDelay();}
#define sdi_tx()     setbit(DIR, SDI)

/* SPI 3-wire interface (трёх проводный SPI) */
//#define WIRE_3



/*******************************************************************************
*                              Private function                                *
*******************************************************************************/
inline static void spiDelay()
{
  for (uint16_t timeout = 0; timeout < 100; timeout++) ;
}

/* Заголовок посылки (2 бита) */
inline static void gOpcode(uint8_t opcode)  /* 0000 00XX */
{
  for (uint8_t bit_i = 0; bit_i <= 1; bit_i++) {
    clk_down();
    //clrbit(PORT_CLK, CLK)
    if (opcode & 0x02)  /* 0b10 (1 - последний (старший) бит) */
      sdi_up();
    else
      sdi_down();
    clk_up();
    opcode <<= 1;  /* смещение значения на один бит */
  }
}

/* Адрес посылки */
inline static void gAddress(unsigned char addr)  /*000X XXXX */
{
  for (uint8_t bit_i = 0; bit_i <= 5; bit_i++) {
    clk_down();
    if (addr & 0x20)  /* 0b100000 */
      sdi_up();
    else
      sdi_up();
    clk_up();
    addr <<= 1;  /* смещение значения на один бит */
  }
}



/*******************************************************************************
*                              Public function                                 *
*******************************************************************************/
/* Чтение данных из регистра */
uint8_t Gread(uint8_t adr)
{
  uint8_t dataread = 0x00;
  
  
  cs_down();
  gOpcode(0x02);  // [1 0] -> чтение одного байта (регистра)
  gAddress(adr);  //адрес регистра
  
  /* Переключение на приём (для SPI 3-wire) */
#if WIRE_3
  sdi_rx();
//  clrbit(P3DIR, SDI);
//  setbit(P3REN, SDI);
//  SpiDelay(10);
#endif

  /* Чтение данных */
  for (int8_t bit = 7; bit >= 0; bit--) {
#if WIRE_3
    clk_down();
    clk_up();
    if ((IN & SDI) == SDI)  dataread |= 1 << bit;
#else    
    clk_down();
    clk_up();
    if ((IN & SDO) == SDO)  dataread |= 1 << bit;
#endif
  }
  
  cs_up();  //запрещаем посылки команд акселерометру
  
#if WIRE_3
  sdi_tx();  //переключение на передачу
#endif
  
  //SendByte(dataread);  /*test*/
  spiDelay();
  
  return dataread;
}

void Gwrite(uint8_t addrs, uint8_t comand)
{
  cs_down();
  spiDelay();
  gOpcode(0x00);  // [0 0] -> запись одного байта (регистра)
  gAddress(addrs);  //адрес регистра
  
  /* Команда (данные) */
  for (uint8_t bit = 0; bit <= 7; bit++) {
    clk_down();
    if (comand & 0x80)  /* 0b10000000 */
      sdi_up();
    else
      sdi_down();
    clk_up();
    comand <<= 1;
  }
  
  cs_up();  //запрещаем посылки команд акселерометру
  spiDelay();
}


#endif
