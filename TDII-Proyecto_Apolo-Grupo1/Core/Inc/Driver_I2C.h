/*
 * I2CDriver.h
 *
 *  Created on: 14 oct. 2023
 *      Author: guido
 */


#ifndef INC_DRIVER_I2C_H_
#define INC_DRIVER_I2C_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <Control_DatosUsuario.h>
#include <string.h>
#include "cmsis_os.h"


#define STRUCT_LENGHT 64

#define BUFFER_LEN 50
#define SLAVE_ADDR (0x18 << 1)

#define LOAD_CONFIG_CMD 0x11
#define GET_CONFIG_CMD 0x12
#define STATE_COMAND_CMD 0x13
#define SAVE_FLASH_CMD 0x14
#define NEXT_SONG_CMD 0x15
#define PREVIOUS_SONG_CMD 0x16
#define STOP_SONG_CMD 0x17
#define LOUD_CONFIG_CMD 0x18
#define GET_MUSIC_STATE_CMD 0x19

#define COMAND_FAILED 0xFF
#define COMAND_OK 0x33
#define MAX_CMD_SEND 5

typedef union  {
    UserData datos;
    uint8_t datosRaw[STRUCT_LENGHT];
}UnionUserData;

uint8_t save_config_esp();
uint8_t get_config_esp();
uint8_t send_cmd_esp(uint8_t CMD);
uint8_t send_volume_esp(uint8_t volume, uint8_t ctr_loudness_state);
void give_sem_save_data();
uint8_t get_music_estate_esp();


extern I2C_HandleTypeDef hi2c1;
extern SemaphoreHandle_t semSaveData;


#ifdef __cplusplus
}
#endif

#endif /* INC_DRIVER_I2C_H_ */



