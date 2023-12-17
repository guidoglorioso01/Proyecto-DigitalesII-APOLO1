/*
 * I2CDriver.c
 *
 *  Created on: 14 dic. 2023
 *      Author: Nic
 */
#include "I2SDriver.h"

//#############################################################################
//Handlers perifericos
//#############################################################################
extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;

//#############################################################################
//Double Buffering
//#############################################################################
//Buffers de entrada
int16_t bufferInA[BUFF_SIZE];
int16_t bufferInB[BUFF_SIZE];
int16_t *ptrDmaIn = bufferInA;
int16_t *ptrProcessIn = bufferInB;

//Buffers de Salida AB
int16_t bufferOut1_AB[BUFF_SIZE];
int16_t bufferOut2_AB[BUFF_SIZE];
int16_t *ptrProcessOutAB = bufferOut1_AB;
int16_t *ptrDmaOutAB = bufferOut2_AB;

//Buffers Salida CD
int16_t bufferOut1_CD[BUFF_SIZE];
int16_t bufferOut2_CD[BUFF_SIZE];
int16_t *ptrProcessOutCD = bufferOut1_CD;
int16_t *ptrDmaOutCD = bufferOut2_CD;

//#############################################################################
//Semaforos
//#############################################################################
SemaphoreHandle_t semRx_I2S;
SemaphoreHandle_t semTxAB_I2S;
SemaphoreHandle_t semTxCD_I2S;
SemaphoreHandle_t semDataReadyToSendDMA;
SemaphoreHandle_t semDataReadyToProc;

//#############################################################################
//Init
//#############################################################################
void initI2SDriver() {
	semRx_I2S = xSemaphoreCreateBinary();
	semTxAB_I2S = xSemaphoreCreateBinary();
	semTxCD_I2S = xSemaphoreCreateBinary();
	semDataReadyToSendDMA = xSemaphoreCreateBinary();
	semDataReadyToProc = xSemaphoreCreateBinary();

	xSemaphoreGive(semRx_I2S);
	xSemaphoreGive(semTxAB_I2S);
	xSemaphoreGive(semTxCD_I2S);
	xSemaphoreGive(semDataReadyToSendDMA);
	xSemaphoreTake(semDataReadyToProc, 0);

	HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);
}


//#############################################################################
//Direccionamiento Double Buffering
//#############################################################################
void switchBufferIn() {
	if(ptrProcessIn==bufferInA) {
		ptrProcessIn = bufferInB;
		ptrDmaIn = bufferInA;
	} else {
		ptrProcessIn = bufferInA;
		ptrDmaIn = bufferInB;
	}
}

void switchBufferOutAB() {
	if(ptrProcessOutAB==bufferOut1_AB) {
		ptrProcessOutAB = bufferOut2_AB;
		ptrDmaOutAB = bufferOut1_AB;
	} else {
		ptrProcessOutAB = bufferOut1_AB;
		ptrDmaOutAB = bufferOut2_AB;
	}
}

void switchBufferOutCD() {
	if(ptrProcessOutCD==bufferOut1_CD) {
		ptrProcessOutCD = bufferOut2_CD;
		ptrDmaOutCD = bufferOut1_CD;
	} else {
		ptrProcessOutCD = bufferOut1_CD;
		ptrDmaOutCD = bufferOut2_CD;
	}
}

//#############################################################################
//Lectura y escritura de buffers
//#############################################################################
size_t readData_I2S(uint8_t canal, q15_t *buff, size_t lenToRead) {
	static uint8_t fIzq = 0, fDer = 0;
	if(canal == IZQUIERDO)
		fIzq = 1;

	if(canal == DERECHO)
		fDer = 1;

	if(lenToRead > DATOS_P_CANAL)
		return 0;

	if(fDer&&fIzq) {
		xSemaphoreTake(semDataReadyToProc,portMAX_DELAY);
		fDer = 0;
		fIzq = 0;
	}
	size_t i=0;
	for(i=0; i<lenToRead;i++) {
		buff[i] = (q15_t) ptrProcessIn[(2*i)+canal];
	}

	return i;
}

/*Modificar*/ // los ifs
void writeData_I2S(uint8_t ampli, int16_t *datos, uint32_t lenToWrite,uint8_t gain) {
	//Valido escritura
	if(lenToWrite>DATOS_P_CANAL)
		return;

	//Escribo en las posiciones del vector de cada ampli


	if(ampli==CHANNEL_0){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutAB[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
	}
	if(ampli==CHANNEL_1){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutAB[i*2+DERECHO] = (int16_t)datos[i]*gain;
	}

	if(ampli==CHANNEL_2){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutCD[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
	}

	if(ampli==CHANNEL_3){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutCD[i*2+DERECHO] = (int16_t)datos[i]*gain;
	}

}

//#############################################################################
//Recibir Transmitir Datos
//#############################################################################
void recibirDatos() {
	switchBufferIn();
	HAL_I2S_Receive_DMA(&hi2s2, ptrDmaIn, BUFF_SIZE);
}

void transmitirDatos() {
	switchBufferOutAB();
	switchBufferOutCD();

	HAL_I2S_Transmit_DMA(&hi2s3, ptrDmaOutAB, BUFF_SIZE);

	//Esta para datos de 8bits auqnue lo configure como trama de 16
	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)ptrDmaOutCD, 2*BUFF_SIZE);
}

//#############################################################################
//CALLBACKS DMA
//#############################################################################

//I2S 2
void callbackI2SRx() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//I2S 3
void callbackI2STx() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//SPI 1
void callbackSPITx() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxCD_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//#############################################################################
//Task recieve
//#############################################################################
void task_I2S_recieve() {
	while(1) {
		//Proceso
		xSemaphoreTake(semDataReadyToSendDMA,portMAX_DELAY);

		//Transmito
		xSemaphoreTake(semTxAB_I2S,portMAX_DELAY);
		xSemaphoreTake(semTxCD_I2S,portMAX_DELAY);

		//Recibo
		xSemaphoreTake(semRx_I2S,portMAX_DELAY);

		//Cuando cumplí con las 3 al mismo tiempo puedo volver a repetir el proceso
		//En las interrupciones:
			//give(semRx_I2S)
			//give(semTxAB_I2S)
			//give(semTxCD_I2S)
		//En funcion de procesamiento
			//give(semProc_I2S) con lberarSemaforoProc()

		recibirDatos();
		transmitirDatos();

		//Aviso que ya podés volver a leer
		xSemaphoreGive(semDataReadyToProc);

		//Procesamiento de prueba en esta misma tarea
		//vTaskDelay(pdMS_TO_TICKS(10));
		//pruebaLoopback();
	}
}

//Cuando terminas de cargar buffers de salida llamas a esta función
void lberarSemaforoProc() {
	xSemaphoreGive(semDataReadyToSendDMA);
}

//#############################################################################
// Ejemplo de procesamiento (no se usa en ninun lado esta funcion)
//#############################################################################
q15_t dataIzq[DATOS_P_CANAL];
q15_t dataDer[DATOS_P_CANAL];

void pruebaLoopback() {
	//Leo
	readData_I2S(IZQUIERDO, dataIzq, DATOS_P_CANAL);
	readData_I2S(DERECHO,   dataDer, DATOS_P_CANAL);

	//Escribo
	writeData_I2S(CHANNEL_0, dataIzq, DATOS_P_CANAL,1);
	writeData_I2S(CHANNEL_1, dataIzq, DATOS_P_CANAL,1);

	writeData_I2S(CHANNEL_2, dataDer, DATOS_P_CANAL,1);
	writeData_I2S(CHANNEL_3, dataDer, DATOS_P_CANAL,1);

	//Libero semaforo de procesamiento
	lberarSemaforoProc();
}
