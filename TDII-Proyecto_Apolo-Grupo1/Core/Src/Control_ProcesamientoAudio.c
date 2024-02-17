/*
 * FilterCoefs.c
 *
 *  Created on: 22 oct. 2023
 *      Author: guido
 */
#include <Control_ProcesamientoAudio.h>
#include <Driver_I2C.h>
#include <Driver_I2S.h>

//#############################################################################
// Variables Globales
//#############################################################################
extern SemaphoreHandle_t semCalcVolume;

float32_t Coef_IIR_EQ[IIR_TAP_NUM_EQ];
float32_t iir_taps_CO0	[IIR_TAP_NUM_CROSS] ;
float32_t iir_taps_CO1	[IIR_TAP_NUM_CROSS] ;
float32_t iir_taps_CO2	[IIR_TAP_NUM_CROSS] ;
float32_t iir_taps_CO3	[IIR_TAP_NUM_CROSS] ;
float32_t gain_channels[4];

float32_t iir_eq_state[4*NUMBER_SOS_EQ];
float32_t iir_ch1_state[4],  iir_ch2_state[4], iir_ch3_state[4],  iir_ch4_state[4];

arm_biquad_casd_df1_inst_f32 iir_eq_settings;
arm_biquad_casd_df1_inst_f32 iir_ch1_settings,  iir_ch2_settings, iir_ch3_settings,  iir_ch0_settings;

float32_t buff_filtrado1[BUFFER_SAMPLE_LEN];
float32_t buff_filtrado2[BUFFER_SAMPLE_LEN];

//#############################################################################
// Funciones de filtrado
//#############################################################################

/*
 * Funcion que realiza el filtrado de ecualizacion
 *
 * parametros:
 * buff_in-> datos de entrada, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * buff_out-> datos de salida, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * */

void filter_function_eq(float32_t *buff_in,float32_t *buff_out){

	/*
	 * NOTAS para ver:
	 * debo verificar que si el filtro tiene ganancia que no me haga overflow
	 *
	 * */


	UserData buff;
	get_user_data(&buff);

	if(buff.general_config.equalization_type == EQ_PROF_PLANO){
		for(int i=0;i<BUFFER_SAMPLE_LEN;i++)
			buff_out[i] = buff_in[i];
		return;
	}
	float32_t check = buff_in[0];
	uint8_t flag =0;
	for(int i=0;i<BUFFER_SAMPLE_LEN;i++){
		if(buff_in[i] != check ){
			flag = 1;
			break;
		}
	}
	if(flag == 1)
		arm_biquad_cascade_df1_f32(&iir_eq_settings, buff_in, buff_out, BUFFER_SAMPLE_LEN);
	else
		for(int i=0;i<BUFFER_SAMPLE_LEN;i++)
			buff_out[i] = 0;

}


/*
 * Funcion que realiza el filtrado de ecualizacion
 *
 * parametros:
 * Channel -> CHANNEl_0 .... CHANNEL_3
 * buff_in-> datos de entrada, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * buff_out-> datos de salida, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * */

void filter_function_co(uint8_t channel, float32_t *buff_in,float32_t *buff_out){

	/*
	 * Tengo que convertir un dato uint16 a q15?
	 * */
	UserData buff;
	get_user_data(&buff);
	// En caso de el filtro configurado se Respuesta plana, copio entrada en salida
	uint8_t filter_type = buff.audio_output[channel].type_equalizer;
	if(filter_type == TYPE_FLAT){
		//memcpy(buff_out,buff_in,BLOCK_SIZE_FLOAT);
	for(int i=0;i<BUFFER_SAMPLE_LEN;i++)
			buff_out[i] = buff_in[i];
		return;
	}


	switch(channel){
	case CHANNEL_0:
		arm_biquad_cascade_df1_f32(&iir_ch0_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_1:
		arm_biquad_cascade_df1_f32(&iir_ch1_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_2:
		arm_biquad_cascade_df1_f32(&iir_ch2_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_3:
		arm_biquad_cascade_df1_f32(&iir_ch3_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	}
}

/*
 * Funcion que inicializa Filtros de ecualizacion
 *
 * parametros:
 * Lado -> LADO_IZQ ... LADO_DER
 * taps-> Puntero a los taps del filtro definidos en este archivo
 * */

void filter_initialization_eq(uint8_t Type){

	UserData buff;
	get_user_data(&buff);
	float32_t sos_float[IIR_TAP_NUM_EQ]; // buffer para los coeficientes calculados
	float32_t sos_float_check_max[IIR_TAP_NUM_EQ];
	float32_t q_bw[7] = {0.7,1.5,1.5,1.5,0.8,1.3,0.6};
	Equalizer atenuaciones;

	switch(Type){
	case EQ_PROF_PERS:
		atenuaciones = buff.general_equalizer_pers; // Caso personalizado lo tomo de la config
		break;
	case EQ_PROF_PLANO: // no inicializo el filtro
		return;
		break;
	default: // Si es cualquier otro filtro lo tomo del ecualizador general
		atenuaciones = buff.general_equalizer;
		break;
	}

	float32_t gain[7] = {atenuaciones._20hz,
					atenuaciones._200hz,
					atenuaciones._500hz,
					atenuaciones._1000hz,
					atenuaciones._4000hz,
					atenuaciones._8000hz,
					atenuaciones._16000hz};

	designFilter(gain,q_bw,Coef_IIR_EQ,FLOAT);

	arm_biquad_cascade_df1_init_f32(&iir_eq_settings,(uint32_t)(IIR_TAP_NUM_EQ / IIR_CANT_COEF),(float32_t *)Coef_IIR_EQ , iir_eq_state);


}

/*
 * Funcion que inicializa Filtros de cross over
 *
 * parametros:
 * Channel -> CHANNEl_0 .... CHANNEL_3
 * taps-> Puntero a los taps del filtro definidos en este archivo
 * */
void filter_initialization_co(uint8_t channel,uint8_t Type){

	float32_t buff[IIR_TAP_NUM_CROSS];

	if(Type == TYPE_FLAT)
		return;

	designCrossover(buff, Type);

	switch(channel){
	case CHANNEL_0:
		for(int i =0;i<IIR_TAP_NUM_CROSS;i++)
			iir_taps_CO0[i]=buff[i];
		arm_biquad_cascade_df1_init_f32(&iir_ch0_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(float32_t *) iir_taps_CO0, &iir_ch1_state[0]);
	break;
	case CHANNEL_1:
		for(int i =0;i<IIR_TAP_NUM_CROSS;i++)
			iir_taps_CO1[i]=buff[i];
		arm_biquad_cascade_df1_init_f32(&iir_ch1_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(float32_t *) iir_taps_CO1 , &iir_ch2_state[0]);
	break;
	case CHANNEL_2:
		for(int i =0;i<IIR_TAP_NUM_CROSS;i++)
			iir_taps_CO2[i]=buff[i];
		arm_biquad_cascade_df1_init_f32(&iir_ch2_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(float32_t *) iir_taps_CO2 , &iir_ch3_state[0]);
	break;
	case CHANNEL_3:
		for(int i =0;i<IIR_TAP_NUM_CROSS;i++)
			iir_taps_CO3[i]=buff[i];
		arm_biquad_cascade_df1_init_f32(&iir_ch3_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(float32_t *) iir_taps_CO3 , &iir_ch4_state[0]);
	break;

	}

}

//Esta funcion inicializa todos los filtros para evitar HARDFAULTS
void filter_init_all(){
	filter_initialization_co(CHANNEL_0,TYPE_BP1);
	filter_initialization_co(CHANNEL_1,TYPE_BP1);
	filter_initialization_co(CHANNEL_2,TYPE_BP1);
	filter_initialization_co(CHANNEL_3,TYPE_BP1);
	filter_initialization_eq(EQ_PROF_POP);

}

void filter_init_system(){

	static uint8_t first_init = 1;

	if(first_init){
		filter_init_all();
		first_init = 0;
	}


	UserData buff;
	get_user_data(&buff);

	//xSemaphoreTake(semProcessData,portMAX_DELAY);
	if(buff.audio_output[0].state == ON ) filter_initialization_co(CHANNEL_0,buff.audio_output[0].type_equalizer);
	if(buff.audio_output[1].state == ON	) filter_initialization_co(CHANNEL_1,buff.audio_output[1].type_equalizer);
	if(buff.audio_output[2].state == ON ) filter_initialization_co(CHANNEL_2,buff.audio_output[2].type_equalizer);
	if(buff.audio_output[3].state == ON ) filter_initialization_co(CHANNEL_3,buff.audio_output[3].type_equalizer);

	first_init = 0; // Una vez incializados una vez ya no se vuelven a inicializar
	filter_initialization_eq(buff.general_config.equalization_type);
	//xSemaphoreGive(semProcessData);
}

//#############################################################################
// Process de filtrado
//#############################################################################


//uint32_t tim1_cuenta=0;
//extern TIM_HandleTypeDef htim2;

/*
 * // Buffer entrada de audio

 * */

void process_filter(){
//
//	tim1_cuenta = 0 ;
//	__HAL_TIM_SET_COUNTER(&htim2,0);

	UserData buff;
	get_user_data(&buff);

	//1 Realizo el filtrado de ecualizacion (canal derecho)

	// Pido datos si los hay
	readData_I2S(DERECHO, buff_filtrado2, BUFFER_SAMPLE_LEN);

	// Realizo el primer filtrado

	filter_function_eq(buff_filtrado2,buff_filtrado1);
	//2 Realizo el filtrado de cada canal que necesite datos del canal derecho.

	for(int i=0;i < 4;i++){
		// Chequeo si el canal esta encendido
		if(buff.audio_output[i].state == ON){

			uint8_t channel = buff.audio_output[i].channel;
			uint8_t left_right = buff.audio_output[i].channel_audio;

			// Si usa el canal derecho obtengo la salida sino no hago nada

			if(left_right == RIGHT_CHANNEL_OUTPUT){


				filter_function_co(channel,buff_filtrado1,buff_filtrado2);
				writeData_I2S(channel,(float32_t*) buff_filtrado2, BUFFER_SAMPLE_LEN,gain_channels[channel]); // tegno que poner el buff1

			}
		}
	}

//	//3 Realizo el filtrado de ecualizacion (canal Izquierdo)
//
	readData_I2S(IZQUIERDO, buff_filtrado2, BUFFER_SAMPLE_LEN);

	filter_function_eq(buff_filtrado2,buff_filtrado1);

	//4 Realizo el filtrado de cada canal que necesite datos del canal Izquierdo.

	for(int i=0;i < 4;i++){
		// Chequeo si el canal esta encendido
		if(buff.audio_output[i].state == ON){

			uint8_t channel = buff.audio_output[i].channel;
			uint8_t left_right = buff.audio_output[i].channel_audio;

			if(left_right == LEFT_CHANNEL_OUTPUT){

				filter_function_co(channel,buff_filtrado1,buff_filtrado2);
				writeData_I2S(channel,(float32_t*) buff_filtrado2, BUFFER_SAMPLE_LEN,gain_channels[channel]); // va el 1

			}
		}
	}
	// pongo los canales que no se usan en cero
	for(int i=0;i < 4;i++){
		// Chequeo si el canal esta encendido
		if(buff.audio_output[i].state == OFF){
			uint8_t channel = buff.audio_output[i].channel;
			writeData_I2S(channel,(float32_t*)buff_filtrado2, BUFFER_SAMPLE_LEN,0);
		}
	}
	//tim1_cuenta = __HAL_TIM_GET_COUNTER(&htim2);
	//lberarSemaforoProc();
//	pruebaLoopback();
}

// Hacer en una tarea aparte

void process_set_gains(){

	UserData buff;
	float gain_system;
	uint8_t estado_loudness;

	get_user_data(&buff);
	gain_system = (float)(buff.general_config.max_volume * buff.main_volume)/10000.0; // Obtengo el volumen seteado por el usuario (por defecto esta en %)
	estado_loudness = buff.general_config.loudness_state;


	if(estado_loudness == OFF){
		gain_channels[0] = gain_system * (float)(buff.audio_output[0].channel_volume / 100.0);
		gain_channels[1] = gain_system * (float)(buff.audio_output[1].channel_volume / 100.0);
		gain_channels[2] = gain_system * (float)(buff.audio_output[2].channel_volume / 100.0);
		gain_channels[3] = gain_system * (float)(buff.audio_output[3].channel_volume / 100.0);
		send_volume_esp((float)gain_system*100,0); // Apago la correccion de loudness
	}
	else{
		gain_channels[0] = (float)(buff.audio_output[0].channel_volume / 100.0);
		gain_channels[1] = (float)(buff.audio_output[1].channel_volume / 100.0);
		gain_channels[2] = (float)(buff.audio_output[2].channel_volume / 100.0);
		gain_channels[3] = (float)(buff.audio_output[3].channel_volume / 100.0);
		send_volume_esp((float)gain_system*100,1); // Prendo la correccion de loudness
	}
}

void give_sem_save_volume(){
	xSemaphoreGive(semCalcVolume);
}
