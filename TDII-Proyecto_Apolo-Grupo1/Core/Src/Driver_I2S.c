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
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;


//#############################################################################
//Double Buffering
//#############################################################################

//Puntero de datos a procesar IN
int16_t bufferIn[PINPONG_BUFF_SIZE];
//Buffer de entrada
static volatile int16_t *ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE/2)];

//Puntero de datos a procesar OUT
int16_t bufferOut_AB[PINPONG_BUFF_SIZE];
int16_t bufferOut_CD[PINPONG_BUFF_SIZE];//={170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0,170,0};
//Buffers de Salida AB
static volatile int16_t *ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE/2)];
static volatile int16_t *ptrProcessOutCD = &bufferOut_CD[(PINPONG_BUFF_SIZE/2)];

uint8_t initialized_I2S=0;
//#############################################################################
//Semaforos
//#############################################################################
SemaphoreHandle_t semRx_I2S;
SemaphoreHandle_t semTxAB_I2S;
SemaphoreHandle_t semTxCD_I2S;

TaskHandle_t I2S_Task_Handler = NULL;
extern SemaphoreHandle_t semDataReady;
//#############################################################################
//Init
//#############################################################################
void initI2SDriver() {


	semRx_I2S = xSemaphoreCreateBinary();
	semTxAB_I2S = xSemaphoreCreateBinary();
	semTxCD_I2S = xSemaphoreCreateBinary();

	xSemaphoreGive(semRx_I2S);
	xSemaphoreGive(semTxAB_I2S);
	xSemaphoreGive(semTxCD_I2S);
	//HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);

	// Busco un flanco ascendente (si se usa como Master al ST entonces se comentan estas lineas)
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS

	//vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 3);
	//vTaskDelay(pdMS_TO_TICKS(1000));

	while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
	while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS


	HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) bufferIn, PINPONG_BUFF_SIZE);

	xTaskCreate(task_I2S_recieve, "Audio In", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3 ,&I2S_Task_Handler);	// Recepcion datos I2S -> Prioridad una menos que la de procesamiento


}

void deinitI2SDriver(){

	if(I2S_Task_Handler != NULL){
		vTaskDelete(I2S_Task_Handler);
		I2S_Task_Handler = NULL;

		vSemaphoreDelete(semRx_I2S);
		vSemaphoreDelete(semTxAB_I2S);
		vSemaphoreDelete(semTxCD_I2S);

		HAL_I2S_DMAStop(&hi2s2);
		HAL_I2S_DMAStop(&hi2s3);
		HAL_SPI_DMAStop(&hspi1);
		initialized_I2S = 0;
	}



}

//#############################################################################
//CALLBACKS DMA
//#############################################################################


//uint32_t tim1_cuenta=0;
//extern TIM_HandleTypeDef htim2;


//SPI 1
void callbackSPITx_CMP() {
	ptrProcessOutCD = &bufferOut_CD[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxCD_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void callbackSPITx_HALF() {
	ptrProcessOutCD = &bufferOut_CD[(PINPONG_BUFF_SIZE / 2)];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxCD_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//I2S 2
void callbackI2SRx_CMP() {

	ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE / 2)];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//	tim1_cuenta = __HAL_TIM_GET_COUNTER(&htim2);
}
void callbackI2SRx_HALF() {

//	tim1_cuenta = 0 ;
//	__HAL_TIM_SET_COUNTER(&htim2,0);

	if(!initialized_I2S){
		HAL_I2S_Transmit_DMA(&hi2s3,(uint16_t *) bufferOut_AB, PINPONG_BUFF_SIZE);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);
		HAL_SPI_Transmit_DMA(&hspi1, (uint16_t*)bufferOut_CD, PINPONG_BUFF_SIZE);
		initialized_I2S=1;
	}

	ptrProcessIn = &bufferIn[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

//I2S 3
void callbackI2STx_CMP() {

	ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE / 2)];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void callbackI2STx_HALF() {

	ptrProcessOutAB = &bufferOut_AB[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


//#############################################################################
//Task recieve
//#############################################################################
void task_I2S_recieve() {

	//initI2SDriver();

	while(1) {

	// Revizo si se termino de recibir y enviar los datos
	xSemaphoreTake(semTxAB_I2S,portMAX_DELAY);

	//xSemaphoreTake(semTxCD_I2S,portMAX_DELAY);

	xSemaphoreTake(semRx_I2S,portMAX_DELAY);

	//Proceso los datos

	//Libero el semaforo asi la tarea de procesamiento arranca
	xSemaphoreGive(semDataReady);

	//pruebaLoopback(); // comentar si no se desea loopback
	}
}

//#############################################################################
//Get and push data
//#############################################################################
size_t readData_I2S(uint8_t canal, float32_t *buff, size_t lenToRead) {

	if(lenToRead > DATOS_P_CANAL)
		return 0;

	size_t i=0;
	for(i=0; i<lenToRead;i++) {
		buff[i] = (float32_t) ptrProcessIn[(2*i)+canal]  / 32768.0;
	}

	return i;
}


void writeData_I2S(uint8_t ampli, float32_t *datos, uint32_t lenToWrite,float gain) {
	//Valido escritura
	if(lenToWrite>DATOS_P_CANAL)
		return;

	//Escribo en las posiciones del vector de cada ampli


	if(ampli==CHANNEL_0){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutAB[i*2+IZQUIERDO] = (int16_t)(datos[i]*gain * 32768.0);
	}
	if(ampli==CHANNEL_1){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutAB[i*2+DERECHO] = (int16_t)(datos[i]* gain * 32768.0);
	}

	if(ampli==CHANNEL_2){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutCD[i*2+IZQUIERDO] = (int16_t)(datos[i]* gain * 32768.0);
	}

	if(ampli==CHANNEL_3){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutCD[i*2+DERECHO] = (int16_t)(datos[i]* gain * 32768.0);
	}

}


void pruebaLoopback() {

	for(int i =0;i<BUFF_SIZE;i++){
		ptrProcessOutAB[i] = ptrProcessIn[i];
	}

}


