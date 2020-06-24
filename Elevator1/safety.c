/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * This file defines the safety module, which observes the running
 * elevator system and is able to stop the elevator in critical
 * situations
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

#include "global.h"
#include "assert.h"

#define POLL_TIME (10 / portTICK_RATE_MS)

#define MOTOR_UPWARD   (TIM3->CCR1)
#define MOTOR_DOWNWARD (TIM3->CCR2)
#define MOTOR_STOPPED  (!MOTOR_UPWARD && !MOTOR_DOWNWARD)
#define MOTOR_RUNNING  (MOTOR_UPWARD || MOTOR_DOWNWARD)

#define STOP_PRESSED  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#define AT_FLOOR      GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define DOORS_CLOSED  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
#define DOORS_OPEN		!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)

#define TRACK_POSITION getCarPosition()

#define FLOOR_1_POSITION   0
#define FLOOR_2_POSITION 400
#define FLOOR_3_POSITION 800
//On floor correctness we add a threshold of 1
#define FLOOR_CORRECTNESS (FLOOR_1_POSITION || FLOOR_2_POSITION)

static portTickType xLastWakeTime;

int doors_opened_run_once = 0;
int doors_arrived_at_floor = 0;
int doors_opened = 0;

static void check(u8 assertion, char *name) {
  if (!assertion) {
    printf("SAFETY REQUIREMENT %s VIOLATED: STOPPING ELEVATOR\n", name);
    for (;;) {
	  setCarMotorStopped(1);
  	  vTaskDelayUntil(&xLastWakeTime, POLL_TIME);
    }
  }
}

static void safetyTask(void *params) {
  s16 timeSinceStopPressed = -1;
	s16 timeSinceStopReleased = -1;
	s16 timeSinceFloorPosition = -1;
	s32 carPositionBefore = -1;
	s32 carPositionAfter = -1;
	s32 timeSinceCarPosition = -1;
	float carSpeed = -1;
  xLastWakeTime = xTaskGetTickCount();
	
	
  for (;;) {
    // Environment assumption 1: the doors can only be opened if
	//                           the elevator is at a floor and
    //                           the motor is not active

	check((AT_FLOOR && MOTOR_STOPPED) || DOORS_CLOSED,
	      "env1");

	// fill in environment assumption 2: The elevator moves at
	//																	a maximum speed of
	//																	50cm/s
	// if the motor is running
	if (MOTOR_RUNNING) {
	  if (timeSinceCarPosition < 0)
		{
			//set a timer 
			timeSinceCarPosition = 0;
			//set first position
			carPositionBefore = getCarPosition();
		}
      else
	    timeSinceCarPosition += POLL_TIME;
			//when the timer is 1000 (1 second)
			if(timeSinceCarPosition * portTICK_RATE_MS >= 1000)
			{
				//set the position after 1 sec
				carPositionAfter = getCarPosition();
				//get the difference between these two values
				carSpeed = ( ( (carPositionAfter - carPositionBefore) * 1.0) );
				//printf("carSpeed: %f\n", carSpeed);
				//check if the speed is more than 50
				check(carSpeed <= 50.0 , "env2");
	
				timeSinceCarPosition = -1;
				carPositionBefore = -1;
				carPositionAfter = -1;
				
			}
	} else {
	  timeSinceCarPosition = -1;
	}

	// fill in environment assumption 3: If the ground floor is put
	//																	at 0cm in an absolute coordinate
	//																	system, the second floor is at
	//																	400cm and the third floor at
	//																	800cm (the at-floor sensors
	//																	reports a floor with a threshold
	//																	of +-0.5cm)
	
	//if the elevator is at a floor
	if(AT_FLOOR)
	{
		//check if track position is 0, 400 or 800
		check((TRACK_POSITION == FLOOR_1_POSITION) || (TRACK_POSITION == FLOOR_2_POSITION) || (TRACK_POSITION == FLOOR_3_POSITION) ||
		(TRACK_POSITION == FLOOR_1_POSITION + 1) || (TRACK_POSITION == FLOOR_2_POSITION + 1) || (TRACK_POSITION == FLOOR_3_POSITION + 1) ||
		(TRACK_POSITION == FLOOR_1_POSITION - 1) || (TRACK_POSITION == FLOOR_2_POSITION - 1) || (TRACK_POSITION == FLOOR_3_POSITION - 1), 
		"env3");
	}
	
// fill in your own environment assumption 4: the car speed needs to be less than 
				//20 cm/s when the position is close to the floor target
				// if track_position is close to FLOOR_1
				if((TRACK_POSITION <= FLOOR_1_POSITION + 10) && getCarTargetPosition() == FLOOR_1_POSITION)
				{
					//check if car speed is less than 20 cm/s
					check(carSpeed <= 20.0 , "env4");
				}
				if((TRACK_POSITION >= FLOOR_2_POSITION - 10 && TRACK_POSITION <= FLOOR_2_POSITION + 10) && getCarTargetPosition() == FLOOR_2_POSITION)
				{
					//check if car speed is less than 20 cm/s
					check(carSpeed <= 20.0 , "env4");
				}
				if ((TRACK_POSITION >= FLOOR_3_POSITION - 10) && getCarTargetPosition() == FLOOR_3_POSITION)
				{
					//check if car speed is less than 20 cm/s
					check(carSpeed <= 20.0 , "env4");
				}
	

    // System requirement 1: if the stop button is pressed, the motor is
	//                       stopped within 1s

	if (STOP_PRESSED) {
	  if (timeSinceStopPressed < 0)
	    timeSinceStopPressed = 0;
      else
	    timeSinceStopPressed += POLL_TIME;

      check(timeSinceStopPressed * portTICK_RATE_MS <= 1000 || MOTOR_STOPPED,
	        "req1");
	} else {
	  timeSinceStopPressed = -1;
	}

    // System requirement 2: the motor signals for upwards and downwards
	//                       movement are not active at the same time

    check(!MOTOR_UPWARD || !MOTOR_DOWNWARD,
          "req2");

	// fill in safety requirement 3: The elevator may not pass
	// 															 the end positions, that is,
	// 															 go through the roof or the
	// 															 floor
	check(TRACK_POSITION >= FLOOR_1_POSITION && TRACK_POSITION <= FLOOR_3_POSITION, "req3");

	// fill in safety requirement 4: A moving elevator halts
	//															only if the stop button
	//															is pressed or the elevator
	//															has arrived at a floor
	
	//In case the stop button is released
	if (STOP_RELEASED) {
	  if (timeSinceStopReleased < 0)
	    timeSinceStopReleased = 0;
      else
	    timeSinceStopReleased += POLL_TIME;
			//check if the motor is not running within 1 sec
      check( (timeSinceStopReleased * portTICK_RATE_MS <= 1000 || MOTOR_STOPPED) || 
			( MOTOR_RUNNING ) || (MOTOR_STOPPED && 
	( (TRACK_POSITION == FLOOR_1_POSITION) || (TRACK_POSITION == FLOOR_2_POSITION) || (TRACK_POSITION == FLOOR_3_POSITION) || STOP_PRESSED) ),
	        "req4");
	} else {
	  timeSinceStopReleased = -1;
		//else check if the motor runs or stop button is pressed or the elevator is on the desired position while the motor is stopped
		check( ( MOTOR_RUNNING ) || (MOTOR_STOPPED && 
	( (TRACK_POSITION == FLOOR_1_POSITION) || (TRACK_POSITION == FLOOR_2_POSITION) || (TRACK_POSITION == FLOOR_3_POSITION) || STOP_PRESSED) ) 
	, "req4");
	}
	
	
	// fill in safety requirement 5: Once the elevator has stopped
	//																at a floor, it will wait at 
	//																least 1s before it continues
	//																to another floor
	
	//if the position is in one of the floors and the motor is stopped
	if( AT_FLOOR && MOTOR_STOPPED)
	{
		
		//count the timeSinceFloorPosition timer for calculating how long the motor is stopped
		if (doors_arrived_at_floor == 0)
		{
			doors_opened = 0;
			doors_arrived_at_floor = 1;
		}
		if (timeSinceFloorPosition < 0)
	    timeSinceFloorPosition = 0;
      else			
	    timeSinceFloorPosition += POLL_TIME;
		//if the door is opened once,
		//set the global variable doors_opened to 1
		if(DOORS_OPEN && doors_opened_run_once == 0){
			doors_opened = 1;
			doors_opened_run_once = 1;
			}			
	}
	//if the elevator is not at the floor
	if( !AT_FLOOR )
	{
		//check if the timeSinceFloorPosition timer is more than one second or is negative (the initial value)
		check( ( timeSinceFloorPosition * portTICK_RATE_MS > 1000) || (timeSinceFloorPosition < 0) , "req5");
		timeSinceFloorPosition = -1;
		doors_arrived_at_floor = 0;
		doors_opened_run_once = 0;
		// fill in safety requirement 6: If the elevator arrived to a floor, the doors must be opened at least once before leaving another floor
	
		check(doors_opened, "req6");
	}
	
	
	
	// fill in safety requirement 7
	check(1, "req7");

	vTaskDelayUntil(&xLastWakeTime, POLL_TIME);
  }
	

}

void setupSafety(unsigned portBASE_TYPE uxPriority) {
  xTaskCreate(safetyTask, "safety", 100, NULL, uxPriority, NULL);
}
