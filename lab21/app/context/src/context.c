// File: TwoTasks.c 

#include <stdio.h>
#include "includes.h"
#include <string.h>
#include "sys/alt_timestamp.h"
#include "alt_types.h"
#define DEBUG 0

/* Definition of Task Stacks */
/* Stack grows from HIGH to LOW memory */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    stat_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK1_PRIORITY      6  // highest priority
#define TASK2_PRIORITY      7
#define TASK_STAT_PRIORITY 12  // lowest priority 
alt_u32 time1,time2;


void printStackSize(char* name, INT8U prio) 
{
  INT8U err;
  OS_STK_DATA stk_data;
    
  err = OSTaskStkChk(prio, &stk_data);
  if (err == OS_NO_ERR) {
    if (DEBUG == 1)
      printf("%s (priority %d) - Used: %d; Free: %d\n", 
	     name, prio, stk_data.OSUsed, stk_data.OSFree);
  }
  else
    {
      if (DEBUG == 1)
	printf("Stack Check Error!\n");    
    }
}

/* Prints a message and sleeps for given time interval */
void task1(void* pdata)
{
  while (1)
    { 
      char text1[] = "TASK 0 is STATE 0\n";
      int i;
      INT8U err;
      
      for (i = 0; i < strlen(text1); i++)
		    putchar(text1[i]);
      time1=alt_timestamp();
	    OSSemPend(pdata,0,&err);
    
      printf("TASK 0 is STATE 1\n");
     
    }
}

/* Prints a message and sleeps for given time interval */
void task2(void* pdata)
{
  while (1)
    { 
      time2=alt_timestamp();
      printf("time differnce in ticks is %u\n",(unsigned int)(time2-time1));
      char text2[] = "TASK 1 is STATE 0\n";
      int i;

      //

      for (i = 0; i < strlen(text2); i++)
		    putchar(text2[i]);
      printf("TASK 1 is STATE 1\n");

      OSSemPost(pdata);
    
    }
}

/* Printing Statistics */
void statisticTask(void* pdata)
{
  while(1)
    {
      printStackSize("Task1", TASK1_PRIORITY);
      printStackSize("Task2", TASK2_PRIORITY);
      printStackSize("StatisticTask", TASK_STAT_PRIORITY);
    }
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
  printf("Lab 3 - Two Tasks\n");
  if (alt_timestamp_start() <0)
  {
    printf("no timestamp device available\n");
  }

  OS_EVENT *semaphore_variable;
  semaphore_variable=OSSemCreate(0);
  OSTaskCreateExt
    ( task1,                        // Pointer to task code
      semaphore_variable,                         // Pointer to argument passed to task
      &task1_stk[TASK_STACKSIZE-1], // Pointer to top of task stack
      TASK1_PRIORITY,               // Desired Task priority
      TASK1_PRIORITY,               // Task ID
      &task1_stk[0],                // Pointer to bottom of task stack
      TASK_STACKSIZE,               // Stacksize
      NULL,                         // Pointer to user supplied memory (not needed)
      OS_TASK_OPT_STK_CHK |         // Stack Checking enabled 
      OS_TASK_OPT_STK_CLR           // Stack Cleared                                 
      );
	   
  OSTaskCreateExt
    ( task2,                        // Pointer to task code
      semaphore_variable,                         // Pointer to argument passed to task
      &task2_stk[TASK_STACKSIZE-1], // Pointer to top of task stack
      TASK2_PRIORITY,               // Desired Task priority
      TASK2_PRIORITY,               // Task ID
      &task2_stk[0],                // Pointer to bottom of task stack
      TASK_STACKSIZE,               // Stacksize
      NULL,                         // Pointer to user supplied memory (not needed)
      OS_TASK_OPT_STK_CHK |         // Stack Checking enabled 
      OS_TASK_OPT_STK_CLR           // Stack Cleared                       
      );  

  if (DEBUG == 1)
    {
      OSTaskCreateExt
	( statisticTask,                // Pointer to task code
	  NULL,                         // Pointer to argument passed to task
	  &stat_stk[TASK_STACKSIZE-1],  // Pointer to top of task stack
	  TASK_STAT_PRIORITY,           // Desired Task priority
	  TASK_STAT_PRIORITY,           // Task ID
	  &stat_stk[0],                 // Pointer to bottom of task stack
	  TASK_STACKSIZE,               // Stacksize
	  NULL,                         // Pointer to user supplied memory (not needed)
	  OS_TASK_OPT_STK_CHK |         // Stack Checking enabled 
	  OS_TASK_OPT_STK_CLR           // Stack Cleared                              
	  );
    }  

  OSStart();
  return 0;
}
