//
// main.c
//
// pi-OS C++ bare metal environment for Raspberry Pi


#include "kernel.h"
#include "circle/startup.h"

#include "circle/sched.h"
#include "circle/task.h"

static CKernel Kernel;

void kernelPrint(const char * pMessage){

	Kernel.write_log(pMessage);
}


void addKernelTimer(unsigned nDelay,
		TKernelTimerHandler *pHandler,
		void *pParam){

	Kernel.addKernelTimer(nDelay, pHandler,  pParam);
}


void kernelPrintV(const char * pMessage,...){


	va_list var;
	va_start (var, pMessage);

	Kernel.write_logV(pMessage, var);

	va_end (var);
	//Kernel.write_log(message);
}



//Sleep timer handler function
void sleepTimerHandler(unsigned hTimer, void *pParam, void *pContext)
{
	Task *pTask = (Task *)pParam;
	pTask->pSysCall->print("sleepTimerHandler is called ");
	

}

/*KERNEL task: TASK ID=0; TASK NUM = 0;
 * 	//The kernel routine: all the kernel codes starts here
 * 	Kernel task doesn't participate scheduling: it's not runnable
 * 	main will call kernelTaskRun after initialize all the tasks, including the kernel task itself(kernel task lies in first place)
 * 	kernelTaskRun serves as the most core function in the OS. Its task ID is 0, and its running span is 0(never ends)
 * 	-------It determines how scheduling system works(By scheduling algorithm)
 * 	-------It determines many other things ... ...
 *
 *
 */

void kernelTaskRun(Task * pTask)
{
	Tscheduler * scheduler = getScheduler();
	pTask->pSysCall->print("Some one calls the kernel, or the queue just starts over");

	while (1)
	{
		if (NEEDTOSCHED == scheduler->SchedulerState ){
			startScheduler(pTask, SCHED_FIFO);

			//TODO PROJECT 2
		 	//@ricrabello implementation begins
			pTask->pSysCall->print("The kernel task will sleep for 3 seconds");
			pTask->pSysCall->sleep(pTask, 3000);
			pTask->pSysCall->msSleep(pTask, 3000000);
			pTask->pSysCall->print("The kernel task wakes up");
			pTask->pSysCall->print("The kernel task is running");
			//@ricrabello implementation ends
		}
	}
}

#define TASKKERNELID 0
#define TASKSCHEDID 1
#define TASKINTERUPTID 2
#define TASKTIMER0ID 3
#define TASKTIMER1ID 4

int main (void)
{
	if (!Kernel.Initialize ())
	{
		halt ();
		return EXIT_HALT;
	}
	kernelPrint("Welcome to ENEE444, Operating System for Embedded Systems");

	/*
	 * ID	| NUM | Task		|runnable 	|  Description
	 * ------------------------------------------------------------------------------
	 * 0	| 0   |kernelTask	|No			|
	 * 1  	| 1   |taskSched	|No			|
	 * 100  | 2   |task1		|Yes		|
	 * 101  | 3   |task2		|Yes		|
	 * 102  | 4   |task3		|Yes		|
	 * 103  | 5   |task4		|Yes		|
	 * 104  | 6   |task5		|Yes		|
	 *
	 *
	 */
	Task kernelTask;
	Task taskSched;
	InitializeScheduler();
	sysCall * pSystemCall = getSysCallPointer();
	pSystemCall->print = & kernelPrint;
	pSystemCall->printV = & kernelPrintV;
	pSystemCall->addKernelTimer = & addKernelTimer;

	Task task1, task2, task3, task4;
	initializeTask(&kernelTask, TASKKERNELID, &TaskEntry, &kernelTaskRun, pSystemCall); // make sure the first task in scheduling queue is kernelTask
	initializeTask(&taskSched, TASKSCHEDID, &TaskEntry, &schedulerRun, pSystemCall);
	AddTask(&kernelTask);
	AddTask(&taskSched);//The first two tasks must be kernel and scheduler


	initializeTask(&task1, (u32)genTaskID(), &TaskEntry, &task1_run, pSystemCall);
	initializeTask(&task2, (u32)genTaskID(), &TaskEntry, &task2_run, pSystemCall);

	kernelTask.priority = 1;
	taskSched.priority = 10;
	task1.priority = 80;
	task2.priority = 50;

	AddTask(&task1);
	AddTask(&task2);

	int i = 0;
	Tscheduler *scheduler = getScheduler();
	scheduler->SchedulerState = NEEDTOSCHED;

	Kernel.write_log("Now print the task queue");
	for(i=0;i<MAXTASK;i++){
		if( 0 != scheduler->taskQueue[i])
		{
			Kernel.write_log("Task_ID=%d, queue_NUM = %d, task=%d, priority=%d"  \
					, scheduler->taskQueue[i]->ID, i,scheduler->taskQueue[i], scheduler->taskQueue[i]->priority);
		}
	}


	// run scheduler task
	kernelTaskRun( &kernelTask);

	//TShutdownMode ShutdownMode = ShutdownHalt;
	TShutdownMode ShutdownMode = Kernel.Run ();

	switch (ShutdownMode)
	{
	case ShutdownReboot:
		reboot ();
		return EXIT_REBOOT;

	case ShutdownHalt:
	default:
		Kernel.write_log("Going to halt");
		halt ();
		Kernel.write_log("Never goes here");
		return EXIT_HALT;
	}
}
