/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * Functions listening for changes of specified pins
 */

#include "FreeRTOS.h"
#include "task.h"

#include "pin_listener.h"
#include "assert.h"

//for printf purposes
//#include <stdio.h>
//Counter for pins
u8 pin0Status = 0;
u8 pin1Status = 0;
u8 pin2Status = 0;
u8 pin3Status = 0;
u8 pin7Status = 0;
u8 pin8Status = 0;

static void pollPin(PinListener *listener,
                    xQueueHandle pinEventQueue) {

  // ...
	//if a pin is 1
	//check its status or update it
	if(GPIO_ReadInputDataBit(listener->gpio, listener->pin) == 1)
	{
		//check if listener->pin is 0
		if(listener->pin == GPIO_Pin_0 && pin0Status == 1)
		{
			//Send event for pin 0
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, 100);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin0Status = 2;
		}
		//in case that pin0Status = 0
		else if(listener->pin == GPIO_Pin_0 && pin0Status == 0)
		{
			//Update its status
			pin0Status = 1;
		}
		//check if listener->pin is 1
		if(listener->pin == GPIO_Pin_1 && pin1Status == 1)
		{
			//Send event for pin 1
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, portMAX_DELAY);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin1Status = 2;
		}
		//in case that pinXStatus = 0
		else if(listener->pin == GPIO_Pin_1 && pin1Status == 0)
		{
			//Update its status
			pin1Status = 1;
		}
		//check if listener->pin is 2
		if(listener->pin == GPIO_Pin_2 && pin2Status == 1)
		{
			//Send event for pin 2
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, portMAX_DELAY);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin2Status = 2;
		}
		//in case that pinXStatus = 0
		else if(listener->pin == GPIO_Pin_2 && pin2Status == 0)
		{
			//Update its status
			pin2Status = 1;
		}
		//check if listener->pin is 3
		if(listener->pin == GPIO_Pin_3 && pin3Status == 1)
		{
			//Send event for pin 3
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, portMAX_DELAY);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin3Status = 2;
		}
		//in case that pinXStatus = 0
		else if(listener->pin == GPIO_Pin_3 && pin3Status == 0)
		{
			//Update its status
			pin3Status = 1;
		}
		//check if listener->pin is 7
		if(listener->pin == GPIO_Pin_7 && pin7Status == 1)
		{
			//Send event for pin 7
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, portMAX_DELAY);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin7Status = 2;
		}
		//in case that pinXStatus = 0
		else if(listener->pin == GPIO_Pin_7 && pin7Status == 0)
		{
			//Update its status
			pin7Status = 1;
			
		}
		//check if listener->pin is 8
		if(listener->pin == GPIO_Pin_8 && pin8Status == 1)
		{
			//Send event for pin 8
			xQueueSend(pinEventQueue, (void *) &listener->risingEvent, portMAX_DELAY);
			//Update pinXStatus to 2 (so it sends the event only once)
			pin8Status = 2;
		}
		//in case that pinXStatus = 0
		else if(listener->pin == GPIO_Pin_8 && pin8Status == 0)
		{
			//Update its status
			pin8Status = 1;
		}
	}
	else
	{
		//if it is 0
		//check if listener->pin is 0
		if(listener->pin == GPIO_Pin_0)
		{
			//update pinXStatus to 0
			pin0Status = 0;
		}
		//check if listener->pin is 1
		if(listener->pin == GPIO_Pin_1)
		{
			//update pinXStatus to 0
			pin1Status = 0;
		}
		//check if listener->pin is 2
		if(listener->pin == GPIO_Pin_2)
		{
			//update pinXStatus to 0
			pin2Status = 0;
		}
		//check the case if listener->pin is 3 and pin3Status is 2 to send a falling event
		if(listener->pin == GPIO_Pin_3 && pin3Status == 2)
		{
			xQueueSend(pinEventQueue, (void *) &listener->fallingEvent, portMAX_DELAY);
		}
		//check if listener->pin is 3
		if(listener->pin == GPIO_Pin_3)
		{
			//update pinXStatus to 0
			pin3Status = 0;
		}
		//check the case if listener->pin is 7 and pin7Status is 2 to send a falling event
		if(listener->pin == GPIO_Pin_7 && pin7Status == 2)
		{
			xQueueSend(pinEventQueue, (void *) &listener->fallingEvent, portMAX_DELAY);
		}
		//check if listener->pin is 7
		if(listener->pin == GPIO_Pin_7)
		{
			//update pinXStatus to 0
			pin7Status = 0;
		}
		//check the case if listener->pin is 8 and pin8Status is 2 to send a falling event
		if(listener->pin == GPIO_Pin_8 && pin8Status == 2)
		{
			xQueueSend(pinEventQueue, (void *) &listener->fallingEvent, portMAX_DELAY);
		}
		//check if listener->pin is 8
		if(listener->pin == GPIO_Pin_8)
		{
			//update pinXStatus to 0
			pin8Status = 0;
		}
	}
}

static void pollPinsTask(void *params) {
  PinListenerSet listeners = *((PinListenerSet*)params);
  portTickType xLastWakeTime;
  int i;

  xLastWakeTime = xTaskGetTickCount();

  for (;;) {
    for (i = 0; i < listeners.num; ++i)
	  pollPin(listeners.listeners + i, listeners.pinEventQueue);
    
	vTaskDelayUntil(&xLastWakeTime, listeners.pollingPeriod);
  }
}

void setupPinListeners(PinListenerSet *listenerSet) {
  portBASE_TYPE res;

  res = xTaskCreate(pollPinsTask, "pin polling",
                    100, (void*)listenerSet,
					listenerSet->uxPriority, NULL);
  assert(res == pdTRUE);
}
