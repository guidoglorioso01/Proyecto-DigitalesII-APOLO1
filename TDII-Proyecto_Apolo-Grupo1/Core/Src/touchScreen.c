/*
 * touchScreen.c
 *
 *  Created on: Jul 22, 2023
 *      Author: guido
 */

#include <touchScreen.h>

extern ADC_HandleTypeDef hadc1;

struct TS_value _valueDisplay;
struct TS_value _valueCoords;
uint8_t flag_dato;

float A_C=0.00170853978;
float B_C=-0.558122993;
float C_C=532.70874;
float D_C=-0.39182511;
float E_C=-0.00379674835;
float F_C=360.130005;

uint32_t _vect_y[NUMBER_OF_SAMPLES];
uint32_t _vect_x[NUMBER_OF_SAMPLES];

/*
 * Maquina de estados que actualiza y calcula valores de coordenadas del display
 * Se debe actualizar como minimo cada 1ms
 * */
void mde_TS(){
	static uint8_t estado = STANDBY;
	TS_value coords_TS;
	switch(estado){
	case STANDBY:
		if(check_standby())
			estado = MEDICION_VALORES;
	break;
	case MEDICION_VALORES:
		updateTouchScreen_x();
		updateTouchScreen_y();
		calculate_cords();
		coords_TS.x = CALIB_X();
		coords_TS.y = CALIB_Y();

		switch( is_valid_value(coords_TS) ){

		default:
		case NO_TOUCH:
			estado = STANDBY;
		break;
		case VALIDO:
			flag_dato = 1;
			_valueCoords = coords_TS; // @suppress("No break at end of case")
		case BUG_PIXEL:
		case REBOTE:
			estado = MEDICION_VALORES;
		break;

		}
	break;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Detecto si el display fue precionado
 * RETURN:
 * 1 -> fue precionado
 * 0 -> no fue precionado
 * */

uint8_t check_standby(void){

	uint8_t devolver=0;

	Set_InputPIN(Xn_PORT,Xn_PIN);
	Set_InputPIN_PULLUP(Yp_PORT,Yp_PIN);
	Set_InputPIN(Yn_PORT,Yn_PIN);
	Set_OutputPIN(Xp_PORT,Xp_PIN);
	SET_PIN(Xp_PORT,Xp_PIN,LOW);

	if(HAL_GPIO_ReadPin(Yp_PORT,Yp_PIN) == 0)
		devolver = 1; // hubo presion

	// dejo los pines listos para ser usados por el display
	//Set_OutputPIN(Xp_PORT,Xp_PIN); // Ya lo seteo arriba
	Set_OutputPIN(Xn_PORT,Xn_PIN);
	Set_OutputPIN(Yp_PORT,Yp_PIN);
	Set_OutputPIN(Yn_PORT,Yn_PIN);

	return devolver;
}


/*
 * Funcion que configura pines y lee el ADC
 *
 * X+: GND	(salida)
 * X-: VCC	(salida)
 * Y+: HI_Z (entrada)
 * Y-: ADC	(entrada)
 * */


void updateTouchScreen_x(){

	Set_InputPIN(Yp_PORT,Yp_PIN);
	Set_AnalogPIN(Yn_PORT,Yn_PIN);

	Set_OutputPIN(Xp_PORT,Xp_PIN);
	Set_OutputPIN(Xn_PORT,Xn_PIN);

	SET_PIN(Xn_PORT,Xn_PIN,HIGH);
	SET_PIN(Xp_PORT,Xp_PIN,LOW);

	// configuro ADC en modo polling.

	Set_channel_ADC(ADC_CHANNEL_4); // configuro que canal quiero convetir

	for(uint32_t i= 0;i<NUMBER_OF_SAMPLES;i++){
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10); //timeout = 10ms
		_vect_x[i]= HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);

	// dejo los pines listos para ser usados por el display
	Set_OutputPIN(Yp_PORT,Yp_PIN);
	Set_OutputPIN(Yn_PORT,Yn_PIN);
}

/*
 * Funcion que configura pines y lee el ADC
 *
 * X+: HI_Z	(entrada)
 * X-: ADC	(entrada)
 * Y+: GND 	(salida)
 * Y-: VCC	(salida)
 * */

void updateTouchScreen_y(){

	Set_InputPIN(Xp_PORT,Xp_PIN);
	Set_AnalogPIN(Xn_PORT,Xn_PIN);

	Set_OutputPIN(Yp_PORT,Yp_PIN);
	Set_OutputPIN(Yn_PORT,Yn_PIN);

	SET_PIN(Yn_PORT,Yn_PIN,HIGH);
	SET_PIN(Yp_PORT,Yp_PIN,LOW);

	Set_channel_ADC(ADC_CHANNEL_1); // configuro que canal quiero convetir
	for(uint32_t i= 0;i<NUMBER_OF_SAMPLES;i++){
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10); //timeout = 10ms
		_vect_y[i]= HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);

	// dejo los pines listos para ser usados por el display
	Set_OutputPIN(Xp_PORT,Xp_PIN);
	Set_OutputPIN(Xn_PORT,Xn_PIN);
}


/*
 * Funcion para calibrar display (nota: Bloqueante)
 * */
void Calibrate_display(){
	float xt1,xt2,xt3,yt1,yt2,yt3;
	float xd1,xd2,xd3,yd1,yd2,yd3;
	float A,B,C,D,E,F;
	struct TS_value buff;

	//calibro punto 1 (y = 50% * ymax ; x=10% * xmax)
	xd1= 0.9 * MAX_X;
	yd1= 0.5 * MAX_Y;

	while(check_standby());// Espero a que se deje de tocar la pantalla

	while(!check_standby()); // Espero hhasta que presionen el punto 1

	buff = promediar_coordenadas(20);
	xt1= buff.x;
	yt1= buff.y;

	//calibro punto 2 (y = 10% * ymax ; x=50% * xmax)
	xd2= 0.5 * MAX_X;
	yd2= 0.9 * MAX_Y;

	while(check_standby());// Espero a que se deje de tocar la pantalla

	while(!check_standby()); // Espero hhasta que presionen el punto 1

	buff = promediar_coordenadas(20);
	xt2= buff.x;
	yt2= buff.y;

	//calibro punto 3 (y = 10% * ymax ; x=10% * xmax)
	xd3= 0.1 * MAX_X;
	yd3= 0.1 * MAX_Y;

	while(check_standby());// Espero a que se deje de tocar la pantalla

	while(!check_standby()); // Espero hhasta que presionen el punto 1

	buff = promediar_coordenadas(20);
	xt3= buff.x;
	yt3= buff.y;

	A=(float) (xd1*(yt2-yt3)+xd2*(yt3-yt1)+xd3*(yt1-yt2))/(xt1*(yt2-yt3)+xt2*(yt3-yt1)+ xt3*(yt1-yt2));
	B=(float) (A*(xt3-xt2)+xd2-xd3)/(yt2-yt3);
	C=(float) xd3 - A*xt3 - B*yt3;
	D=(float) (yd1*(yt2-yt3)+yd2*(yt3-yt1)+yd3*(yt1-yt2))/(xt1*(yt2-yt3)+xt2*(yt3-yt1)+xt3*(yt1-yt2));
	E=(float) (D*(xt3-xt2)+yd2-yd3)/(yt2-yt3);
	F=(float) yd3 - D*xt3 - E*yt3;

	A_C = A;
	B_C = B;
	C_C = C;
	D_C = D;
	E_C = E;
	F_C = F;
}


/*
 * Funcion que obtiene un numero de muestras N y las promedia.
 *
 * */
struct TS_value promediar_coordenadas(uint32_t muestras){
	struct TS_value devolver;
	uint32_t x_promedio = 0;
	uint32_t y_promedio = 0;

	for(int i = 0;i<muestras;i++){
		HAL_Delay(10);
		updateTouchScreen_x();
		updateTouchScreen_y();
		calculate_cords();
		x_promedio += _valueDisplay.x;
		y_promedio += _valueDisplay.y;
	}
	devolver.x = (uint16_t) (x_promedio / muestras);
	devolver.y = (uint16_t) (y_promedio / muestras);
	return devolver;
}


/*
 * Funcion que promedia los valores levantados por el ADC tanto en X como en Y
 * */
void calculate_cords(){
	uint32_t i = 0;
	uint32_t promedio=0;

	//Algortimo que calcula el valor de X en base a las muestras
	for(i=0;i<NUMBER_OF_SAMPLES;i++){
		promedio += _vect_x[i];
	}
	_valueDisplay.x = promedio/NUMBER_OF_SAMPLES;

	//Algortimo que calcula el valor de X en base a las muestras
	for(i=0,promedio=0;i<NUMBER_OF_SAMPLES;i++){
		promedio += _vect_y[i];
	}
	_valueDisplay.y = promedio/NUMBER_OF_SAMPLES;
}


/*Funcion que verifica que tipo de valor se obtuvo
 *Devuelve cero si el valor medido es valido
 *Devuelve codigo de Error en caso de no ser valido
 *
 * */
uint8_t is_valid_value(struct TS_value pantalla){

	static struct TS_value previo;
	if(pantalla.x < 5 || pantalla.y<5)
		return NO_TOUCH;
	if(pantalla.x > 223 && pantalla.x < 228 && pantalla.y > 95 && pantalla.y < 97 )
		return BUG_PIXEL;
	if(pantalla.x > (previo.x + OFFSET_REBOTE) || pantalla.x < (previo.x - OFFSET_REBOTE) || pantalla.y > (previo.y + OFFSET_REBOTE) || pantalla.y < (previo.y - OFFSET_REBOTE)){
		previo = pantalla;
		return REBOTE;
	}
	previo = pantalla;
	return VALIDO;
}

uint8_t get_TS_cords(TS_value* value_str){

	if(flag_dato){
		flag_dato=0;
		*value_str = _valueCoords;
		return 1;
	}
	return 0;

}


//////////////////////////////////Funciones de Hardware///////////////////////////////////////////////////////
/*
 * Channel es un define ->ejemplo para configurar el channel 1: ADC_CHANNEL_1
 *
 * */

void Set_channel_ADC(uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig1 = {0};

  sConfig1.Channel = channel;// ADC_CHANNEL_4;
  sConfig1.Rank = 1;
  sConfig1.SamplingTime = ADC_SAMPLETIME_3CYCLES; //3 le da un delay al ADC para tener lecturas mas estables
  if(HAL_ADC_ConfigChannel(&hadc1, &sConfig1)!=HAL_OK){
	  while(1){

	  }
  }

}

/*
 * PIN : es un define (GPIO pins define) -> ejemplo pin 13: GPIO_PIN_13
 * PORT: es un define -> ejemplo puerto A :GPIOA
 * */

void Set_OutputPIN(GPIO_TypeDef * PORT,uint32_t PIN){
	GPIO_InitTypeDef pin_init;
	pin_init.Pin = PIN;
	pin_init.Mode = GPIO_MODE_OUTPUT_PP;
	pin_init.Pull = GPIO_NOPULL;
	pin_init.Speed = GPIO_SPEED_MEDIUM;
	//pin_init.Alternate = ? //no entendi que es
	HAL_GPIO_Init(PORT,&pin_init);
}
void Set_InputPIN(GPIO_TypeDef * PORT,uint32_t PIN){
	GPIO_InitTypeDef pin_init;
	pin_init.Pin = PIN;
	pin_init.Mode = GPIO_MODE_INPUT;
	pin_init.Pull = GPIO_NOPULL;
	pin_init.Speed = GPIO_SPEED_MEDIUM;
	//pin_init.Alternate = ? //no entendi que es
	HAL_GPIO_Init(PORT,&pin_init);
}
void Set_InputPIN_PULLUP(GPIO_TypeDef * PORT,uint32_t PIN){
	GPIO_InitTypeDef pin_init;
	pin_init.Pin = PIN;
	pin_init.Mode = GPIO_MODE_INPUT;
	pin_init.Pull = GPIO_PULLUP;
	pin_init.Speed = GPIO_SPEED_MEDIUM;
	//pin_init.Alternate = ? //no entendi que es
	HAL_GPIO_Init(PORT,&pin_init);
}

void Set_AnalogPIN(GPIO_TypeDef * PORT,uint32_t PIN){
	GPIO_InitTypeDef pin_init;
	pin_init.Pin = PIN;
	pin_init.Mode = GPIO_MODE_ANALOG;
	pin_init.Pull = GPIO_NOPULL;
	pin_init.Speed = GPIO_SPEED_MEDIUM;
	//pin_init.Alternate = ? //no entendi que es
	HAL_GPIO_Init(PORT,&pin_init);
}


