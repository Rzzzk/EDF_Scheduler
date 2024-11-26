

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


/*timer1*/

void TIMER1_reset()
{
	T1TCR |=0x2;
	T1TCR &=~0x2; 
}

static void TIMER1_config()
{
	T1PR = 1000;
	T1TCR |=0x1;
}

void TIMER1_init()
{
	TIMER1_reset();
	TIMER1_config();
}

/****************************************************************/

#define task_1_counter_load (int)1000   // the counter of for loop of task 1
#define task_2_counter_load (int)1000  // the counter of for loop of task 2
#define task_1_tag 1	
#define task_2_tag 2

int i = 0 , j = 0; // counter

TaskHandle_t task_1_Handle = NULL; // task1 handler
TaskHandle_t task_2_Handle = NULL; // task1 handler

const TickType_t task_1_period = 10; // task1 delay 
const TickType_t task_2_period = 20; // task2 delay


int task1_timeIn = 0 , task1_timeOut = 0 , task1_totalTime = 0;
int task2_timeIn = 0 , task2_timeOut = 0 , task2_totalTime = 0;
int totalTimeOfSydtem = 0;
float cpu_load = 0.0;


/*the first task code*/
void task_1(void *pvParameters)
{
	/*Initialise the task_1_LastWakeTime variable with the current time*/
	TickType_t task_1_LastWakeTime = xTaskGetTickCount();
	/*set the task tag for task 1*/
	vTaskSetApplicationTaskTag( NULL, (void*)task_1_tag );
	
	while(1)
	{	
		/*togle pin8*/
		if(GPIO_read(PORT_0,PIN8)==PIN_IS_HIGH)
		{
			GPIO_write(PORT_0,PIN8,PIN_IS_LOW);
		}
		else
		{
			GPIO_write(PORT_0,PIN8,PIN_IS_HIGH);
		}
		
		for(i=0 ; i<task_1_counter_load ; i++){} 
				
		/*blocked for "task_1_Delay"*/ 
		vTaskDelayUntil( &task_1_LastWakeTime, task_1_period );

	}
}


/*the second task code*/
void task_2(void *pvParameters)
{
	/*Initialise the task_2_LastWakeTime variable with the current time*/
	TickType_t task_2_LastWakeTime = xTaskGetTickCount();
	/*set the task tag for task 2*/
	vTaskSetApplicationTaskTag( NULL, (void*)task_2_tag );
	
	while(1)
	{	
		/*togle pin9*/
		if(GPIO_read(PORT_0,PIN9)==PIN_IS_HIGH)
		{
			GPIO_write(PORT_0,PIN9,PIN_IS_LOW);
		}
		else
		{
			GPIO_write(PORT_0,PIN9,PIN_IS_HIGH);
		}
		
		for(i=0 ; i<task_2_counter_load ; i++){} 
		
		/*blocked for "task_2_Delay"*/
	  vTaskDelayUntil( &task_2_LastWakeTime, task_2_period );
	}
}



/*idle task hook code*/

void vApplicationIdleHook( void )
{
	GPIO_write(PORT_0,PIN0,PIN_IS_HIGH);
}

void vApplicationTickHook( void )
{
	GPIO_write(PORT_0,PIN7,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN7,PIN_IS_LOW);
}

/****************************************************************/

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	
	
	
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
	/*task creation*/
	xTaskPeriodicCreate(
               task_1,            /* Function that implements the task. */
               "task_1",          /* Text name for the task. */
               100,                /* Stack size in words, not bytes. */
               ( void * ) 1,      /* Parameter passed into the task. */
               1,                 /* Priority at which the task is created. */							 
               &task_1_Handle,
               task_1_period);  /* Used to pass out the created task's handle. */
							 
  xTaskPeriodicCreate(
               task_2,            /* Function that implements the task. */
               "task_2",          /* Text name for the task. */
               100,                /* Stack size in words, not bytes. */
               ( void * ) 1,      /* Parameter passed into the task. */
               3,                 /* Priority at which the task is created. */						 
               &task_2_Handle,
               task_2_period);  /* Used to pass out the created task's handle. */

  /*start scheduler*/
	vTaskStartScheduler();


	while(1)
	{
		
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/*configure timer1*/
	TIMER1_init();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


