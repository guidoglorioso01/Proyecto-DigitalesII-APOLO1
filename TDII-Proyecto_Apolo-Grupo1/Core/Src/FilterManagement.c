/*
 * FilterCoefs.c
 *
 *  Created on: 22 oct. 2023
 *      Author: guido
 */
#include <FilterManagement.h>



//#############################################################################
// Variables Globales
//#############################################################################

q15_t Coef_IIR_EQ[IIR_TAP_NUM_EQ];
q15_t iir_taps_CO0	[IIR_TAP_NUM_CROSS] ;
q15_t iir_taps_CO1	[IIR_TAP_NUM_CROSS] ;
q15_t iir_taps_CO2	[IIR_TAP_NUM_CROSS] ;
q15_t iir_taps_CO3	[IIR_TAP_NUM_CROSS] ;
float32_t gain_channels[4];

q15_t iir_eq_state[4*NUMBER_SOS_EQ];
q15_t iir_ch1_state[4],  iir_ch2_state[4], iir_ch3_state[4],  iir_ch4_state[4];

arm_biquad_casd_df1_inst_q15 iir_eq_settings;
arm_biquad_casd_df1_inst_q15 iir_ch1_settings,  iir_ch2_settings, iir_ch3_settings,  iir_ch0_settings;

q15_t buff_filtrado1[BUFFER_SAMPLE_LEN];
q15_t buff_filtrado2[BUFFER_SAMPLE_LEN];
//#############################################################################
//Double Buffering
//#############################################################################

//Buffers de entrada
uint16_t bufferInA[BUFF_SIZE];
uint16_t bufferInB[BUFF_SIZE];
uint16_t *ptrDmaIn = bufferInA;
uint16_t *ptrProcessIn = bufferInB;

//Buffers de Salida AB
uint16_t bufferOut1_AB[BUFF_SIZE];
uint16_t bufferOut2_AB[BUFF_SIZE];
uint16_t *ptrProcessOutAB = bufferOut1_AB;
uint16_t *ptrDmaOutAB = bufferOut2_AB;

//Buffers Salida CD
uint16_t bufferOut1_CD[BUFF_SIZE];
uint16_t bufferOut2_CD[BUFF_SIZE];
uint16_t *ptrProcessOutCD = bufferOut1_CD;
uint16_t *ptrDmaOutCD = bufferOut2_CD;


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

void filter_function_eq(q15_t *buff_in,q15_t *buff_out){

	/*
	 * NOTAS para ver:
	 * debo verificar que si el filtro tiene ganancia que no me haga overflow
	 *
	 * */
	UserData buff;
	get_user_data(&buff);

	if(buff.general_config.equalization_type == EQ_PROF_PLANO){
		memcpy(buff_out,buff_in,BLOCK_SIZE_FLOAT);
		return;
	}

	arm_biquad_cascade_df1_q15(&iir_eq_settings, buff_in, buff_out, BUFFER_SAMPLE_LEN);

}


/*
 * Funcion que realiza el filtrado de ecualizacion
 *
 * parametros:
 * Channel -> CHANNEl_0 .... CHANNEL_3
 * buff_in-> datos de entrada, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * buff_out-> datos de salida, por defecto debe ser de tamaño BLOCK_SIZE_FLOAT
 * */

void filter_function_co(uint8_t channel, q15_t *buff_in,q15_t *buff_out){

	/*
	 * Tengo que convertir un dato uint16 a q15?
	 * */
	UserData buff;
	get_user_data(&buff);
	// En caso de el filtro configurado se Respuesta plana, copio entrada en salida
	uint8_t filter_type = buff.audio_output[channel].type_equalizer;
	if(filter_type == TYPE_FLAT){
		memcpy(buff_out,buff_in,BLOCK_SIZE_FLOAT);
		return;
	}


	switch(channel){
	case CHANNEL_0:
		arm_biquad_cascade_df1_q15(&iir_ch0_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_1:
		arm_biquad_cascade_df1_q15(&iir_ch1_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_2:
		arm_biquad_cascade_df1_q15(&iir_ch2_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
		break;
	case CHANNEL_3:
		arm_biquad_cascade_df1_q15(&iir_ch3_settings, buff_in, buff_out, BLOCK_SIZE_FLOAT);
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

	designFilter(gain,q_bw,sos_float,Q15);

	float32_t max;
	uint32_t index;
	arm_abs_f32(sos_float,sos_float_check_max,IIR_TAP_NUM_EQ); // pongo todos los coef en modulo
	arm_max_f32(sos_float_check_max,IIR_TAP_NUM_EQ,&max,&index); 		// verifico que ningun sea mayor a 1

	if(max > 1){
		// Si algun coef es mayor a 1 divido el vector para que todos sean menores a 1
		// si max > 1 y max < 2 se divide por 2
		// si max > 2 y max < 3 se divide por 4. Ningun coeficiente es mayor a 3.
		arm_scale_f32(sos_float,(float32_t)1/(1 << (uint32_t)max ) ,sos_float,IIR_TAP_NUM_EQ );
	}

	arm_float_to_q15(sos_float, Coef_IIR_EQ ,IIR_TAP_NUM_EQ);

	arm_biquad_cascade_df1_init_q15(&iir_eq_settings,(uint32_t)(IIR_TAP_NUM_EQ / IIR_CANT_COEF),(q15_t *)Coef_IIR_EQ , iir_eq_state, (int8_t)max);


}

/*
 * Funcion que inicializa Filtros de cross over
 *
 * parametros:
 * Channel -> CHANNEl_0 .... CHANNEL_3
 * taps-> Puntero a los taps del filtro definidos en este archivo
 * */
void filter_initialization_co(uint8_t channel,uint8_t Type){

	/*
	 * Tengo que cambiar esta funcion para que directamente los calcule los coeficientes y no tenga que guardarlos en float
	 * */

	float32_t buff[IIR_TAP_NUM_CROSS];
	float32_t buff_max[IIR_TAP_NUM_CROSS];

	designCrossover(buff, Type);

	float32_t max;

	arm_abs_f32(buff,buff_max,IIR_TAP_NUM_CROSS); // pongo todos los coef en modulo

	uint32_t index;
	arm_max_f32(buff_max,IIR_TAP_NUM_CROSS,&max,&index); 		// verifico que ningun sea mayor a 1

	if(max > 1){
		// Si algun coef es mayor a 1 divido el vector para que todos sean menores a 1
		// si max > 1 y max < 2 se divide por 2
		// si max > 2 y max < 3 se divide por 4. Ningun coeficiente es mayor a 3.
		arm_scale_f32(buff,(float32_t)1/(1 << (uint32_t)max ) ,buff,IIR_TAP_NUM_CROSS );
	}

	switch(channel){
	case CHANNEL_0:
		arm_float_to_q15(buff, iir_taps_CO0 ,IIR_TAP_NUM_CROSS);
		arm_biquad_cascade_df1_init_q15(&iir_ch0_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(q15_t *) iir_taps_CO0, &iir_ch1_state[0],(int8_t)max);
	break;
	case CHANNEL_1:
		arm_float_to_q15(buff, iir_taps_CO1 ,IIR_TAP_NUM_CROSS);
		arm_biquad_cascade_df1_init_q15(&iir_ch1_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(q15_t *) iir_taps_CO1 , &iir_ch2_state[0],(int8_t)max);
	break;
	case CHANNEL_2:
		arm_float_to_q15(buff, iir_taps_CO2 ,IIR_TAP_NUM_CROSS);
		arm_biquad_cascade_df1_init_q15(&iir_ch2_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(q15_t *) iir_taps_CO2 , &iir_ch3_state[0],(int8_t)max);
	break;
	case CHANNEL_3:
		arm_float_to_q15(buff, iir_taps_CO3 ,IIR_TAP_NUM_CROSS);
		arm_biquad_cascade_df1_init_q15(&iir_ch3_settings,(uint32_t)(IIR_TAP_NUM_CROSS / IIR_CANT_COEF),(q15_t *) iir_taps_CO3 , &iir_ch4_state[0],(int8_t)max);
	break;

	}

}

void filter_init_system(){

	UserData buff;
	get_user_data(&buff);

	//xSemaphoreTake(semProcessData,portMAX_DELAY);
	if(buff.audio_output[0].state == ON) filter_initialization_co(CHANNEL_0,buff.audio_output[0].type_equalizer);
	if(buff.audio_output[1].state == ON) filter_initialization_co(CHANNEL_1,buff.audio_output[1].type_equalizer);
	if(buff.audio_output[2].state == ON) filter_initialization_co(CHANNEL_2,buff.audio_output[2].type_equalizer);
	if(buff.audio_output[3].state == ON) filter_initialization_co(CHANNEL_3,buff.audio_output[3].type_equalizer);

	filter_initialization_eq(buff.general_config.equalization_type);
	//xSemaphoreGive(semProcessData);
}

//#############################################################################
// Process de filtrado
//#############################################################################


uint32_t tim1_cuenta=0;
extern TIM_HandleTypeDef htim2;



void process_filter(){

	// Buffer entrada de audio
	tim1_cuenta = 0 ;
	__HAL_TIM_SET_COUNTER(&htim2,0);
	UserData buff;
	get_user_data(&buff);

	//1 Realizo el filtrado de ecualizacion (canal derecho)

//	for(int i=0;BUFFER_SAMPLE_LEN;i++){ //  Agarro del buff DMA los datos que necesito Canal 0 (Creo que es el derecho)
//		buff_filtrado1[i] = (q15_t) readData_DMA(DERECHO,i);
//	}
	filter_function_eq(buff_filtrado1,buff_filtrado2);

	//2 Realizo el filtrado de cada canal que necesite datos del canal derecho.

	for(int i=0;i < 4;i++){
		// Chequeo si el canal esta encendido
		if(buff.audio_output[i].state == ON){

			uint8_t channel = buff.audio_output[i].channel;
			uint8_t left_right = buff.audio_output[i].channel_audio;

			// Si usa el canal derecho obtengo la salida sino no hago nada

			if(left_right == RIGHT_CHANNEL_OUTPUT){

				filter_function_co(channel,buff_filtrado2,buff_filtrado1);

				for(int i2=0;i2<BUFFER_SAMPLE_LEN;i2++){
					// Escribo en el buffer que corresponda y aplico la ganancia del canal
					writeData_DMA(channel, i2, (uint16_t)(buff_filtrado2[i2]* (float)gain_channels[channel]));
				}
			}
		}
	}

	//3 Realizo el filtrado de ecualizacion (canal Izquierdo)

//	for(int i=0;BUFFER_SAMPLE_LEN;i++) //  Agarro del buff DMA los datos que necesito Canal 0 (Creo que es el derecho)
//		buff_filtrado1[i] = readData_DMA(IZQUIERDO,i);

	// Solo para debug  SACAR!!!!!!!!!!!
	for(int i=0;i<BUFFER_SAMPLE_LEN;i++){
		buff_filtrado1[i] =0;
	}

	buff_filtrado1[1] = 0x3FFF; //hago un delta
	// fin del debug

	filter_function_eq(buff_filtrado1,buff_filtrado2);

	//4 Realizo el filtrado de cada canal que necesite datos del canal Izquierdo.

	for(int i=0;i < 4;i++){
		// Chequeo si el canal esta encendido
		if(buff.audio_output[i].state == ON){

			uint8_t channel = buff.audio_output[i].channel;
			uint8_t left_right = buff.audio_output[i].channel_audio;

			// Si usa el canal izquierdo obtengo la salida sino no hago nada

				// Solo para debug  SACAR!!!!!!!!!!!
				for(int i=0;i<BUFFER_SAMPLE_LEN;i++){
					buff_filtrado2[i] =0;
				}

				buff_filtrado2[0] = 0x3FFF; //hago un delta
				// fin del debug

			if(left_right == LEFT_CHANNEL_OUTPUT){
				filter_function_co(channel,buff_filtrado2,buff_filtrado1);

				for(int i2=0;i2<BUFFER_SAMPLE_LEN;i2++){
					// Escribo en el buffer que corresponda y aplico la ganancia del canal
					writeData_DMA(channel, i2, (uint16_t)(buff_filtrado2[i2]* (float)gain_channels[channel]));
				}
			}
		}
	}
	tim1_cuenta = __HAL_TIM_GET_COUNTER(&htim2);
}


void process_set_gains(){

	UserData buff;
	get_user_data(&buff);
	float setpoint,gain_system;

	setpoint = (float)(buff.general_config.max_volume * buff.main_volume)/10000.0; // Obtengo el volumen seteado por el usuario (por defecto esta en %)

	if(buff.general_config.loudness_state == OFF){
		gain_system = setpoint;
	}
	else{
		gain_system = setpoint;
		//gain_system = loudness_correction();
	}

	// Calculo la ganancia de cada canal

	gain_channels[0] = gain_system * (float)(buff.audio_output[0].channel_volume / 100.0);
	gain_channels[1] = gain_system * (float)(buff.audio_output[1].channel_volume / 100.0);
	gain_channels[2] = gain_system * (float)(buff.audio_output[2].channel_volume / 100.0);
	gain_channels[3] = gain_system * (float)(buff.audio_output[3].channel_volume / 100.0);

}

//#############################################################################
// Funciones para leer/escribir en buffer DMA
//#############################################################################

uint16_t readData_DMA(canal_t canal, uint32_t nro_dato) {
	//uint16_t devolver = ;
	return ptrProcessIn[2*nro_dato+canal];
}


void writeData_DMA(uint8_t ampli, uint32_t nro_dato, uint16_t dato) {
	if(ampli==CHANNEL_0)
		ptrProcessOutAB[nro_dato*2+IZQUIERDO] = dato;
	if(ampli==CHANNEL_1)
		ptrProcessOutAB[nro_dato*2+DERECHO] = dato;
	if(ampli==CHANNEL_2)
		ptrProcessOutCD[nro_dato*2+IZQUIERDO] = dato;
	if(ampli==CHANNEL_3)
		ptrProcessOutCD[nro_dato*2+DERECHO] = dato;
}



/// Hay que convertirlos a q15

//const float32_t iir_taps_CINE	[IIR_TAP_NUM_EQ] = {1.0035378522117637,-1.98280682256079,0.9795532716154106,1.9828777291113742,-0.9830202172765903,
//											1.005307517480333,-1.9734674498154148,0.9689614064054366,1.9734674498154148,-0.9742689238857696,
//											0.9871489298278249,-1.9071458251197513,0.9248463814189198,1.9071458251197513,-0.9119953112467447,
//											0.9709377937722502,-1.7747224840497484,0.83004295107702,1.7747224840497484,-0.8009807448492702,
//											1.0490228845094334,-1.483803319833424,0.7133120343437699,1.483803319833424,-0.7623349188532034,
//											1.275738956648572,-0.6040523436846152,0.1701579641904454,0.6040523436846152,-0.4458969208390174,
//											1.3394827017747446,0.16414835499397923,0.12596313593495098,-0.4887002536076259,-0.1408939390960488};
//const float32_t iir_taps_ROCK	[IIR_TAP_NUM_EQ] = {1.0,-1.9796510852239366,0.9798520305684094,1.9796510852239366,-0.9798520305684094,
//											1.005307517480333,-1.9734674498154148,0.9689614064054366,1.9734674498154148,-0.9742689238857696,
//											1.0265588842505295,-1.9416920481180697,0.9200704940528975,1.9416920481180697,-0.9466293783034272,
//											1.0,-1.8031815766483155,0.8298609096336507,1.8031815766483155,-0.8298609096336507,
//											0.9532680504559645,-1.414462297957696,0.7267095218896991,1.414462297957696,-0.6799775723456635,
//											1.1290968068756282,-0.5740715530819134,0.24503624569935156,0.5740715530819134,-0.37413305257497975,
//											1.0,0.3084498998339442,0.11076634870281762,-0.3084498998339442,-0.11076634870281762};
//const float32_t iir_taps_POP		[IIR_TAP_NUM_EQ] = {1.0,-1.9796510852239366,0.9798520305684094,1.9796510852239366,-0.9798520305684094,
//											1.005307517480333,-1.9734674498154148,0.9689614064054366,1.9734674498154148,-0.9742689238857696,
//											1.0265588842505295,-1.9416920481180697,0.9200704940528975,1.9416920481180697,-0.9466293783034272,
//											1.0299320990635645,-1.827843653252661,0.8249557798521062,1.827843653252661,-0.8548878789156708,
//											0.9532680504559645,-1.414462297957696,0.7267095218896991,1.414462297957696,-0.6799775723456635,
//											0.8856636507255232,-0.5084343074801985,0.33135604520064066,0.5084343074801985,-0.217019695926164,
//											1.0,0.3084498998339442,0.11076634870281762,-0.3084498998339442,-0.11076634870281762};
//const float32_t iir_taps_ELECTRO	[IIR_TAP_NUM_EQ] = {1.001760796957629,-1.981299069651173,0.9797772977817074,1.981333973793491,-0.9815031905970182,
//											1.0107957448848515,-1.9775026167584935,0.9675099847253587,1.9775026167584935,-0.9783057296102101,
//											1.0130183701606368,-1.9319737554214733,0.92386800379332,1.9319737554214733,-0.9368863739539568,
//											0.9709377937722502,-1.7747224840497484,0.83004295107702,1.7747224840497484,-0.8009807448492702,
//											1.1014229857242093,-1.5123063909289807,0.6947654478198052,1.5123063909289807,-0.7961884335440143,
//											1.275738956648572,-0.6040523436846152,0.1701579641904454,0.6040523436846152,-0.4458969208390174,
//											1.157289495548599,0.24997746864619855,0.11646729534355592,-0.39947700210733944,-0.12425725743101422};
//const float32_t iir_taps_VOCAL	[IIR_TAP_NUM_EQ] = {0.9982422979987072,-1.977851379102525,0.9797780004746307,1.9778165363112876,-0.9780551412645754,
//											0.9947205035394189,-1.9630485353990421,0.9691252745504605,1.9630485353990421,-0.9638457780898795,
//											1.0130183701606368,-1.9319737554214733,0.92386800379332,1.9319737554214733,-0.9368863739539568,
//											1.0614664290522369,-1.8491228854981583,0.8150155232375795,1.8491228854981583,-0.8764819522898164,
//											1.0490228845094334,-1.483803319833424,0.7133120343437699,1.483803319833424,-0.7623349188532034,
//											0.8856636507255232,-0.5084343074801985,0.33135604520064066,0.5084343074801985,-0.217019695926164,
//											0.8640880296990532,0.3451832956610154,0.10736920874937309,-0.21600253835165056,-0.10063799575779099};


//// Esto hay que sacarlo e implementarlo con funciones que los calcule en el momento
//const float32_t iir_taps_LP1	[IIR_TAP_NUM_CROSS] = {0.00019735700341214275,0,0.0003947140068242855,0.00019735700341214275,1.943806476755819,-0.9445959047694675};
//const float32_t iir_taps_LP2	[IIR_TAP_NUM_CROSS] = {0.0011839064420396121,0,0.0023678128840792243,0.0011839064420396121,1.862368233780738,-0.8671038595488963};
//const float32_t iir_taps_BP1	[IIR_TAP_NUM_CROSS] = {0.5458369509680533,0,0.0,-0.5458369509680533,0.8211437833546443,0.09169216101118098};
//const float32_t iir_taps_BP2	[IIR_TAP_NUM_CROSS] = {0.4900251652308176,0,0.0,-0.4900251652308176,0.8814272214519776,-0.02416128361386568};
//const float32_t iir_taps_HP1	[IIR_TAP_NUM_CROSS] = {0.3714253996511387,0,-0.7428507993022774,0.3714253996511387,0.43778719219258744,-0.04791440641196737};
//const float32_t iir_taps_HP2	[IIR_TAP_NUM_CROSS] = {0.28791215498173994,0,-0.5758243099634799,0.28791215498173994,0.14629785437805412,-0.005350765548905597};
//
