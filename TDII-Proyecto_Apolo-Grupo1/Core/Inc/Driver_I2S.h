/*
 * I2SDriver.h
 *
 *  Created on: Dec 14, 2023
 *      Author: Nic
 */

#ifndef INC_DRIVER_I2S_H_
#define INC_DRIVER_I2S_H_

#include <Control_ProcesamientoAudio.h>
#include "main.h"
#include "freertos.h"
#include "semphr.h"

typedef enum{IZQUIERDO=0,DERECHO=1}canal_t;

#define DATOS_P_CANAL 	BUFFER_SAMPLE_LEN
#define BUFF_SIZE 		DATOS_P_CANAL*2
#define PINPONG_BUFF_SIZE BUFF_SIZE*2
#define WS_PIN_Pin GPIO_PIN_12
#define WS_PIN_GPIO_Port GPIOB
//#############################################################################
//Init
//#############################################################################
void initI2SDriver();

//#############################################################################
//Task
//#############################################################################
void task_I2S_recieve();

//#############################################################################
//Callbacks
//#############################################################################
void callbackI2SRx_CMP();
void callbackI2SRx_HALF();
void callbackI2STx_CMP();
void callbackI2STx_HALF();
void callback_TIM1();
void callbackSPITx_CMP();
void callbackSPITx_HALF();

//#############################################################################
//Direccionamiento Ping - Pong Buffering (Double Buffering)
//#############################################################################
void switchBufferIn();
void switchBufferOutAB();
void switchBufferOutCD();

//#############################################################################
//Recibir Transmitir Datos
//#############################################################################
void recibirDatos();
void transmitirDatos();

//#############################################################################
//Funciones del usuario de la API
//#############################################################################
size_t readData_I2S(uint8_t canal, float32_t *buff, size_t lenToRead);
void writeData_I2S(uint8_t ampli, float32_t *datos, uint32_t lenToWrite,float gain);
void lberarSemaforoProc();

//#############################################################################
//Funcion para prueba de la API
//#############################################################################
void pruebaLoopback();

#endif /* INC_DRIVER_I2S_H_ */
