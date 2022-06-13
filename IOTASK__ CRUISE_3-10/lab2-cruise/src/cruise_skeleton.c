/* Cruise control skeleton for the IL 2206 embedded lab
 *
 * Maintainers:  Rodolfo Jordao (jordao@kth.se), George Ungereanu (ugeorge@kth.se)
 *
 * Description:
 *
 *   In this file you will find the "model" for the vehicle that is being simulated on top
 *   of the RTOS and also the stub for the control task that should ideally control its
 *   velocity whenever a cruise mode is activated.
 *
 *   The missing functions and implementations in this file are left as such for
 *   the students of the IL2206 course. The goal is that they get familiriazed with
 *   the real time concepts necessary for all implemented herein and also with Sw/Hw
 *   interactions that includes HAL calls and IO interactions.
 *
 *   If the prints prove themselves too heavy for the final code, they can
 *   be exchanged for alt_printf where hexadecimals are supported and also
 *   quite readable. This modification is easily motivated and accepted by the course
 *   staff.
 */
#include <stdio.h>
#include "system.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"

#define DEBUG 1

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Button Patterns */

#define GAS_PEDAL_FLAG      0x08
#define BRAKE_PEDAL_FLAG    0x04
#define CRUISE_CONTROL_FLAG 0x02
/* Switch Patterns */

#define TOP_GEAR_FLAG       0x00000002
#define ENGINE_FLAG         0x00000001

/* LED Patterns */

#define DE2_PIO_GREENLED9_BASE 0x90e0
#define DE2_PIO_REDLED18_BASE 0x9120
#define D2_PIO_KEYS4_BASE 0x9140
#define DE2_PIO_TOGGLES18_BASE 0x9150

#define LED_RED_0 0x00000001 // Engine
#define LED_RED_1 0x00000002 // Top Gear

#define LED_GREEN_0 0x0001 // Cruise Control activated
#define LED_GREEN_2 0x0002 // Cruise Control Button
#define LED_GREEN_4 0x0010 // Brake Pedal
#define LED_GREEN_6 0x0040 // Gas Pedal

//Position LEDS
#define LED_RED_17 0x20000
#define LED_RED_16 0x10000
#define LED_RED_15 0x8000
#define LED_RED_14 0x4000
#define LED_RED_13 0x2000
#define LED_RED_12 0x1000

/*
 * Definition of Tasks
 */

#define TASK_STACKSIZE 2048

OS_STK StartTask_Stack[TASK_STACKSIZE]; 
OS_STK ControlTask_Stack[TASK_STACKSIZE]; 
OS_STK VehicleTask_Stack[TASK_STACKSIZE];
OS_STK SwitchIOTask_Stack[TASK_STACKSIZE]; 
OS_STK ButtonIOTask_Stack[TASK_STACKSIZE];

// Task Priorities

#define STARTTASK_PRIO 5
#define switchIO_PRIO  10
#define buttonIO_PRIO  11
#define VEHICLETASK_PRIO  9
#define CONTROLTASK_PRIO  8


// Task Periods

#define CONTROL_PERIOD  300
#define VEHICLE_PERIOD  300
#define SWITCH_PERIOD  300
#define BUTTON_PERIOD  300

/*
 * Definition of Kernel Objects 
 */

// MailBoxes for Values
OS_EVENT *Mbox_Throttle;
OS_EVENT *Mbox_Velocity;

//MailBoxes for Flags
OS_EVENT *Mbox_Brake;
OS_EVENT *Mbox_Engine;
OS_EVENT *throttle_flag;
OS_EVENT *cruise_flag;
OS_EVENT *topgear_flag;

// Semaphores
OS_EVENT *switchTaskSem;
OS_EVENT *buttonTaskSem;
OS_TMR *vehicleTaskSem;
OS_TMR *controlTaskSem;
// SW-Timer
OS_TMR *switchTaskTimer;
OS_TMR *buttonTaskTimer;
OS_TMR *vehicleTaskTimer;
OS_TMR *controlTaskTimer;
/*
 * Types
 */
enum active {on = 2, off = 1};
enum active top_gear = off; //global
enum active engine = off; //global

/*
 * Global variables
 */
int delay; // Delay of HW-timer 
INT16U led_green = 0; // Green LEDs
INT32U led_red = 0;   // Red LEDs


/*
 * Helper functions
 */

void buttonTimerCallback (void *ptmr, void *callback_arg)
{
  OSSemPost(callback_arg);
}
void switchTimerCallback (void *ptmr, void *callback_arg)
{
  OSSemPost(callback_arg);
}

void vehicleTimerCallback (void *ptmr, void *callback_arg)
{
  OSSemPost(callback_arg);
}
void controlTimerCallback (void *ptmr, void *callback_arg)
{
  OSSemPost(callback_arg);
}

// int buttons_pressed(void)
// {
//   return ~IORD_ALTERA_AVALON_PIO_DATA(D2_PIO_KEYS4_BASE);    
// }

// int switches_pressed(void)
// {
//   return IORD_ALTERA_AVALON_PIO_DATA(DE2_PIO_TOGGLES18_BASE);    
// }

/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */

  return delay;
}

static int b2sLUT[] = {0x40, //0
  0x79, //1
  0x24, //2
  0x30, //3
  0x19, //4
  0x12, //5
  0x02, //6
  0x78, //7
  0x00, //8
  0x18, //9
  0x3F, //-
};

/*
 * convert int to seven segment display format
 */
int int2seven(int inval){
  return b2sLUT[inval];
}

/*
 * output current velocity on the seven segement display
 */
void show_velocity_on_sevenseg(INT8S velocity){
  int tmp = velocity;
  int out;
  INT8U out_high = 0;
  INT8U out_low = 0;
  INT8U out_sign = 0;

  if(velocity < 0){
    out_sign = int2seven(10);
    tmp *= -1;
  }else{
    out_sign = int2seven(0);
  }

  out_high = int2seven(tmp / 10);
  out_low = int2seven(tmp - (tmp/10) * 10);

  out = int2seven(0) << 21 |
    out_sign << 14 |
    out_high << 7  |
    out_low;
  IOWR_ALTERA_AVALON_PIO_DATA(DE2_PIO_HEX_LOW28_BASE,out);
}

/*
 * shows the target velocity on the seven segment display (HEX5, HEX4)
 * when the cruise control is activated (0 otherwise)
 */
void show_target_velocity(INT8U target_vel)
{
	//Implement and call in control task
}


void show_position(INT16U position)
{
	
	/*
 * indicates the position of the vehicle on the track with the four leftmost red LEDs
 * LEDR17: [0m, 400m)
 * LEDR16: [400m, 800m)
 * LEDR15: [800m, 1200m)
 * LEDR14: [1200m, 1600m)
 * LEDR13: [1600m, 2000m)
 * LEDR12: [2000m, 2400m]
 */

	if (0 <= position && position < 400)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_17);
  else if (400 <= position && position < 800)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_16);
  else if (800 <= position && position < 1200)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_15);
  else if (1200 <= position && position < 1600)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_14);
  else if (1600 <= position && position < 2000)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_13);
  else if (2000 <= position && position < 2400)
    IOWR_ALTERA_AVALON_PIO_SET_BITS(DE2_PIO_REDLED18_BASE,LED_RED_12);
 
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

/*
 * The task 'VehicleTask' is the model of the vehicle being simulated. It updates variables like
 * acceleration and velocity based on the input given to the model.
 * 
 * The car model is equivalent to moving mass with linear resistances acting upon it.
 * Therefore, if left one, it will stably stop as the velocity converges to zero on a flat surface.
 * You can prove that easily via basic LTI systems methods.
 */
void VehicleTask(void* pdata)
{ 
  // constants that should not be modified
  const unsigned int wind_factor = 1;
  const unsigned int brake_factor = 4;
  const unsigned int gravity_factor = 2;
  // variables relevant to the model and its simulation on top of the RTOS
  INT8U err,semerror;  
  void* msg;
  INT8U* throttle; 
  INT16S acceleration;  
  INT16U position = 0; 
  INT16S velocity = 0; 
  enum active brake_pedal = off;
  enum active engine = off;

  printf("Vehicle task created!\n");

  while(1)
  {
    err = OSMboxPostOpt(Mbox_Velocity, (void *) &velocity, OS_POST_OPT_BROADCAST); //Change this to broadcast

    //OSTimeDlyHMSM(0,0,0,VEHICLE_PERIOD); 
    OSTmrStart(vehicleTaskTimer, &err);
    OSSemPend(pdata,0,&semerror);

    /* Non-blocking read of mailbox: 
       - message in mailbox: update throttle
       - no message:         use old throttle
       */
    msg = OSMboxPend(Mbox_Throttle, 1, &err); 
    if (err == OS_NO_ERR)
    { 
      printf("Yayy I got throttle from the mb\n"); //Stopped here yesterday
      throttle = (INT8U*) msg;
    }
    /* Same for the brake signal that bypass the control law */
    msg = OSMboxPend(Mbox_Brake, 1, &err); 
    if (err == OS_NO_ERR) 
      brake_pedal = (enum active) msg;
    /* Same for the engine signal that bypass the control law */
    msg = OSMboxPend(Mbox_Engine, 1, &err); 
    if (err == OS_NO_ERR) 
      engine = (enum active) msg;


    // vehichle cannot effort more than 80 units of throttle
    if (*throttle > 80) { *throttle = 80;}

    // brakes + wind
    if (brake_pedal == off)
    {
      // wind resistance

      acceleration = - wind_factor*velocity;
      // actuate with engines
      if (engine == on)
        acceleration += (*throttle);

      // gravity effects
      if (400 <= position && position < 800)
        acceleration -= gravity_factor; // traveling uphill
      else if (800 <= position && position < 1200)
        acceleration -= 2*gravity_factor; // traveling steep uphill
      else if (1600 <= position && position < 2000)
        acceleration += 2*gravity_factor; //traveling downhill
      else if (2000 <= position)
        acceleration += gravity_factor; // traveling steep downhill
    }
    // if the engine and the brakes are activated at the same time,
    // we assume that the brake dynamics dominates, so both cases fall
    // here.
    else 
      acceleration = - brake_factor*velocity;

    printf("Position: %d m\n", position);
    printf("Velocity: %d m/s\n", velocity);
    printf("Accell: %d m/s2\n", acceleration);
    printf("Throttle: %d V\n", *throttle);

    position = position + velocity * VEHICLE_PERIOD / 1000;
    velocity = velocity  + acceleration * VEHICLE_PERIOD / 1000.0;
    // reset the position to the beginning of the track
    if(position > 2400)
      position = 0;
    printf("I finally got here");
  	show_position(position);
    show_velocity_on_sevenseg((INT8S) velocity);
  }
} 

/*
 * The task 'ControlTask' is the main task of the application. It reacts
 * on sensors and generates responses.
 */

void ControlTask(void* pdata)
{
  INT8U err,semerror;
  INT16U regular_throttle = 20; /* Value between 0 and 80, which is interpreted as between 0.0V and 8.0V */
  INT16U cruiseControl_throttle = 20;
  void* msg;
  INT8U cruiseCount=0;
  INT16S current_velocity;
  INT16S* current_velocity_ptr;
  INT16S set_velocity;
  INT16S correction;


  enum active gas_pedal = off;
  enum active top_gear = off;
  enum active cruise_control = off;
  enum active brake_pedal=off;
  enum active engine_state=off;


  printf("Control Task created!\n");

  while(1)
  {
    msg = OSMboxPend(Mbox_Velocity, 0, &err);
    if (err == OS_NO_ERR) 
    { 
      printf("Yayy I got velocity from the mb\n");
      current_velocity_ptr = (INT16S*) msg;
      current_velocity= *current_velocity_ptr;
    }
    //Here we are getting the states from the IO
    msg=OSMboxPend(throttle_flag,1, &err);
    if (err == OS_NO_ERR)
    {
     printf("Yayy I got gas_pedal from the mb\n"); 
      gas_pedal = (enum active) msg;
    }
  	// msg=OSMboxPend(topgear_flag,1, &err);
   //  if (err == OS_NO_ERR) 
   //   top_gear = (enum active) msg;
  	msg=OSMboxPend(cruise_flag,1, &err);
    if (err == OS_NO_ERR) 
    {
     printf("Yayy I got cruiseControl from the mb\n"); 
      cruise_control = (enum active) msg;
    }
     
  	msg=OSMboxPend(Mbox_Brake,1, &err);
    if (err == OS_NO_ERR) 
     brake_pedal = (enum active) msg;
  	msg=OSMboxPend(Mbox_Engine,1,&err);
    if (err == OS_NO_ERR) 
     engine_state = (enum active) msg;

  	if(cruise_control==on && cruiseCount==0)
  	{
  		set_velocity=current_velocity;
  		cruiseControl_throttle= regular_throttle;
  		cruiseCount++;
  	}

    //Revisit this as IO are now in control FLAG
  	if (cruise_control == on)

  	{	
  		printf("Cruise Control Active!\n");
  		//Cruise Control Algo
  		correction=-1*(current_velocity-set_velocity);
  		if(correction>0)
  		{
  			cruiseControl_throttle ++;
  			if (cruiseControl_throttle>80) cruiseControl_throttle=80;
 		  }  
 		  else
		  {
 			  cruiseControl_throttle-=5;
 			  if (cruiseControl_throttle<0) cruiseControl_throttle=0;
 		  }


  		err = OSMboxPost(Mbox_Throttle, (void *) &cruiseControl_throttle);
  		show_target_velocity((INT8S)set_velocity);
  	}
  	else 
      //(gas_pedal==on && brake_pedal==off)
  	{
  		cruiseCount=0;
  		regular_throttle++;
  		if (regular_throttle>80)
  			regular_throttle=80;
  		err = OSMboxPost(Mbox_Throttle, (void *) &regular_throttle);
  		printf("fuck off no cruise control 4 u\n");
  		show_target_velocity(0);
  	}
  	// else
  	// {	cruiseCount=0;
  	// 	regular_throttle--;
  	// 	if (regular_throttle<0)
  	// 		regular_throttle=0;
  	// 	show_target_velocity(0);
  	// }





    // Here you can use whatever technique or algorithm that you prefer to control
    // the velocity via the throttle. There are no right and wrong answer to this controller, so
    // be free to use anything that is able to maintain the cruise working properly. You are also
    // allowed to store more than one sample of the velocity. For instance, you could define
    //
    // INT16S previous_vel;
    // INT16S pre_previous_vel;
    // ...
    //
    // If your control algorithm/technique needs them in order to function. 

    //OSTimeDlyHMSM(0,0,0, CONTROL_PERIOD);
    OSTmrStart(controlTaskTimer, &err);
    OSSemPend(pdata,0,&semerror);
  }
}

/* 
 * The task 'StartTask' creates all other tasks kernel objects and
 * deletes itself afterwards.
 */ 

void StartTask(void* pdata)
{
  INT8U err;
  void* context;

  static alt_alarm alarm;     /* Is needed for timer ISR function */

  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000; 
  printf("delay in ticks %d\n", delay);

  /* 
   * Create Hardware Timer with a period of 'delay' 
   */
  if (alt_alarm_start (&alarm,
        delay,
        alarm_handler,
        context) < 0)
  {
    printf("No system clock available!n");
  }

  /* 
   * Create and start Software Timer 
   */
  switchTaskSem  =OSSemCreate(0);
  buttonTaskSem  =OSSemCreate(0);
  vehicleTaskSem =OSSemCreate(0);
  controlTaskSem =OSSemCreate(0);


  switchTaskTimer=OSTmrCreate(
              10, //delay
              SWITCH_PERIOD,//period
              OS_TMR_OPT_PERIODIC,
              switchTimerCallback, //get back to this after making semaphores
              switchTaskSem, //get back to this after making semaphores
              "switchTaskTimer",
              &err);
  buttonTaskTimer=OSTmrCreate(
              10, //delay
              BUTTON_PERIOD,//period
              OS_TMR_OPT_PERIODIC,
              buttonTimerCallback, //get back to this after making semaphores
              buttonTaskSem, //get back to this after making semaphores
              "buttonTaskTimer",
              &err);
  vehicleTaskTimer=OSTmrCreate(
              10, //delay
              VEHICLE_PERIOD,//period
              OS_TMR_OPT_PERIODIC,
              vehicleTimerCallback, //get back to this after making semaphores
              vehicleTaskSem, //get back to this after making semaphores
              "vehicleTaskTimer",
              &err);
  controlTaskTimer=OSTmrCreate(
              10, //delay
              CONTROL_PERIOD,//period
              OS_TMR_OPT_PERIODIC,
              controlTimerCallback, //get back to this after making semaphores
              controlTaskSem, //get back to this after making semaphores
              "controlTaskTimer",
              &err);
  /*
   * Creation of Kernel Objects
   */

  // Mailboxes
  Mbox_Throttle = OSMboxCreate((void*) 0); /* Empty Mailbox - Throttle */
  Mbox_Velocity = OSMboxCreate((void*) 0); /* Empty Mailbox - Velocity */
  
  Mbox_Brake    = OSMboxCreate((void*) 1); //The 1 is /* Empty Mailbox - Brake*/
  Mbox_Engine   = OSMboxCreate((void*) 1); /* Empty Mailbox - Engine */
  throttle_flag = OSMboxCreate((void*) 0);
  cruise_flag   = OSMboxCreate((void*) 0);
  topgear_flag  = OSMboxCreate((void*) 0);

  /*
   * Create statistics task
   */

  OSStatInit();

  /* 
   * Creating Tasks in the system 
   */


  err = OSTaskCreateExt(
      ControlTask, // Pointer to task code
      controlTaskSem,        // Pointer to argument that is
      // passed to task
      &ControlTask_Stack[TASK_STACKSIZE-1], // Pointer to top
      // of task stack
      CONTROLTASK_PRIO,
      CONTROLTASK_PRIO,
      (void *)&ControlTask_Stack[0],
      TASK_STACKSIZE,
      (void *) 0,
      OS_TASK_OPT_STK_CHK);

  err = OSTaskCreateExt(
      VehicleTask, // Pointer to task code
      vehicleTaskSem,        // Pointer to argument that is
      // passed to task
      &VehicleTask_Stack[TASK_STACKSIZE-1], // Pointer to top
      // of task stack
      VEHICLETASK_PRIO,
      VEHICLETASK_PRIO,
      (void *)&VehicleTask_Stack[0],
      TASK_STACKSIZE,
      (void *) 0,
      OS_TASK_OPT_STK_CHK);

  err = OSTaskCreateExt(
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

    err = OSTaskCreateExt(
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


  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}

/*
 *
 * The function 'main' creates only a single task 'StartTask' and starts
 * the OS. All other tasks are started from the task 'StartTask'.
 *
 */

int main(void) {

  printf("Lab: Cruise Control\n");

  OSTaskCreateExt(
      StartTask, // Pointer to task code
      NULL,      // Pointer to argument that is
      // passed to task
      (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
      // of task stack 
      STARTTASK_PRIO,
      STARTTASK_PRIO,
      (void *)&StartTask_Stack[0],
      TASK_STACKSIZE,
      (void *) 0,  
      OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSStart();

  return 0;
}
