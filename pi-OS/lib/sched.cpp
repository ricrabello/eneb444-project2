/*
 * sched.c
 *
 *  Created on: Feb 8, 2017
 *      Author: wangfakai
 */


#include <circle/sched.h>
#include "../../pi-OS/include/circle/timer.h"
#include "../../pi-OS/include/circle/util.h"
#include <circle/taskswitch.h>
#include <circle/logger.h>

//#include <circle/timer.h>
#define TASKKERNELID 0
#define TASKKERNELNUM 0
#define TASKSCHEDID 1
#define TASKSCHEDNUM 1
#define NULL 0

const char FromScheduler[] = "Scheduler";


//CAAUSION: don't change variable declaration, no matter location or feature
static int taskIDcounter = 0;
static Tscheduler scheduler;
static sysCall systemCall;

void InitializeScheduler()
{
	int i;
	taskIDcounter = 100;
	scheduler.pCurrentTask = 0;
	scheduler.pLastTask = 0;
	scheduler.lastTaskSwitchReason = KERNELSWITCHING;
	for (i = 0 ; i< MAXTASK; i++){
		scheduler.taskQueue[i] = 0;
	}
	systemCall.yield = &yield;
	systemCall.sleep = (void (*)(void*, unsigned int))(&sleep);
	systemCall.exit = &exit;
}

//systemCall,scheduler must be accessed by getSysCallPointer.
//Although they can be accessed directly from other source files, they are not the same
sysCall * getSysCallPointer()
{
	return &systemCall;
}

Tscheduler * getScheduler()
{
	return &scheduler;
}

void startScheduler(Task * pKernelTask, int ALGORITHM)
{
	int taskNum = MAXTASK;
	Task * pSchedTask = scheduler.taskQueue[TASKSCHEDNUM];
	while( 1 )
	{
		scheduler.pCurrentTask = pSchedTask;
		taskNum = schedulePriority();
		if (MAXTASK == taskNum){
			pSchedTask->pSysCall->print("SCHEDULER:: no READY task in queue now, scheduler quite");
			scheduler.SchedulerState = NOREADYTASK;
			break;
		}
		pSchedTask->pSysCall->print("SCHEDULER:: scheduler going to yield to USER task now  -------");
		kernelYieldByNum(taskNum);
		pSchedTask->pSysCall->print("SCHEDULER:: someone return control to scheduler         ++++++");
	}
	scheduler.SchedulerState = NOREADYTASK;
	pSchedTask->pSysCall->print("SCHEDULER:: going out of scheduler");
}


/*scheduling task: TASK ID=1; TASK NUM = 1;
 * This Run function is for scheduling task. But it will not run.
 * Its task num is 1, and its entry will be override as returning point in kernelYield.
 * We need this "task", currently only for the purpose of storing returning address.
 * It's not a runnable task now, the first will begin after scheduling task
 */

void schedulerRun(Task * taskSched)
{
 int ALGORITHM = SCHED_FIFO;
 int taskNum = GetNextTask();
 static int counter = 2;
 taskSched->pSysCall->print("Now we are in the schedulerRun");
 scheduler.pCurrentTask = taskSched;
 CLogger::Get ()->Write (FromScheduler, LogPanic, "schedulerRusdgsdfg    dfg ");

 //switchToTask(taskNum);//This will switch to Application in the task queue
 kernelYield();
 taskSched->pSysCall->print("someone  schedulerRun");

}


int schedulePriority()
{
	int taskNum = 0;
	int i = 0;
	int highest_priority = 100;
	int heighest_priority_task_num = 100;
	//Task * heighestPriorityTask;
	for(i=2; i< MAXTASK;i++){

		if ( 0 != scheduler.taskQueue[i]  && TaskStateReady == scheduler.taskQueue[i]->State){

			if( scheduler.taskQueue[i]->priority < highest_priority){
				heighest_priority_task_num = i;
				//heighestPriorityTask = scheduler.taskQueue[i];
				highest_priority = scheduler.taskQueue[i]->priority;
			}

		}
	}
	taskNum = heighest_priority_task_num;
	return taskNum;
}

int scheduleFIFO()
{
	int taskNum = MAXTASK;
	//At the beginning, there may be not pLastTask
	if (NULL == scheduler.pLastTask){
		return GetNumOfNextReadyTask();
	}
	return GetNumOfNextReadyTask();

}

int  getTaskNumByTaskPointer(Task *pTask)
{
	int i;
	if(0 == pTask)
	{
		return MAXTASK;
	}
	for (i = 0 ; i< MAXTASK; i++){
		//find the current task, then flag
		if(scheduler.pCurrentTask == scheduler.taskQueue[i]){
			return i;
		}
	}
	return MAXTASK;
}

int genTaskID()
{
	return taskIDcounter ++;
}


void AddTask (Task *pTask)
{
	int i;
	for (i = 0 ; i< MAXTASK; i++){
		if(0 == scheduler.taskQueue[i]){
			scheduler.taskQueue[i] = pTask;
			break;
		}
	}

}
void BlockTask (Task **ppTask);
void WakeTask (Task **ppTask);		// can be called from interrupt context

void RemoveTaskFromQueue (Task *pTask)
{
	int i;

	for (i = 0 ; i< MAXTASK; i++){
		if(pTask == scheduler.taskQueue[i]){
			scheduler.taskQueue[i] = 0;
			break;
		}
	}
}
/*
 * called by scheduler only. Select the next task after current task in the queue
 * kernel task doesn't participate the selecting procedure
 */
unsigned GetNextTask (void)
{
	int i;
	boolean posFlag;
	posFlag = FALSE;
	for (i = 0 ; i< MAXTASK; i++){
		//select next task after current task
		if ( TRUE == posFlag && 0 != scheduler.taskQueue[i]){
			return i;
		}
		//find the current task, then flag
		if(scheduler.pCurrentTask == scheduler.taskQueue[i]){
			posFlag = TRUE;
			continue;
		}
	}
	//if no following task is found, then loop around
	for (i = KERNELTASKNUM + 2 ; i< MAXTASK; i++){
		if ( TRUE == posFlag && 0 != scheduler.taskQueue[i]){
			return i;
		}
	}
	//if return 0, then no user task in the queue (only the kernel task)
	return 0;
}


unsigned GetNumOfNextReadyTask (void)
{
	int i;
	boolean posFlag;
	posFlag = FALSE;
	for (i = 0 ; i< MAXTASK; i++){
		//select next task after current task
		if ( TRUE == posFlag && 0 != scheduler.taskQueue[i] && TaskStateReady == scheduler.taskQueue[i]->State){
			return i;
		}
		//find the current task, then flag
		if(scheduler.pCurrentTask == scheduler.taskQueue[i]){
			posFlag = TRUE;
			continue;
		}
	}
	//if no following task is found, then loop around
	for (i = KERNELTASKNUM + 2 ; i< MAXTASK; i++){
		if ( TRUE == posFlag && 0 != scheduler.taskQueue[i] && TaskStateReady == scheduler.taskQueue[i]->State){
			return i;
		}
	}
	//if return MAXTASK, then no user task in the queue (only the kernel task)
	return MAXTASK;
}


/*
 * Application calls yield, record switch reason, then return control to kernel.
 * (scheduler is part of the kernel, and currently kernel stops at scheduler, saved in scheduler.taskQueue[KERNELTASKNUM])
 * yield means Application reaches the end of code normally
 */
void yield (void)
{
	Task *pNext = scheduler.taskQueue[TASKSCHEDNUM];
	TTaskRegisters *pNewRegs = &(pNext->Regs);

	TTaskRegisters *pOldRegs = &(scheduler.pCurrentTask->Regs);
	RemoveTaskFromQueue (scheduler.pCurrentTask);

	scheduler.pLastTask = scheduler.pCurrentTask;
	scheduler.lastTaskSwitchReason = APPENDS;
	scheduler.pCurrentTask = pNext;

	TaskSwitch (pOldRegs, pNewRegs);
}


void kernelYield (void)
{
	int nextNum = GetNextTask ();
	Task *pNext = scheduler.taskQueue[nextNum];
	TTaskRegisters *pNewRegs = &(pNext->Regs);

	TTaskRegisters *pOldRegs = &(scheduler.pCurrentTask->Regs);
	scheduler.pLastTask = scheduler.pCurrentTask;
	scheduler.pCurrentTask = pNext;
	TaskSwitch (pOldRegs, pNewRegs);
}


void kernelYieldByNum (int nextNum)
{
	Task *pNext = scheduler.taskQueue[nextNum];
	TTaskRegisters *pNewRegs = &(pNext->Regs);

	TTaskRegisters *pOldRegs = &(scheduler.pCurrentTask->Regs);
	scheduler.pLastTask = scheduler.pCurrentTask;
	scheduler.pCurrentTask = pNext;
	TaskSwitch (pOldRegs, pNewRegs);
}


/*
 * Application calls sleep, record switch reason, then return control to kernel.
 * (scheduler is part of the kernel, and currently kernel stops at scheduler, saved in scheduler.taskQueue[KERNELTASKNUM])
 * sleep means Application doesn't finish, and it want to run at some time later
 */
void sleepYield (void)
{
	// TODO: project 2



}


/*
 * Application calls exit, record switch reason, then return control to kernel.
 * (scheduler is part of the kernel, and currently kernel stops at scheduler, saved in scheduler.taskQueue[KERNELTASKNUM])
 * The task will also be removed from task queue, and print some debugging information
 */
void exit()
{

 Task *pNext = scheduler.taskQueue[KERNELTASKNUM];
 //RemoveTaskFromQueue(scheduler.pCurrentTask);
 TTaskRegisters *pOldRegs = &(scheduler.pCurrentTask->Regs);
 TTaskRegisters *pNewRegs = &(pNext->Regs);
 scheduler.pLastTask = scheduler.pCurrentTask; // scheduler need to know last task to continuing scheduling
 scheduler.lastTaskSwitchReason = APPEXIT;
 scheduler.pCurrentTask = pNext;
 TaskSwitch (pOldRegs, pNewRegs); //which line will be executed next? Why?

}

void sleep (Task *pTask, unsigned nSeconds)
{
 // be sure the clock does not run over taken as signed int
 const unsigned nSleepMax = 1800; // normally 2147 but to be sure
 pTask->pSysCall->print("sleep is called ");
 pTask->State = TaskStateSleeping;
 scheduler.lastTaskSwitchReason = APPSLEEPING;

 if (nSeconds > nSleepMax)
 {
  msSleep(pTask, nSleepMax);
 }
 else{
  // addKernelTimer is in unit of 10ms
  msSleep(pTask, 100* nSeconds);
 }
}

void msSleep (Task *pTask, unsigned nMilliSeconds)
{
	pTask->pSysCall->print("msSleep is called");

	// TODO: project 2
	// IMPORTANT: addKernelTimer is in unit of 10ms


}


//This function is used by SVC second level handler
//yieldPrepare() is called in the first stage of exit
void yieldPrepare(){
	//TODO
}

//This function is used by SVC second level handler
//sleepYieldPrepare() is called in the first stage of sleep
void sleepYieldPrepare(unsigned seconds, void *pParam){
	//TODO

}


int getCurrentTaskPointer(){
	return (int)scheduler.pCurrentTask;
}

//This function is used by SVC second level handler
//getKernelRegPtr() is called in the second stage of exit & sleep
int getKernelRegPtr(){
	return (u32)(&scheduler.taskQueue[TASKSCHEDNUM]->Regs);
}


void sleepTimerHandler (unsigned hTimer, void *pParam, void *pContext)
{

	Task *pTask = (Task *) pParam;
	pTask->pSysCall->print("timer went off");
	pTask->pSysCall->print("sleepTimerHandler is called ");

	// TODO: project 2


}


