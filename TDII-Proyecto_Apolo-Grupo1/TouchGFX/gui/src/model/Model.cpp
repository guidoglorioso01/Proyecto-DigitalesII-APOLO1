#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern "C"{
	xQueueHandle queue_progres_var;
}
unsigned char value=0;


Model::Model() : modelListener(0)
{
	queue_progres_var = xQueueCreate(10,sizeof(uint8_t));
}

void Model::tick()
{

	if(xQueueReceive(queue_progres_var, &value, 0) == pdTRUE){
		modelListener->presenter_set_progress_music(value);
	}

}
