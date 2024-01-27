/*
 * I2CDriver.c
 *
 *  Created on: 14 dic. 2023
 *      Author: Nic
 */
#include <Driver_I2S.h>

//#############################################################################
//Handlers perifericos
//#############################################################################
extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;
//extern SPI_HandleTypeDef hspi1;
//extern TIM_HandleTypeDef htim1;
static volatile int16_t data_ready_r,data_ready_t ;
//#############################################################################
//Double Buffering
//#############################################################################
//Buffers de entrada
int16_t bufferIn[PINPONG_BUFF_SIZE];
static volatile int16_t *ptrDmaIn = &bufferIn[0];
static volatile int16_t *ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE/2)-1];

//Buffers de Salida AB
int16_t bufferOut_AB[PINPONG_BUFF_SIZE];
static volatile int16_t *ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE/2)-1];
static volatile int16_t *ptrDmaOutAB = &bufferOut_AB[0];


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

	//HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);

	HAL_I2S_Transmit_DMA(&hi2s3,(uint16_t *) ptrDmaOutAB, PINPONG_BUFF_SIZE);

	// Busco un flanco ascendente (si se usa como Master al ST entonces se comentan estas lineas)
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS

	HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) ptrDmaIn, PINPONG_BUFF_SIZE);
}



//#############################################################################
//CALLBACKS DMA
//#############################################################################

//I2S 2
void callbackI2SRx_CMP() {
	if(data_ready_r){
		ptrDmaIn = &bufferIn[(PINPONG_BUFF_SIZE / 2)-1];
		ptrProcessIn = &bufferIn[0];
		data_ready_r = 0;
	}

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void callbackI2SRx_HALF() {
	if(data_ready_r){
		ptrDmaIn = &bufferIn[0];
		ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE / 2)-1];
		data_ready_r = 0;
	}

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//I2S 3
void callbackI2STx_CMP() {
	ptrDmaOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE / 2)-1];
	ptrProcessOutAB = &bufferOut_AB[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}
void callbackI2STx_HALF() {

	ptrDmaOutAB = &bufferOut_AB[0];
	ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE / 2)-1];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


//#############################################################################
//Task recieve
//#############################################################################
void task_I2S_recieve() {
	while(1) {


		//xSemaphoreTake(semDataReadyToSendDMA,portMAX_DELAY);

		xSemaphoreTake(semTxAB_I2S,portMAX_DELAY);
		//xSemaphoreTake(semTxCD_I2S,portMAX_DELAY);

		xSemaphoreTake(semRx_I2S,portMAX_DELAY);

	pruebaLoopback();
	}
}

//Cuando terminas de cargar buffers de salida llamas a esta función
void lberarSemaforoProc() {
	xSemaphoreGive(semDataReadyToSendDMA);
}


void pruebaLoopback() {

	for(int i =0;i<BUFF_SIZE;i++){
		ptrProcessOutAB[i] = ptrProcessIn[i];
	}
	data_ready_r = 1;
	data_ready_t = 1;

}
