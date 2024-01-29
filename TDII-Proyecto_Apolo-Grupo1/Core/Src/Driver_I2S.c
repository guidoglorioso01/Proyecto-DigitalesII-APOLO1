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


//#############################################################################
//Double Buffering
//#############################################################################

//Puntero de datos a procesar IN
int16_t bufferIn[PINPONG_BUFF_SIZE];
//Buffer de entrada
static volatile int16_t *ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE/2)-1];

//Puntero de datos a procesar OUT
int16_t bufferOut_AB[PINPONG_BUFF_SIZE];
//Buffers de Salida AB
static volatile int16_t *ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE/2)-1];



//#############################################################################
//Semaforos
//#############################################################################
SemaphoreHandle_t semRx_I2S;
SemaphoreHandle_t semTxAB_I2S;
SemaphoreHandle_t semTxCD_I2S;
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
	while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
	while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS

	HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) bufferIn, PINPONG_BUFF_SIZE);

}



//#############################################################################
//CALLBACKS DMA
//#############################################################################

//I2S 2
void callbackI2SRx_CMP() {

	ptrProcessIn = &bufferIn[(PINPONG_BUFF_SIZE / 2)-1];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}
void callbackI2SRx_HALF() {
	static uint8_t i=1;
	if(i){
		HAL_I2S_Transmit_DMA(&hi2s3,(uint16_t *) bufferOut_AB, PINPONG_BUFF_SIZE);
		i=0;
	}

	ptrProcessIn = &bufferIn[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

//I2S 3
void callbackI2STx_CMP() {

	ptrProcessOutAB = &bufferOut_AB[(PINPONG_BUFF_SIZE / 2)-1];

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
	while(1) {

	// Revizo si se termino de recibir y enviar los datos
	xSemaphoreTake(semTxAB_I2S,portMAX_DELAY);

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
		buff[i] = (float32_t) ptrProcessIn[(2*i)+canal];
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
			ptrProcessOutAB[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
	}
	if(ampli==CHANNEL_1){
		for(uint32_t i=0; i<lenToWrite; i++)
			ptrProcessOutAB[i*2+DERECHO] = (int16_t)datos[i]*gain;
	}

// Por el momento solo hay dos canales funcionando

//	if(ampli==CHANNEL_2){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutCD[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
//	}
//
//	if(ampli==CHANNEL_3){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutCD[i*2+DERECHO] = (int16_t)datos[i]*gain;
//	}

}


void pruebaLoopback() {

	for(int i =0;i<BUFF_SIZE;i++){
		ptrProcessOutAB[i] = ptrProcessIn[i];
	}

}


