/*
 * user_info.h
 *
 *  Created on: 18 sep. 2023
 *      Author: guido
 */
//#include <FilterManagement.h>
#include <Driver_I2C.h>
#include "stm32f4xx_hal.h"

#include <math.h>

#ifndef INC_USER_INFO_H_
#define INC_USER_INFO_H_

#define OFFSET_GRAFICO 15

#define FLAT_PROFILE(vec) do { \
    (vec)[0] = 0; \
    (vec)[1] = 0; \
    (vec)[2] = 0; \
    (vec)[3] = 0; \
    (vec)[4] = 0; \
    (vec)[5] = 0; \
    (vec)[6] = 0; \
} while(0)

#define POP_PROFILE(vec) do { \
    (vec)[0] = 0; \
    (vec)[1] = 3; \
    (vec)[2] = 6; \
    (vec)[3] = 3; \
    (vec)[4] =-3; \
    (vec)[5] =-3; \
    (vec)[6] = 0; \
} while(0)
#define ROCK_PROFILE(vec) do { \
    (vec)[0] = 0; \
    (vec)[1] = 3; \
    (vec)[2] = 6; \
    (vec)[3] = 0; \
    (vec)[4] =-3; \
    (vec)[5] = 3; \
    (vec)[6] = 0; \
} while(0)
#define CINE_PROFILE(vec) do { \
    (vec)[0] = 6; \
    (vec)[1] = 3; \
    (vec)[2] =-3; \
    (vec)[3] =-3; \
    (vec)[4] = 3; \
    (vec)[5] = 6; \
    (vec)[6] = 6; \
} while(0)
#define ELECTRO_PROFILE(vec) do { \
    (vec)[0] = 3; \
    (vec)[1] = 6; \
    (vec)[2] = 3; \
    (vec)[3] =-3; \
    (vec)[4] = 6; \
    (vec)[5] = 6; \
    (vec)[6] = 3; \
} while(0)
#define VOCAL_PROFILE(vec) do { \
    (vec)[0] =-3; \
    (vec)[1] =-3; \
    (vec)[2] = 3; \
    (vec)[3] = 6; \
    (vec)[4] = 3; \
    (vec)[5] =-3; \
    (vec)[6] =-3; \
} while(0)

#define CROSSOVER_LP1(vec) do { \
	    (vec)[0] = 	0; \
	    (vec)[1] = 	3; \
	    (vec)[2] =  16; \
	    (vec)[3] =  27; \
	    (vec)[4] =  52; \
	    (vec)[5] =  64; \
	    (vec)[6] =  76; \
	} while(0)
//frecuencia de corte: 500Hz
#define CROSSOVER_LP2(vec) do { \
	    (vec)[0] = 0; \
	    (vec)[1] = 0; \
	    (vec)[2] = 3; \
	    (vec)[3] = 12; \
	    (vec)[4] = 36; \
	    (vec)[5] = 48; \
	    (vec)[6] = 60; \
	} while(0)

//frecuencia de corte: 200Hz
//frecuencia de corte: 6000Hz
#define CROSSOVER_BP1(vec) do { \
	    (vec)[0] = 40; \
	    (vec)[1] = 3; \
	    (vec)[2] = 0; \
	    (vec)[3] = 0; \
	    (vec)[4] = 1; \
	    (vec)[5] = 6; \
	    (vec)[6] = 18; \
	} while(0)

//frecuencia de corte: 500Hz
//frecuencia de corte: 7000Hz
#define CROSSOVER_BP2(vec) do { \
	    (vec)[0] = 56; \
	    (vec)[1] = 17; \
	    (vec)[2] = 3; \
	    (vec)[3] = 0; \
	    (vec)[4] = 0; \
	    (vec)[5] = 5; \
	    (vec)[6] = 16; \
	} while(0)

//frecuencia de corte: 8000Hz
#define CROSSOVER_HP1(vec) do { \
	    (vec)[0] = 100; \
	    (vec)[1] = 64; \
	    (vec)[2] = 48; \
	    (vec)[3] = 36; \
	    (vec)[4] = 12; \
	    (vec)[5] = 3; \
	    (vec)[6] = 0; \
	} while(0)
//frecuencia de corte: 10000Hz
#define CROSSOVER_HP2(vec) do { \
	    (vec)[0] = 100; \
	    (vec)[1] = 67; \
	    (vec)[2] = 52; \
	    (vec)[3] = 40; \
	    (vec)[4] = 16; \
	    (vec)[5] = 5; \
	    (vec)[6] = 0; \
	} while(0)

#define CROSSOVER_FLAT(vec) do { \
	    (vec)[0] = 0; \
	    (vec)[1] = 0; \
	    (vec)[2] = 0; \
	    (vec)[3] = 0; \
	    (vec)[4] = 0; \
	    (vec)[5] = 0; \
	    (vec)[6] = 0; \
	} while(0)


typedef struct{
	int8_t _20hz;
	int8_t _200hz;
	int8_t _500hz;
	int8_t _1000hz;
	int8_t _4000hz;
	int8_t _8000hz;
	int8_t _16000hz;
}Equalizer;

//type_in options
#define AUX 0
#define BLUT 1

//channel_options
#define LEFT_CHANNEL_INPUT 		0
#define RIGHT_CHANNEL_INPUT 	1
#define STEREO_CHANNEL_INPUT 	2

typedef struct{
	uint8_t type_in; // Tipo de entrada (auxiliar o bluetooth)
	uint8_t channel;
	uint8_t channel_balance; // 127 indica balance = 0
}AudioInput;

#define CHANNEL_0 0
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3
#define LEFT_CHANNEL_OUTPUT 		0
#define RIGHT_CHANNEL_OUTPUT 		1
#define ON 1
#define OFF 0
#define MAX_VOLUME 100

//Max atenuacion 60db
//Todo se mide en atenuacion
//frecuencia de corte: 200Hz


#define TYPE_LP1 1
#define TYPE_LP2 2
#define TYPE_BP1 3
#define TYPE_BP2 4
#define TYPE_HP1 5
#define TYPE_HP2 6
#define TYPE_FLAT 0

typedef struct{
	uint8_t channel;
	uint8_t state;
	uint8_t channel_volume;
	uint8_t type_equalizer;
	uint8_t channel_audio; // indica si saco canal izquierdo o derecho
}AudioOutput;

#define SYSTEM_2_0 	0
#define SYSTEM_2_1 	1
#define SYSTEM_3_0 	2
#define SYSTEM_3_1 	3
#define SYSTEM_4_0 	4
#define SYSTEM_PERS 5
#define EQ_PROF_PLANO 	0
#define EQ_PROF_POP 	1
#define EQ_PROF_ROCK 	2
#define EQ_PROF_ELECTRO 3
#define EQ_PROF_VOCAL 	4
#define EQ_PROF_CINE 	5
#define EQ_PROF_PERS 	6

typedef struct {
	uint8_t system_type;
	uint8_t equalization_type;
	uint8_t max_volume;
	uint8_t loudness_state;
}GeneralConfig;

#define INITIALIZED 3
typedef struct {
	uint8_t Isinitailized;
	uint8_t main_volume;
	Equalizer general_equalizer;
	Equalizer general_equalizer_pers;
	AudioInput audio_input;
	AudioOutput audio_output[4];
	AudioOutput audio_output_pers[4];
	GeneralConfig general_config;
	uint8_t channelInUse; //workarround para problema en croosover edit
}UserData;

typedef struct Punto_t {
    double x;
    double y;
}Punto_t;

#define CANT_FREQ_FILTRO 7
void calcularPuntos(Punto_t puntos[],uint8_t n, uint8_t m, Punto_t *res);

void get_pol_coef(double x1, double x2, double y1, double y2,int order, double *coef);

void config_eq(int8_t *values,Equalizer *position);
void init_userdata();
void reset_userdata();
void save_user_data();

void set_user_data(UserData new_data);
void get_user_data(UserData *read_data);


#endif /* INC_USER_INFO_H_ */
