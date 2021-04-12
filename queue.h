/*******************************************************************************
*                                                                              *
*                             MESSAGE QUEUE                                    *
*                    очередь для отправки сообщений                            *
*                                                                              *
* Версия: 1.0                                                                  *
* Дата:   20.07.2019 г.                                                        *
*                                                                              *
*******************************************************************************/
#ifndef queue_h
#define queue_h

#include <stdint.h>
#include <stdbool.h>
//#include "definitions.h"


/*******************************************************************************
*                              Private function                                *
*******************************************************************************/
#define QUEUE_SIZE 3  /*размер очереди*/
static uint8_t queue_buff[QUEUE_SIZE];  /*буфер очереди*/
static uint8_t queue_count = 0;  /*счётчик заполнения буфера*/
static uint8_t queue_in = 0;  /*индекс входа в очередь*/
static uint8_t queue_out = 0;  /*индекс выхода из очереди*/


/*******************************************************************************
*                              Public function                                 *
*******************************************************************************/
bool isQueueTransmit = false; 

/* Постановка в очередь */
bool QueueIn(uint8_t data)
{
  if (queue_count < QUEUE_SIZE) {
    queue_buff[queue_in] = data;
    queue_in++;
    if (queue_in == QUEUE_SIZE)  queue_in = 0;
    queue_count++;
    isQueueTransmit = true;
    return true;
  } else
    return false;
}

/* Забираем значение из очереди */
uint8_t QueueOut()
{
  uint8_t data;
  
  
  data = queue_buff[queue_out];
  queue_out++;
  if (queue_out == QUEUE_SIZE)  queue_out = 0;
  queue_count--;
  if (queue_count == 0)  isQueueTransmit = false;
  
  return data;
}

#endif
