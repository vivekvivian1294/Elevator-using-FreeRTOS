/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * Class for keeping track of the car position.
 */

#include "FreeRTOS.h"
#include "task.h"

#include "position_tracker.h"

#include "assert.h"
//for printf purposes
#include <stdio.h>

#define POLL_TIME (3 / portTICK_RATE_MS)

static portTickType xLastWakeTime;

static void positionTrackerTask(void *params) {

  // ...
	PositionTracker *tracker = (PositionTracker *)params;
	int toggle = 1;
	for(;;)
	{
		//Update position
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == 1 && toggle == 1)
		{
			toggle = 0;
			printf("Tracker position : %ld\n", tracker->position);
			if(tracker->direction == Up)
			{
				tracker->position += 1;
			}
			else if (tracker->direction == Down)
			{
				tracker->position -= 1;
			}
			
		}
		else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == 0 && toggle == 0)
		{
			toggle = 1;
		}
		//vTaskDelay(3);
		vTaskDelayUntil(&xLastWakeTime, POLL_TIME);
	}
  

}

void setupPositionTracker(PositionTracker *tracker,
                          GPIO_TypeDef * gpio, u16 pin,
						  portTickType pollingPeriod,
						  unsigned portBASE_TYPE uxPriority) {
  portBASE_TYPE res;

  tracker->position = 0;
  tracker->lock = xSemaphoreCreateMutex();
  assert(tracker->lock != NULL);
  tracker->direction = Unknown;
  tracker->gpio = gpio;
  tracker->pin = pin;
  tracker->pollingPeriod = pollingPeriod;

  res = xTaskCreate(positionTrackerTask, "position tracker",
                    80, (void*)tracker, uxPriority, NULL);
  assert(res == pdTRUE);
}

void setDirection(PositionTracker *tracker, Direction dir) {

  // ...
	tracker->direction = dir;

}

s32 getPosition(PositionTracker *tracker) {

  // ...
	
  return tracker->position;

}
void updatePosition(PositionTracker *tracker, Direction dir)
{
	if(dir == Up)
	{
		tracker->position += 50;
	}
	else if (dir == Down)
	{
		tracker->position -= 50;
	}
}
