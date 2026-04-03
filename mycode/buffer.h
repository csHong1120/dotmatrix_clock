
/*****************************************************
Project : 
Company : Intellian Technologies
Comments:

Chip type               : STM32F429ZGT
Program type            : Application
Core Clock frequency    : 25 MHz

File    : buffer.h
Version :
Date    :
Author  :

*****************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUFFER_H__
#define __BUFFER_H__


/* Includes ------------------------------------------------------------------*/
#include "main.h"




/* Defines -------------------------------------------------------------------*/

/* Buf mask */
#define __BUF_MASK(size) (size-1)
/* Check buf is full or not */
#define __BUF_IS_FULL(size, head, tail) ((tail&__BUF_MASK(size))==((head+1)&__BUF_MASK(size)))
/* Check buf will be full in next receiving or not */
#define __BUF_WILL_FULL(size, head, tail) ((tail&__BUF_MASK(size))==((head+2)&__BUF_MASK(size)))
/* Check buf is empty */
#define __BUF_IS_EMPTY(size, head, tail) ((head&__BUF_MASK(size))==(tail&__BUF_MASK(size)))
/* Reset buf */
#define __BUF_RESET(bufidx)           (bufidx=0)
#define __BUF_INCR(size, bufidx)      (bufidx=(bufidx+1)&__BUF_MASK(size))
#define __BUF_DECR(size, bufidx)      (bufidx=(bufidx-1)&__BUF_MASK(size))

#endif


