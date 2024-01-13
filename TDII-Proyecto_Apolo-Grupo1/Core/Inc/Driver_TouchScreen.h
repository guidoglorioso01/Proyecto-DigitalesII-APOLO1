/*
 * touchScreen.h
 *
 *  Created on: Jul 22, 2023
 *      Author: guido
 */


#ifndef INC_DRIVER_TOUCHSCREEN_H_
#define INC_DRIVER_TOUCHSCREEN_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

/*
 * A1(X-) -> PA1 : valor en X
 * A2(Y-) -> PA4 : valor en Y
 * D6(Y+) -> PB10 : valor en Y
 * D7(X+) -> PA8  : valor en X
 * */

#define Xn_PORT GPIOA		//A1_PORT
#define Xn_PIN GPIO_PIN_1   //A1_PIN
#define Yn_PORT GPIOA 		//A2_PORT
#define Yn_PIN  GPIO_PIN_4 	//A2_PIN

#define Yp_PORT GPIOB 		//D6_PORT
#define Yp_PIN GPIO_PIN_10 	// D6_PIN
#define Xp_PORT GPIOA 		// D7_PORT
#define Xp_PIN GPIO_PIN_8 	//D7_PIN

#define HIGH 1
#define LOW 0

#define MAX_X 480
#define MAX_Y 320

#define NUMBER_OF_SAMPLES 15
#define OFFSET_REBOTE 5

// define MDS
#define STANDBY 0
#define MEDICION_VALORES 1

//errores
#define NO_TOUCH 1
#define BUG_PIXEL 2
#define REBOTE 3
#define VALIDO 0

// MACROS

#define CALIB_X() ( A_C*(_valueDisplay.x)+B_C*(_valueDisplay.y)+C_C )
#define CALIB_Y() ( D_C*(_valueDisplay.x)+E_C*(_valueDisplay.y)+F_C )

#define SET_PIN(PORT,pin,estado) (HAL_GPIO_WritePin(PORT, pin,estado)) // Estado = 0 -> pin RESET ;Estado = 0 -> pin SET
/*
#define PIN_LOW(port, pin)    (port)->BSRR = (1<<((pin)+16)) // Setea en forma atomica el registro de salida en 0
#define PIN_HIGH(port, pin)   (port)->BSRR = (1<<(pin))		// Idem pero en 1

#define PIN_READ(port, pin)   (port)->IDR & (1<<(pin)) // devuelve 0 en en caso de haber un cero a la salida o !=0 si hay un 1

#define PIN_MODE2(reg, pin, mode) reg=(reg&~(0x3<<((pin)<<1)))|(mode<<((pin)<<1)) // Setea el registro del puerto correspondiente al modo del pin
#define PIN_OUTPUT(port, pin) 	PIN_MODE2((port)->MODER, pin, 0x1)				// en 1 de pone como salida
#define PIN_INPUT(port, pin)	PIN_MODE2((port)->MODER, pin, 0x0)
#define PIN_ANALOGINPUT(port, pin)	PIN_MODE2((port)->MODER, pin, 0x3) // seteo en modo analogic
*/
void Set_OutputPIN(GPIO_TypeDef * PORT,uint32_t PIN);
void Set_InputPIN(GPIO_TypeDef * PORT,uint32_t PIN);
void Set_AnalogPIN(GPIO_TypeDef * PORT,uint32_t PIN);
void Set_channel_ADC(uint32_t channel);
void Set_InputPIN_PULLUP(GPIO_TypeDef * PORT,uint32_t PIN);


//Tipo de dato para resultados
typedef struct TS_value{
	uint16_t x;
	uint16_t y;
}TS_value;

// Actualizan el vector de mediciones del punto presionado
void updateTouchScreen();
void updateTouchScreen_x();
void updateTouchScreen_y();
uint8_t check_standby(void);
// Promediado de coordenadas y actualiza la variable global
void calculate_cords();
uint8_t get_TS_cords(TS_value* value_str);

// Funciones para calibrar pantalla
void Calibrate_display();
struct TS_value promediar_coordenadas(uint32_t muestras);
void Wait_for_touch(struct TS_value no_press_value);

// Indica si el valor obtenido es correcto o es un error
uint8_t is_valid_value(struct TS_value pantalla);
void mde_TS();

#ifdef __cplusplus
}
#endif
#endif /* INC_DRIVER_TOUCHSCREEN_H_ */
