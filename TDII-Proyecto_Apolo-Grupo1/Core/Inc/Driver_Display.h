/**
  ******************************************************************************
  * @file    hx8347g.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-May-2014
  * @brief   This file contains all the functions prototypes for the hx8347g.c
  *          driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HX8347G_H
#define __HX8347G_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "stdlib.h"
// Pins definitions

#define RD_PORT GPIOA
#define RD_PIN  0//GPIO_PIN_0
#define WR_PORT GPIOA
#define WR_PIN  1//GPIO_PIN_1
#define CD_PORT GPIOA          // RS PORT
#define CD_PIN  4//GPIO_PIN_4     // RS PIN
#define CS_PORT GPIOB
#define CS_PIN  0//GPIO_PIN_0
#define RESET_PORT GPIOC
#define RESET_PIN  1//GPIO_PIN_1

/*
 * Pin 9 , puerto A el D0
 * Pin 7 , puerto C el D1
 * Pin 10, puerto A el D2
 * Pin 3 , puerto B el D3
 * Pin 5 , puerto B el D4
 * Pin 4 , puerto B el D5
 * Pin 10, puerto B el D6
 * Pin 8 , puerto A el D7
 *
 * */
#define D0_PORT GPIOA
#define D0_PIN 9
#define D1_PORT GPIOC
#define D1_PIN 7
#define D2_PORT GPIOA
#define D2_PIN 10
#define D3_PORT GPIOB
#define D3_PIN 3
#define D4_PORT GPIOB
#define D4_PIN 5
#define D5_PORT GPIOB
#define D5_PIN 4
#define D6_PORT GPIOB
#define D6_PIN 10
#define D7_PORT GPIOA
#define D7_PIN 8


#define  WIDTH    ((uint16_t)320)
#define  HEIGHT   ((uint16_t)480)

/*		width(x)
 * ----------------------
 * |(x=0,y=0)			|
 * |					|
 * |					|												---------------------------------
 * |					|			 Si lo rotas cambia el x por y		|(x=0;y=0)						|
 * |					| Height (y) ------------------------------->	|								|
 * |					|												|								|
 * |					|												|								|
 * |					|												|								|
 * |					|												|								|
 * |		   (320,480)|												|					   (480,320)|
 * ---------------------												---------------------------------
 *
 * */
// configure macros for the data pins.
/*
 * Pone en cero los pines 8 - 9 - 10 del puerto A
 * Pone en cero los pines 3 - 4 - 5 - 10 del puerto B
 * Pone en cero los pines 7 del puerto C
 * Escribe en:
 * pin 9 , puerto A el D0 ok
 * pin 10, puerto A el D2 ok
 * pin 8 , puerto A el D7 ok
 * pin 3 , puerto B el D3 ok
 * pin 5 , puerto B el D4 ok
 * pin 4 , puerto B el D5 ok
 * pin 10, puerto B el D6 ok
 * pin 7 , puerto C el D1 ok
 * */

#define write_8(d) { \
GPIOA->BSRR = 0x0700 << 16; \
GPIOB->BSRR = 0x0438 << 16; \
GPIOC->BSRR = 0x0080 << 16; \
GPIOA->BSRR = (((d) & (1<<0)) << 9) \
            | (((d) & (1<<2)) << 8) \
            | (((d) & (1<<7)) << 1); \
GPIOB->BSRR = (((d) & (1<<3)) << 0) \
            | (((d) & (1<<4)) << 1) \
            | (((d) & (1<<5)) >> 1) \
            | (((d) & (1<<6)) << 4); \
GPIOC->BSRR = (((d) & (1<<1)) << 6); \
}

/*
 * Lee los datos de la siguiente forma:
 * Pin 9 , puerto A el D0
 * Pin 7 , puerto C el D1
 * Pin 10, puerto A el D2
 * Pin 3 , puerto B el D3
 * Pin 5 , puerto B el D4
 * Pin 4 , puerto B el D5
 * Pin 10, puerto B el D6
 * Pin 8 , puerto A el D7
 *
 * */
#define read_8() (          (((GPIOA->IDR & (1<<9)) >> 9) \
                            | ((GPIOC->IDR & (1<<7)) >> 6) \
                            | ((GPIOA->IDR & (1<<10)) >> 8) \
                            | ((GPIOB->IDR & (1<<3)) >> 0) \
                            | ((GPIOB->IDR & (1<<5)) >> 1) \
                            | ((GPIOB->IDR & (1<<4)) << 1) \
                            | ((GPIOB->IDR & (1<<10)) >> 4) \
                            | ((GPIOA->IDR & (1<<8))  >> 1)))




/************************** For 84 MHZ ****************************/
#define WRITE_DELAY { WR_ACTIVE2; } // !!!! Nota lo cambie estaba en WR_ACTIVE2 !!!!!
#define READ_DELAY  { RD_ACTIVE8; }



/*****************************  DEFINES FOR DIFFERENT TFTs   ****************************************************/


#define SUPPORT_68140             //RM68140 +52 bytes defaults to PIXFMT=0x55


/*********************************** PIN USE *********************************************************/

#define PIN_LOW(port, pin)    (port)->BSRR = (1<<((pin)+16)) // Setea en forma atomica el registro de salida en 0
#define PIN_HIGH(port, pin)   (port)->BSRR = (1<<(pin))		// Idem pero en 1

#define PIN_READ(port, pin)   (port)->IDR & (1<<(pin)) // devuelve 0 en en caso de haber un cero a la salida o !=0 si hay un 1

#define PIN_MODE2(reg, pin, mode) reg=(reg&~(0x3<<((pin)<<1)))|(mode<<((pin)<<1)) // Setea el registro del puerto correspondiente al modo del pin
#define PIN_OUTPUT(port, pin) 	PIN_MODE2((port)->MODER, pin, 0x1)				// en 1 de pone como salida
#define PIN_INPUT(port, pin)	PIN_MODE2((port)->MODER, pin, 0x0)  			// en 0 se pone como entrada


/*
 * Pone en modo output el pin 8 9 y 10   del puerto A
 * Pone en modo output el pin 3 4 5 y 10 del puerto B
 * Pone en modo output el pin 7 		 del puerto C
 * */
#define setWriteDir() { setReadDir(); \
                          GPIOA->MODER |=  0x150000; GPIOB->MODER |=  0x100540; GPIOC->MODER |=  0x4000; }
/*
 * Pone en modo input el pin 8 9 y 10   del puerto A
 * Pone en modo input el pin 3 4 5 y 10 del puerto B
 * Pone en modo input el pin 7 		 	del puerto C
 * */

#define setReadDir()  { GPIOA->MODER &= ~0x3F0000; GPIOB->MODER &= ~0x300FC0; GPIOC->MODER &= ~0xC000; }

#define RD_ACTIVE  PIN_LOW(RD_PORT, RD_PIN)
#define RD_IDLE    PIN_HIGH(RD_PORT, RD_PIN)
#define RD_OUTPUT  PIN_OUTPUT(RD_PORT, RD_PIN)
#define WR_ACTIVE  PIN_LOW(WR_PORT, WR_PIN)
#define WR_IDLE    PIN_HIGH(WR_PORT, WR_PIN)
#define WR_OUTPUT  PIN_OUTPUT(WR_PORT, WR_PIN)
#define CD_COMMAND PIN_LOW(CD_PORT, CD_PIN)
#define CD_DATA    PIN_HIGH(CD_PORT, CD_PIN)
#define CD_OUTPUT  PIN_OUTPUT(CD_PORT, CD_PIN)
#define CS_ACTIVE  PIN_LOW(CS_PORT, CS_PIN)
#define CS_IDLE    PIN_HIGH(CS_PORT, CS_PIN)
#define CS_OUTPUT  PIN_OUTPUT(CS_PORT, CS_PIN)
#define RESET_ACTIVE  PIN_LOW(RESET_PORT, RESET_PIN)
#define RESET_IDLE    PIN_HIGH(RESET_PORT, RESET_PIN)
#define RESET_OUTPUT  PIN_OUTPUT(RESET_PORT, RESET_PIN)


#define WR_ACTIVE2  {WR_ACTIVE; WR_ACTIVE;}
#define WR_ACTIVE4  {WR_ACTIVE2; WR_ACTIVE2;}
#define WR_ACTIVE8  {WR_ACTIVE4; WR_ACTIVE4;}
#define RD_ACTIVE2  {RD_ACTIVE; RD_ACTIVE;}
#define RD_ACTIVE4  {RD_ACTIVE2; RD_ACTIVE2;}
#define RD_ACTIVE8  {RD_ACTIVE4; RD_ACTIVE4;}
#define RD_ACTIVE16 {RD_ACTIVE8; RD_ACTIVE8;}
#define WR_IDLE2  {WR_IDLE; WR_IDLE;}
#define WR_IDLE4  {WR_IDLE2; WR_IDLE2;}
#define RD_IDLE2  {RD_IDLE; RD_IDLE;}
#define RD_IDLE4  {RD_IDLE2; RD_IDLE2;}

#define WR_STROBE { WR_ACTIVE; WR_IDLE; }         //PWLW=TWRL=50ns
#define RD_STROBE RD_IDLE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE   //PWLR=TRDL=150ns


#define true	1
#define false	0

#define MAX_X 480
#define MAX_Y 320

#define MIPI_DCS_REV1   (1<<0)
#define AUTO_READINC    (1<<1)
#define READ_BGR        (1<<2)
#define READ_LOWHIGH    (1<<3)
#define READ_24BITS     (1<<4)
#define XSA_XEA_16BIT   (1<<5)
#define READ_NODUMMY    (1<<6)
#define INVERT_GS       (1<<8)
#define INVERT_SS       (1<<9)
#define MV_AXIS         (1<<10)
#define INVERT_RGB      (1<<11)
#define REV_SCREEN      (1<<12)
#define FLIP_VERT       (1<<13)
#define FLIP_HORIZ      (1<<14)



void tft_init();
void reset(void);
uint16_t readID(void);

void setRotation(uint8_t r);
void invertDisplay(uint8_t i);
void setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
void drawRGBBitmap(uint16_t x, uint16_t y, uint16_t *bitmap, uint16_t w, uint16_t h);
void delay_f (uint32_t time);


uint16_t readReg(uint16_t reg, int8_t index);

uint32_t readReg40(uint16_t reg);

#ifdef __cplusplus
}
#endif

#endif /* __HX8347G_H */





