/*
 * user_info.c
 *
 *  Created on: 18 sep. 2023
 *      Author: guido
 */
#include <Control_DatosUsuario.h>


UserData user_data;


void init_userdata(){

	//Flash_Read_Data(&user_data);

	if(user_data.Isinitailized != INITIALIZED){

		user_data.main_volume = 50;

		//inicializo ecualizador general

		int8_t ecualizacion[8] ;
		FLAT_PROFILE(ecualizacion);
		config_eq(ecualizacion, &user_data.general_equalizer);

		//inicializo entrada de audio

		user_data.audio_input.type_in = AUX;
		user_data.audio_input.channel = STEREO_CHANNEL_INPUT;
		user_data.audio_input.channel_balance = 0; // va de -50 a 50

		//configuracion general

		user_data.general_config.max_volume = 75; // protegemos al usuario de que se rompa los timpanos
		user_data.general_config.equalization_type = EQ_PROF_PLANO ;
		user_data.general_config.system_type = SYSTEM_2_0;
		user_data.general_config.loudness_state = OFF;

		//configuro canales de salida en formato 2.0

		user_data.audio_output[CHANNEL_0].channel = 0;
		user_data.audio_output[CHANNEL_0].state = ON;
		user_data.audio_output[CHANNEL_0].channel_audio = RIGHT_CHANNEL_OUTPUT;
		user_data.audio_output[CHANNEL_0].channel_volume = MAX_VOLUME;
		user_data.audio_output[CHANNEL_0].type_equalizer = TYPE_FLAT;

		user_data.audio_output[CHANNEL_1].channel = 1;
		user_data.audio_output[CHANNEL_1].state = ON;
		user_data.audio_output[CHANNEL_1].channel_audio = LEFT_CHANNEL_OUTPUT;
		user_data.audio_output[CHANNEL_1].channel_volume = MAX_VOLUME;
		user_data.audio_output[CHANNEL_1].type_equalizer = TYPE_FLAT;

		user_data.audio_output[CHANNEL_2].channel = 2;
		user_data.audio_output[CHANNEL_2].state = OFF;
		user_data.audio_output[CHANNEL_2].channel_audio = RIGHT_CHANNEL_OUTPUT;
		user_data.audio_output[CHANNEL_2].channel_volume = MAX_VOLUME;
		user_data.audio_output[CHANNEL_2].type_equalizer = TYPE_FLAT;

		user_data.audio_output[CHANNEL_3].channel = 3;
		user_data.audio_output[CHANNEL_3].state = OFF;
		user_data.audio_output[CHANNEL_3].channel_audio = LEFT_CHANNEL_OUTPUT;
		user_data.audio_output[CHANNEL_3].channel_volume = MAX_VOLUME;
		user_data.audio_output[CHANNEL_3].type_equalizer = TYPE_FLAT;

		user_data.audio_output_pers[0] = user_data.audio_output[0];
		user_data.audio_output_pers[1] = user_data.audio_output[1];
		user_data.audio_output_pers[2] = user_data.audio_output[2];
		user_data.audio_output_pers[3] = user_data.audio_output[3];

		user_data.channelInUse = CHANNEL_0;
		//save_user_data();

		user_data.Isinitailized = INITIALIZED;
	}

}


void config_eq(int8_t *values,Equalizer *position){
	position->_20hz		= values[0]; //dB
	position->_200hz 	= values[1]; //dB
	position->_500hz 	= values[2]; //dB
	position->_1000hz	= values[3]; //dB
	position->_4000hz 	= values[4]; //dB
	position->_8000hz 	= values[5]; //dB
	position->_16000hz 	= values[6]; //dB
}

void reset_userdata(){
	init_userdata();
}

//void Get_CrossOver_coef(uint8_t type_crossover,uint8_t *buff_coef){
//	/*
//	 * Funcion que recibe como parametro un vector que debe tener almenos 7 posiciones
//	 * y completo en el los coeficientes del cross over pedido
//	 *
//	 * */
//
//	switch(type_crossover){
//	case TYPE_LP1:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	case TYPE_LP2:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	case TYPE_BP1:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	case TYPE_BP2:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	case TYPE_HP1:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	case TYPE_HP2:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	default:
//	case TYPE_FLAT:
//		CARGAR_CROSSOVER(buff_coef,0,0,0,0,0,0,0);
//	break;
//	}
//
//}


/*
 * Rate: separacion entre muestras
 * puntos: puntos de referencia
 * n: cant de puntos a dados
 * m: cant de puntos a calcular
 * x: vector donde devuelvo lo calculado (double)
 */
void calcularPuntos(Punto_t puntos[],uint8_t n, uint8_t m, Punto_t *res){


	double x_1,x_2,y_1,y_2 ;

	float max_freq= 4.301; // log(20kHz)
	float min_freq= 1; // log(1Hz)

	//int order_pol = 4;
	//double coef[4];
	Punto_t puntos_log[n] ;
	Punto_t ress[m];
	double x = 0;
	int i_vector = 0 ;

	// Hago puntos del eje x logaritmico

	for(int i =0;i<n;i++){
		if(puntos[i].x!=0)
			puntos_log[i].x = log10(puntos[i].x);
		else
			puntos_log[i].x = 0;

		puntos_log[i].y = puntos[i].y;
	}

	// completo el eje de frecuencia a plotear

	float x_rate = (max_freq - min_freq) / m;

	for(int i = 0;i<m;i++)
		ress[i].x = min_freq + i*x_rate;

	// hago la interpolacion

	for(int i=0;i<n-1;i++){

		 x_1 = puntos_log[i].x;
		 x_2 = puntos_log[i + 1].x;
		 y_1 = puntos_log[i].y;
		 y_2 = puntos_log[i + 1].y;

		// get_pol_coef(x_1, x_2, y_1, y_2, order_pol, coef);

		 while(ress[i_vector].x < x_2){

			x = ress[i_vector].x;

			/*for (int i3 = 0; i3 <= order_pol; i3++) {
				result += coef[i3] * pow(x, order_pol - i3);
			}*/
			float m_f = (y_2 - y_1) / (x_2 - x_1);
			float b_f = y_1 - m_f * x_1;

			ress[i_vector].y =  m_f * x + b_f;;

			i_vector++;
		 }
	}


	for(int i = i_vector - 1; i < m;i++){
		ress[i].y = ress[i_vector-1].y;
	}
	for(int i= 0;i<m;i++){
		res[i].y = ress[i].y;
		res[i].x = ress[i].x;
	}
}



//void get_pol_coef(double x1, double x2, double y1, double y2,int order, double *coef){
//
//    Parameters:
//    x_points : vector con "n" puntos evaluados en X
//    y_points : vector con "n" puntos evaluados en y
//    n: Numero de puntos pasados
//    coef: Vector donde se guardan los coeficientes en orden A + Bx + Cx^2 ......
//	*/
//
//	// Calcular los coeficientes del polinomio utilizando las fórmulas de interpolación
//	 // Calcular los coeficientes del polinomio usando los puntos y el orden dado
//	  /*  for (int i = 0; i <= order; i++) {
//	        coef[i] = 0.0;
//	    }
//
//	    double x_diff = x2 - x1;
//	    double x_term = 1.0;
//
//	    for (int i = 0; i <= order; i++) {
//	        for (int j = 0; j <= i; j++) {
//	            coef[i] += (y1 * x_term) / (i - j + 1);
//	            x_term *= x_diff;
//	        }
//	        x_term = 1.0;
//	    }
//
//}



void set_user_data(UserData new_data){
	user_data = new_data;
}
void get_user_data(UserData *read_data){
	*read_data = user_data;
}










