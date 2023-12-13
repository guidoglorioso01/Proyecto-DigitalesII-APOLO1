/*
 * I2CDriver.c
 *
 *  Created on: 14 oct. 2023
 *      Author: guido
 */
#include "I2CDriver.h"

UnionUserData local_data; // La uso para escribir en la estructura
uint8_t Buffer_tx[BUFFER_LEN];
uint8_t Buffer_rx[BUFFER_LEN];

uint8_t save_config_esp(){
	uint8_t nbr_of_try = 0;
	do{
		Buffer_tx[0] = LOAD_CONFIG_CMD;
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de guardar

		get_user_data(&local_data.datos);
		//memcpy(&local_data.datos,&user_data,STRUCT_LENGHT);// Copio los datos del usuario en la estructura

		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, local_data.datosRaw , STRUCT_LENGHT ,HAL_MAX_DELAY); // Envio los datos del usuario

		vTaskDelay(300); // Cambiar por un vTaskDelay

		Buffer_tx[0] = STATE_COMAND_CMD;
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de verificar estado previo

		vTaskDelay(300);

		HAL_I2C_Master_Receive(&hi2c1,SLAVE_ADDR,Buffer_rx,1,HAL_MAX_DELAY);

		nbr_of_try++;
	}while((Buffer_rx[0] != COMAND_OK)&&(nbr_of_try<MAX_CMD_SEND));

	if(nbr_of_try >= MAX_CMD_SEND)
		return ERROR;
	return SUCCESS;

}


uint8_t get_config_esp(){
	uint8_t nbr_of_try = 0;
	do{
		Buffer_tx[0] = GET_CONFIG_CMD;
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de guardar

		vTaskDelay(300); // Cambiar por un vTaskDelay

		HAL_I2C_Master_Receive(&hi2c1,SLAVE_ADDR,local_data.datosRaw,STRUCT_LENGHT,HAL_MAX_DELAY);

		Buffer_tx[0] = STATE_COMAND_CMD;

		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de verificar estado previo

		vTaskDelay(300);

		HAL_I2C_Master_Receive(&hi2c1,SLAVE_ADDR,Buffer_rx,1,HAL_MAX_DELAY);

		nbr_of_try++;
	}while((Buffer_rx[0] != COMAND_OK)&&(nbr_of_try<MAX_CMD_SEND));

	if(nbr_of_try >= MAX_CMD_SEND)
		return ERROR;
	set_user_data(local_data.datos);
	//memcpy(&user_data,&local_data.datos,STRUCT_LENGHT);// Copio los datos del usuario en la estructura

	return SUCCESS;
}


uint8_t send_cmd_esp(uint8_t CMD){
	uint8_t nbr_of_try = 0;
	do{
		Buffer_tx[0] = CMD;
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de guardar

		Buffer_tx[0] = STATE_COMAND_CMD;
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDR, Buffer_tx , 1 ,HAL_MAX_DELAY); // Envio comando de verificar estado previo

		vTaskDelay(300);

		HAL_I2C_Master_Receive(&hi2c1,SLAVE_ADDR,Buffer_rx,1,HAL_MAX_DELAY);

		nbr_of_try++;
	}while((Buffer_rx[0] != COMAND_OK)&&(nbr_of_try<MAX_CMD_SEND));

	if(nbr_of_try >= MAX_CMD_SEND)
		return ERROR;
	return SUCCESS;
}

void give_sem_save_data(){
	xSemaphoreGive(semSaveData);
}
