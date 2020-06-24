/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * The planner module, which is responsible for consuming
 * pin/key events, and for deciding where the elevator
 * should go next
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "global.h"
#include "planner.h"
#include "assert.h"

//include library for GPIO
#include "stm32f10x_gpio.h"


#define FLOOR_1   0
#define FLOOR_2 400
#define FLOOR_3 800

#define DOORS_OPEN		!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)

//the message that is received from pin_listener
PinEvent message;

//the queue that should be used for planner
xQueueHandle plannerQueue;

//boolean variables for controling if a message to a floor is received,
//and the plannerQueue has not completed the task for it
u8 messageToFloor1 = 0;
u8 messageToFloor2 = 0;
u8 messageToFloor3 = 0;

//set desired car position variable (initial value 0)
s32 desiredCarPosition = 0;

static void plannerTask(void *params) {

  // ...
	//create the planner queue
	plannerQueue = xQueueCreate(32, sizeof(PinEvent));
  assert(plannerQueue!= NULL);
	
	
	for(;;)
	{
		//if the door is open
		if(DOORS_OPEN)
		{
			//delay the planner for 300 ms
			vTaskDelay(300);
		}
		//else,
		else
		{
			//wait and check every 100ms if a message is received
			if(xQueueReceive(pinEventQueue, &message, 100))
			{
				//if message is TO_FLOOR_1 and messageToFloor1 = 0 and carPosition != FLOOR_1
				if(message == TO_FLOOR_1 && messageToFloor1 == 0 && getCarPosition() != FLOOR_1)
				{
					//Do event for TO_FLOOR_1
					printf("Message received! Message: TO_FLOOR_1\n");
					//wait 1 sec
					//vTaskDelay(1000);
					//setCarTargetPosition(0);
					//set messageToFloor1 to 1
					messageToFloor1 = 1;
					//send message to plannerQueue
					xQueueSend(plannerQueue, (void *) &message, portMAX_DELAY);
					printf("Send message TO_FLOOR_1 to plannerQueue!\n");
				}
				//if message is TO_FLOOR_2 and messageToFloor2 = 0 and carPosition != FLOOR_2 and desiredCarPosition = FLOOR_3 and getCarPosition < FLOOR_2 - 50,
				//this is the case if the elevator goes to FLOOR_3 and the button TO_FLOOR_2 is pressed while the current position of the elevator is far away from
				//FLOOR_2
				else if(message == TO_FLOOR_2 && messageToFloor2 == 0 && getCarPosition() != FLOOR_2 && desiredCarPosition == FLOOR_3 && getCarPosition() < FLOOR_2 - 50)
				{
					//Do event for TO_FLOOR_2 while it is going to FLOOR_3
					printf("Message received! Message: TO_FLOOR_2\n");
					//set messageToFloor2 to 1
					//send message to plannerQueue but in front
					xQueueSendToFront(plannerQueue, (void *) &message, portMAX_DELAY);
					printf("Send message TO_FLOOR_2 to plannerQueue in front!\n");
				}
				//if message is TO_FLOOR_2 and messageToFloor2 = 0 and carPosition != FLOOR_2 and desiredCarPosition = FLOOR_1 and getCarPosition < FLOOR_2 + 50,
				//this is the case if the elevator goes to FLOOR_1 and the button TO_FLOOR_2 is pressed while the current position of the elevator is far away from
				//FLOOR_2
				else if(message == TO_FLOOR_2 && messageToFloor2 == 0 && getCarPosition() != FLOOR_2 && desiredCarPosition == FLOOR_1 && getCarPosition() > FLOOR_2 + 50)
				{
					//Do event for TO_FLOOR_2 while it is going to FLOOR_3
					printf("Message received! Message: TO_FLOOR_2\n");
					//set messageToFloor2 to 1
					//send message to plannerQueue but in front
					xQueueSendToFront(plannerQueue, (void *) &message, portMAX_DELAY);
					printf("Send message TO_FLOOR_2 to plannerQueue in front!\n");
				}
				//if message is TO_FLOOR_2 and messageToFloor2 = 0 and carPosition != FLOOR_2
				else if (message == TO_FLOOR_2 && messageToFloor2 == 0 && getCarPosition() != FLOOR_2)
				{
					//Do event for TO_FLOOR_2
					printf("Message received! Message: TO_FLOOR_2\n");
					//Wait 1 sec
					//vTaskDelay(1000);
					//setCarTargetPosition(400);
					//set messageToFloor2 to 1
					messageToFloor2 = 1;
					//send message to plannerQueue
					xQueueSend(plannerQueue, (void *) &message, portMAX_DELAY);
					printf("Send message TO_FLOOR_2 to plannerQueue!\n");
				}
				//if message is TO_FLOOR_3 and messageToFloor3 = 0 and carPosition != FLOOR_3
				else if (message == TO_FLOOR_3 && messageToFloor3 == 0 && getCarPosition() != FLOOR_3)
				{
					//Do event for TO_FLOOR_3
					printf("Message received! Message: TO_FLOOR_3\n");
					//Wait 1 sec
					//vTaskDelay(1000);
					//setCarTargetPosition(800);
					//setCarTargetPosition(800);
					//set messageToFloor3 to 1
					messageToFloor3 = 1;
					//send message to plannerQueue
					xQueueSend(plannerQueue, (void *) &message, portMAX_DELAY);
					printf("Send message TO_FLOOR_3 to plannerQueue!\n");
				}
				//if message is STOP_PRESSED
				else if (message == STOP_PRESSED)
				{
					//Do event for STOP_PRESSED
					printf("Message received! Message: STOP_PRESSED\n");
					setCarMotorStopped(1);
				}
				//if message is STOP_RELEASED
				else if (message == STOP_RELEASED)
				{
					//Do event for STOP_RELEASED
					printf("Message received! Message: STOP_RELEASED\n");
					setCarMotorStopped(0);
				}
				//if message is ARRIVED_AT_FLOOR
				else if (message == ARRIVED_AT_FLOOR)
				{
					//Do event for ARRIVED_AT_FLOOR
					printf("Message received! Message: ARRIVED_AT_FLOOR\n");
				}
				//if message is LEFT_FLOOR
				else if (message == LEFT_FLOOR)
				{
					//Do event for LEFT_FLOOR
					printf("Message received! Message: LEFT_FLOOR\n");
				}
				//if message is DOORS_CLOSED
				else if (message == DOORS_CLOSED)
				{
					//Do event for DOORS_CLOSED
					printf("Message received! Message: DOORS_CLOSED\n");
				}
				//if message is DOORS_OPENING
				else if (message == DOORS_OPENING)
				{
					//Do event for DOORS_OPENING
					printf("Message received! Message: DOORS_OPENING\n");
				}
			}
			
			//check if plannerQueue has a message but do not destroy it
			if(xQueuePeek(plannerQueue, &message, 100))
			{
				//if messsage is to floor 1
				if (message == TO_FLOOR_1)
				{
					//set desired car position to FLOOR_1
					desiredCarPosition = FLOOR_1;
					printf("DesiredCarPosition: FLOOR_1\n");
				}
				//if messsage is to floor 2
				else if(message == TO_FLOOR_2)
				{
					//set desired car position to FLOOR_2
					desiredCarPosition = FLOOR_2;
					printf("DesiredCarPosition: FLOOR_2\n");
				}
				//if messsage is to floor 3
				else if(message == TO_FLOOR_3)
				{
					//set desired car position to FLOOR_3
					desiredCarPosition = FLOOR_3;
					printf("DesiredCarPosition: FLOOR_3\n");
				}
				//setCarTargetPosition to the desired position
				setCarTargetPosition(desiredCarPosition);
				//if the car is at the desired position, check which is the message to turn off the boolean variable
				if(getCarPosition() == desiredCarPosition)
				{
					//if message is TO_FLOOR1
					if(message == TO_FLOOR_1)
						{
							//set messageToFloor1 to 0
							messageToFloor1 = 0;
							//remove message from plannerQueue
							xQueueReceive(plannerQueue, &message, portMAX_DELAY);
							//wait 3 seconds
							vTaskDelay(3000);
						}
					else if(message == TO_FLOOR_2)
						{
							//set messageToFloor2 to 0
							messageToFloor2 = 0;
							//remove message from plannerQueue
							xQueueReceive(plannerQueue, &message, portMAX_DELAY);
							//wait 3 seconds
							vTaskDelay(3000);
						}
					else if(message == TO_FLOOR_3)
						{
							//set messageToFloor3 to 0
							messageToFloor3 = 0;
							//remove message from plannerQueue
							xQueueReceive(plannerQueue, &message, portMAX_DELAY);
							//wait 3 seconds
							vTaskDelay(3000);
						}
				}
			}
		}
		
		
	}
	
	
  //vTaskDelay(100);
	//vTaskDelay(portMAX_DELAY);
}

void setupPlanner(unsigned portBASE_TYPE uxPriority) {
  xTaskCreate(plannerTask, "planner", 100, NULL, uxPriority, NULL);
}
