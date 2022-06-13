#include <stdio.h>
#include "system.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"

#define DE2_PIO_GREENLED9_BASE 0x90e0
#define DE2_PIO_REDLED18_BASE 0x9120
#define D2_PIO_KEYS4_BASE 0x9140
#define DE2_PIO_TOGGLES18_BASE 0x9150

#define LED_RED_0 0x00000001 // Engine
#define LED_RED_1 0x00000002 // Top Gear

// #define LED_GREEN_0 0x0001 // Cruise Control button
#define LED_GREEN_2 0x0004 // Cruise Control activated
#define LED_GREEN_4 0x0010 // Brake Pedal
#define LED_GREEN_6 0x0040 // Gas Pedal

/* Push Button Patterns */

#define GAS_PEDAL_FLAG      0x08
#define BRAKE_PEDAL_FLAG    0x04
#define CRUISE_CONTROL_FLAG 0x02
/* Toggle Switch Patterns */

#define TOP_GEAR_FLAG       0x00000002
#define ENGINE_FLAG         0x00000001

// #define STARTTASK_PRIO     5
// #define VEHICLETASK_PRIO  10
// #define CONTROLTASK_PRIO  12
#define switchIO_PRIO 13
#define buttonIO_PRIO 14

#define TASK_STACKSIZE 2048
OS_STK SwitchIOTask_Stack[TASK_STACKSIZE]; 
OS_STK ButtonIOTask_Stack[TASK_STACKSIZE];

//Here we declare the timer variables

OS_TMR *switchTaskTimer;
OS_TMR *buttonTaskTimer;

//Here we declare the semaphores

OS_EVENT *switchTaskSem;
OS_EVENT *buttonTaskSem;
// Here we declare the mailbox flags
OS_EVENT *Mbox_Brake;
OS_EVENT *Mbox_Engine;
OS_EVENT *throttle_flag;
OS_EVENT *cruise_flag;
OS_EVENT *topgear_flag;

enum active {on = 2, off = 1};

enum active top_gear = off; //global
enum active engine = off; //global


void buttonTimerCallback (void *ptmr, void *callback_arg)
{
	OSSemPost(callback_arg);
}
void switchTimerCallback (void *ptmr, void *callback_arg)
{
	OSSemPost(callback_arg);
}
void SwitchIO(void* pdata)
{
	alt_u8 keyVal_hex;
	void* msg;
	INT8U err,semerror;
	INT16S* current_velocity_ptr;
	INT16S current_velocity;

	while(1)
	{
		keyVal_hex = IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE); //This returns a binary no. with all set bits equal to one
		msg = OSMboxPend(Mbox_Velocity, 1, &err);
    	if (err == OS_NO_ERR)
    	{ 
      		current_velocity_ptr = (INT16S*) msg;
      		current_velocity = *current_velocity_ptr;
      	}
		 //This sets the red LED register in correspondance to the toggle switch register
		
		if (keyVal_hex & ENGINE_FLAG)
		{
			IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_0);
			printf("Engine is turned ON\n");
			engine = on;
		}
		else 
		{
			//pend the mailbox to get current velocity here
			if(current_velocity==0)
			{
				IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_REDLED18_BASE,LED_RED_0);
				printf("Engine is turned OFF\n");
				engine = off;
			}

		}
		if(engine == on)
		{
			if (keyVal_hex & TOP_GEAR_FLAG)
			{
				IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_1);
				printf("Top gear is ENGAGED\n");
				top_gear = on;
			}
			else
			{
				IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_REDLED18_BASE,LED_RED_1);
				printf("Top gear is DISENGAGED\n");
				top_gear = off;
			}
		}
		else
		{
				IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_REDLED18_BASE,LED_RED_1);
				printf("Top gear is DISENGAGED\n");
				top_gear = off;

		}
		err = OSMboxPostOpt(Mbox_Engine, (void *) &engine, OS_POST_OPT_BROADCAST);
		err = OSMboxPostOpt(topgear_flag, (void *) &top_gear, OS_POST_OPT_BROADCAST);
		OSTmrStart(switchTaskTimer, &err);
		OSSemPend(pdata,0,&semerror);
		// OSTimeDlyHMSM(0,0,0,300);//Period unknown for now, will need to change
	}
} 
void ButtonIO(void* pdata)
{
	alt_u8 pbVal_hex;
	INT8U err,semerror;
	void* msg;
	INT16S* current_velocity_ptr;
	INT16S current_velocity;

	enum active gas_pedal = off;// local to button only
	enum active brake_pedal = off;// local button only
	enum active cruise_control = off;//local button only
	while(1)
	{
		pbVal_hex = ~IORD_ALTERA_AVALON_PIO_DATA(D2_PIO_KEYS4_BASE);
		msg = OSMboxPend(Mbox_Velocity, 1, &err);
    	if (err == OS_NO_ERR)
    	{ 
      		current_velocity_ptr = (INT16S*) msg;
      		current_velocity = *current_velocity_ptr;
      	}
	 /*This is Pb 1 for cruise control
         *
         */
		if (engine == on)
		{
	        if ((pbVal_hex & CRUISE_CONTROL_FLAG) && gas_pedal==off && brake_pedal ==off && top_gear == on && current_velocity >=20) //Use & here because we just check that certain bit and don't care about the others
	        {

	            IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_2); //This function takes a decimal or hex and sets the corresponding bits
	            
	            printf("CRUISE CONTROL ON! via Pb 1\n");
	            cruise_control = on;

	 
	        }
	        else
	        {
	            IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_2); //Using Clear_bits you just pass the bits you want to clear
	            
	            printf("CRUISE CONTROL OFF! via Pb 1\n");
	            cruise_control = off;
	        }
	        /*This is Pb 2 for brakes
	         *
	         */
	        if (pbVal_hex & BRAKE_PEDAL_FLAG) //Use & here because we just check that certain bit and don't care about the others
	        {
	            IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_4); //This function takes a decimal or hex and sets the corresponding bits
	            
	            printf("BRAKES APPLIED! via Pb 2\n");
	            brake_pedal = on;
	 
	        }
	        else
	        {
	            IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_4); //Using Clear_bits you just pass the bits you want to clear
	            
	            printf("BRAKES DISENGAGED! via Pb 2\n");
	            brake_pedal = off;
	        }
	 
	        /*This is Pb 3 for Gas Pedal
	         *
	         */
	 
	        if ((pbVal_hex & GAS_PEDAL_FLAG) && brake_pedal==off) //Use & here because we just check that certain bit and don't care about the others
	        {
	            IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_6); //This function takes a decimal or hex and sets the corresponding bits
	            
	            printf("ACCELERATOR APPLIED! via Pb 3\n");
	            gas_pedal=on;
	 
	        }
	        else
	        {
	            IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_6); //Using Clear_bits you just pass the bits you want to clear
	            
	            printf("ACCELERATOR DISENGAGED! via Pb 3\n");
	            gas_pedal=off;

	        }
    	}
    	else
    	{

    		  IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_2); //Using Clear_bits you just pass the bits you want to clear
	          printf("CRUISE CONTROL OFF! via Pb 1\n");
	          cruise_control=off;

	          IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_4); //Using Clear_bits you just pass the bits you want to clear
	          printf("BRAKES DISENGAGED! via Pb 2\n");
	          brake_pedal=off;

	          IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(DE2_PIO_GREENLED9_BASE,LED_GREEN_6); //Using Clear_bits you just pass the bits you want to clear
	          printf("ACCELERATOR DISENGAGED! via Pb 3\n");
	          gas_pedal=off;
    	}

    	err = OSMboxPostOpt(Mbox_Brake, (void *) &brake_pedal, OS_POST_OPT_BROADCAST);
		err = OSMboxPostOpt(cruise_flag, (void *) &cruise_control, OS_POST_OPT_BROADCAST);
		err = OSMboxPostOpt(throttle_flag, (void *) &gas_pedal, OS_POST_OPT_BROADCAST);
        printf("**********************\n");

        OSTmrStart(buttonTaskTimer, &err);

        OSSemPend(pdata,0,&semerror);
        //OSTimeDlyHMSM(0,0,0,300);//Period unknown for now, will need to change
 
    }
	


}

int main(void) {
  printf("iotasks_test:\n");

  switchTaskSem=OSSemCreate(0);
  buttonTaskSem=OSSemCreate(0);




  INT8U err1,err2;

 switchTaskTimer=OSTmrCreate(
 					 		10, //delay
					 		300,//period
					 		OS_TMR_OPT_PERIODIC,
					 		switchTimerCallback, //get back to this after making semaphores
					 		switchTaskSem, //get back to this after making semaphores
					 		"switchTaskTimer",
					 		&err1);
 buttonTaskTimer=OSTmrCreate(
 							10, //delay
					 		300,//period
					 		OS_TMR_OPT_PERIODIC,
					 		buttonTimerCallback, //get back to this after making semaphores
					 		buttonTaskSem, //get back to this after making semaphores
							"buttonTaskTimer",
					 		&err2);

 if (err1 == OS_ERR_NONE) 
 	{
		printf("switch timer was created but not started\n");
	}
 if (err2 == OS_ERR_NONE) 
 	{
		printf("button timer was created but not started\n");
	}

  OSTaskCreateExt(
		  SwitchIO, // Pointer to task code
		  switchTaskSem,      // Pointer to argument that is
		  // passed to task
		  (void *)&SwitchIOTask_Stack[TASK_STACKSIZE-1], // Pointer to top
		  // of task stack 
		  switchIO_PRIO,
		  switchIO_PRIO,
		  (void *)&SwitchIOTask_Stack[0],
		  TASK_STACKSIZE,
		  (void *) 0,  
		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(
		  ButtonIO, // Pointer to task code
		  buttonTaskSem,      // Pointer to argument that is
		  // passed to task
		  (void *)&ButtonIOTask_Stack[TASK_STACKSIZE-1], // Pointer to top
		  // of task stack 
		  buttonIO_PRIO,
		  buttonIO_PRIO,
		  (void *)&ButtonIOTask_Stack[0],
		  TASK_STACKSIZE,
		  (void *) 0,  
		  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	 
  OSStart();
  
  return 0;
}
