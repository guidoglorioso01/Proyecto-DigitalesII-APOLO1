/*
 * tasks.c
 * En este archivo estan todas las tareas de la aplicacion
 *  Created on: Jan 10, 2024
 *      Author: guido
 */
#include <process.h>

extern void TouchGFX_Task(void const *argument);
extern QueueHandle_t queue_progres_var;
extern SemaphoreHandle_t semSaveData;
extern SemaphoreHandle_t semI2CResource;
extern SemaphoreHandle_t semProcessData; // semaforo para indicar si se puede tocar o no el buffer de entrada/salida
extern SemaphoreHandle_t semCalcCoefs;
extern SemaphoreHandle_t semCalcVolume;
extern SemaphoreHandle_t semDataReady;
void init_tasks(){

	/*
	 *					 ESQUEMA DE PRIORIDADES
	 *
	 * PROCESAR SEÑAL - CALCULAR FILTROS - SYNC DISP - TASK GFX -> idle + 2
	 * CALCULO DE VOLUMEN - GUARDAR DATOS USER - REFRESCAR TOUCH -> idle + 1
	 *
	 *NOTA: saveData amanece con mayor prioridad para tener los valores de config, luego se le baja la prioridad a idle + 1
	 * */


	xTaskCreate(Touchscreen_process, "UPD TS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(SaveData_process, "SaveData", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4 ,NULL);
	xTaskCreate(volume_process, "volSYS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1 ,NULL);
	xTaskCreate(upd_progressVar_process, "progVar", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1 ,NULL);
	xTaskCreate(TouchGFXSYNC_process, "GFX SYNC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(TouchGFX_Task, "UPD GFX", 3000, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(FilterData_process, "ProcessData", 2000, NULL, tskIDLE_PRIORITY + 3 ,NULL); 		// se le debe dar la mayor prioridad
	xTaskCreate(CalculateCoefs_process, "CalcCoefsData", 500, NULL, tskIDLE_PRIORITY + 3 ,NULL); 	// Process que calcula los coeficientes aun mas prioridad para tenerlos listos antes de convertir
	xTaskCreate(task_I2S_recieve, "Audio In", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3 ,NULL);	// Recepcion datos I2S -> Prioridad una menos que la de procesamiento

	initI2SDriver();
}


void init_semaphores(){

	semProcessData = xSemaphoreCreateBinary();
	semSaveData = xSemaphoreCreateBinary();
	semCalcCoefs = xSemaphoreCreateBinary();
	semI2CResource = xSemaphoreCreateBinary();
	semCalcVolume = xSemaphoreCreateBinary();
	semDataReady= xSemaphoreCreateBinary();

	xSemaphoreGive(semCalcVolume);
	xSemaphoreGive(semProcessData);
	xSemaphoreGive(semI2CResource);
	xSemaphoreTake(semCalcCoefs,0);
	xSemaphoreTake(semDataReady,0);

}


//////////////////////////Tareas///////////////////

void Touchscreen_process(void *arguments){

	while(1){
		mde_TS();
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void TouchGFXSYNC_process(void *arguments){
	extern void touchgfxSignalVSync(void);

	while(1){
		touchgfxSignalVSync();
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}


void SaveData_process(void *arguments){
	get_config_esp();
	init_userdata();

	vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 1); // Una vez inicializado ya no necesita tanta prioridad

	while(1){

		xSemaphoreTake(semSaveData,portMAX_DELAY);
		if(save_config_esp() == SUCCESS)
			send_cmd_esp(SAVE_FLASH_CMD);

	}
}

void FilterData_process(void *arguments){

	vTaskDelay(pdMS_TO_TICKS(2000));
	filter_init_system(); // inicializo los filtros

	while(1){

		// Verifico si hay datos nuevos para filtrar
		xSemaphoreTake(semDataReady,portMAX_DELAY);
		// Verifico que los filtros no estan siendo configurados
		xSemaphoreTake(semProcessData,portMAX_DELAY);
		process_filter();
		xSemaphoreGive(semProcessData);
		}
}

void CalculateCoefs_process(void *arguments){

	while(1){
		xSemaphoreTake(semCalcCoefs,portMAX_DELAY);

		//Como se cambian los filtros no se pueden usar.
		xSemaphoreTake(semProcessData,portMAX_DELAY);
		filter_init_system();
		xSemaphoreGive(semProcessData);
	}

}

void volume_process(void *arguments){
	while(1){
		xSemaphoreTake(semCalcVolume,portMAX_DELAY);
		process_set_gains();

		//vTaskDelay(pdMS_TO_TICKS(50));
	}

}


void upd_progressVar_process(void *arguments){
	uint8_t valor_progress=0;
	while(1){
		valor_progress = get_music_estate_esp();

		xQueueSend(queue_progres_var,&valor_progress,portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
