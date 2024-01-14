/*
 * FilterCoefs.h
 *
 *  Created on: 22 oct. 2023
 *      Author: guido
 */

#define ARM_MATH_CM4
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include <math.h>
#include <Control_DatosUsuario.h>
#include <Driver_I2S.h>


#ifndef INC_FILTERCOEFS_H_
#define INC_FILTERCOEFS_H_


#ifdef __cplusplus
 extern "C" {
#endif

 // Si trabajo con Q15
 #define IIR_CANT_COEF 			6
 // Si trabajo con Q31
 //#define IIR_CANT_COEF 		5

 #define NUMBER_SOS_EQ 		7
 #define IIR_TAP_NUM_EQ 		IIR_CANT_COEF*NUMBER_SOS_EQ	// -> IIR_CANT_COEF coef por SOS , 7 SOS por IIR
 #define IIR_TAP_NUM_CROSS 	IIR_CANT_COEF 	// -> IIR_CANT_COEF coef por SOS , 1 SOS por IIR


 #define BUFFER_SAMPLE_LEN   1024
 #define BLOCK_SIZE_FLOAT BUFFER_SAMPLE_LEN


 typedef enum{Q15=0,Q32=1}type_data_filter;

 void give_sem_save_volume();

// Funciones diseño filtros RBJ

float32_t convertBwToOctaves(double fc_inf, double fc_sup);
float32_t get_alpha(float32_t w0, float32_t Q);
float32_t get_A(float32_t dbGain);
float32_t get_w0(float32_t f0);
float32_t get_alpha_fromBW(float32_t w0, float32_t BW);
void low_shelf_pass(float32_t f0, float32_t dbGain, float32_t Q, float32_t* b, float32_t* a);
void high_shelf_pass(float32_t f0, float32_t dbGain, float32_t Q, float32_t* b, float32_t* a);
void peaking_filter(float32_t f0, float32_t dbGain, float32_t BW, float32_t* b, float32_t* a);
void band_pass(float32_t f0, float32_t dbGain, float32_t BW, float32_t* b, float32_t* a);
void low_pass(float32_t f0, float32_t Q, float32_t* b, float32_t* a);
void high_pass(float32_t f0, float32_t Q, float32_t* b, float32_t* a);
void designFilter(float32_t gainsdB[7], float32_t Q_BW[7],float32_t *sos,type_data_filter type);
void designCrossover(float32_t *buff,uint8_t Type);


//Funciones de drivers de filtrado

void filter_function_eq(q15_t *buff_in,q15_t *buff_out);
void filter_function_co(uint8_t channel, q15_t *buff_in,q15_t *buff_out);
void filter_initialization_eq(uint8_t Type);
void filter_initialization_co(uint8_t channel,uint8_t Type);
void filter_init_system();
void filter_init_all();

// Process functions
void process_filter();
void process_set_gains();


#ifdef __cplusplus
}
#endif

#endif /* INC_FILTERCOEFS_H_ */
