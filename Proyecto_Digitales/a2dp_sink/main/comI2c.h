#ifndef COMI2C
#define COMI2C

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
//#include "..\build\bootloader\config\sdkconfig.h"
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/////////////////////////////////////// Defines ////////////////////////////////////////////////
#define LOAD_CONFIG_CMD 0x11
#define GET_CONFIG_CMD 0x12
#define STATE_COMAND_CMD 0x13
#define SAVE_FLASH_CMD 0x14
#define NEXT_SONG_CMD 0x15
#define PREVIOUS_SONG_CMD 0x16
#define STOP_SONG_CMD 0x17
#define LOUD_CONFIG_CMD 0x18

#define DEBUG 1 // Poner en 1 para debug
#define SLAVE_ADDR 0x18
#define DATA_LENGTH 512                  /*!< Data buffer length of test buffer */
#define STRUCT_LENGHT 64 


#define I2C_SLAVE_SCL_IO GPIO_NUM_19                        /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO GPIO_NUM_18                        /*!< gpio number for i2c slave data */
#define I2C_SLAVE_NUM I2C_NUM_0                             /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave rx buffer size */
#define ESP_SLAVE_ADDR SLAVE_ADDR             /*!< ESP32 slave address, you can set any 7bit value */
#define I2C_MASTER_NUM I2C_NUM_0  

#define led1 2

// Errores defines

#define ERROR 1
#define INSUFICIENT_DATA 2
#define INVALID_COMMAND 3
#define COMAND_FAILED 0xFF
#define COMAND_OK 0x33

typedef struct{
	int8_t _20hz;
	int8_t _200hz;
	int8_t _500hz;
	int8_t _1000hz;
	int8_t _4000hz;
	int8_t _8000hz;
	int8_t _16000hz;
}Equalizer;

typedef struct{
	uint8_t channel;
	uint8_t state;
	uint8_t channel_volume;
	uint8_t type_equalizer;
	uint8_t channel_audio; 
}AudioOutput;

typedef struct {
	uint8_t system_type;
	uint8_t equalization_type;
	uint8_t max_volume;
	uint8_t loudness_state;
}GeneralConfig;

typedef struct{
	uint8_t type_in; 
	uint8_t channel;
	int8_t channel_balance; 
}AudioInput;

typedef struct {
	uint8_t Isinitailized;
	uint8_t main_volume;
	Equalizer general_equalizer;
	Equalizer general_equalizer_pers;
	AudioInput audio_input;
	AudioOutput audio_output[4];
	AudioOutput audio_output_pers[4];
	GeneralConfig general_config;
	uint8_t channelInUse; 
}UserData;

typedef union  {
    UserData user_data;
    uint8_t datosRaw[STRUCT_LENGHT]; 
}UnionUserData;

/////////////////////////////////////// Prototypes ////////////////////////////////////////////////

void i2c_task (void* parameters);
uint8_t anlyses_message(uint8_t command);
void print_data(UserData data);
esp_err_t init_nvs(void);
esp_err_t read_nvs(UserData *value);
esp_err_t write_nvs(UserData *value);
void comi2c_start_up();


#endif