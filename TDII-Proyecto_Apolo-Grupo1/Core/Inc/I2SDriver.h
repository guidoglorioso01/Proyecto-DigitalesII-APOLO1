/*
 * I2SDriver.h
 *
 *  Created on: Dec 14, 2023
 *      Author: Nic
 */

#ifndef INC_I2SDRIVER_H_
#define INC_I2SDRIVER_H_

#include "main.h"
#include "freertos.h"
#include "semphr.h"
#include "FilterManagement.h"

typedef enum{IZQUIERDO=0,DERECHO=1}canal_t;

#define DATOS_P_CANAL 	1000
#define BUFF_SIZE 		DATOS_P_CANAL*2


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
void callbackI2SRx();
void callbackI2STx();
void callbackSPITx();


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
size_t readData_I2S(uint8_t canal, q15_t *buff, size_t lenToRead);
void writeData_I2S(uint8_t ampli, int16_t *datos, uint32_t lenToWrite);
void lberarSemaforoProc();

//#############################################################################
//Funcion para prueba de la API
//#############################################################################
void pruebaLoopback();

#endif /* INC_I2SDRIVER_H_ */
