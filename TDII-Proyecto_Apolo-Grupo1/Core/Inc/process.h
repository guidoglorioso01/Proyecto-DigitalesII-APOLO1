/*
 * tasks.h
 *
 *  Created on: Jan 10, 2024
 *      Author: guido
 */

#include <Control_DatosUsuario.h>
#include <Control_ProcesamientoAudio.h>
#include <Driver_Display.h>
#include <Driver_I2C.h>
#include <Driver_I2S.h>
#include <Driver_TouchScreen.h>
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"


#ifndef INC_TASKS_H_
#define INC_TASKS_H_

void init_semaphores();
void init_tasks();

void TouchGFXSYNC_process(void *arguments);
void Touchscreen_process(void *arguments);
void SaveData_process(void *arguments);
void FilterData_process(void *arguments);
void CalculateCoefs_process(void *arguments);
void volume_process(void *arguments);
void upd_progressVar_process(void *arguments);



#endif /* INC_TASKS_H_ */
