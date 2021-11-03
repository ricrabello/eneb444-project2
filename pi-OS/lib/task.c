/*
 * task.c
 *
 *  Created on: Feb 8, 2017
 *      Author: wangfakai
 */


#include <circle/task.h>
#include <circle/sched.h>
#include <circle/sysconfig.h>
#include <circle/types.h>
//#include <circle/task_def.h>

void initializeTask(Task * pTask, unsigned ID, void (* enter)(Task * task), void (* run)(Task * task), void * sysCall)
{
	pTask->ID = ID;
	pTask->pSelf = pTask;
	pTask->pStack = (Task *) malloc(STACKSIZE);
	pTask->StackSize = STACKSIZE;
	InitializeRegs(pTask);

	pTask->taks_entry = enter;
	pTask->Run = run;
	pTask->pSysCall = sysCall;
	pTask->State = TaskStateReady;
}


void InitializeRegs (Task * pTask)
{
	//TaskRegisters * pRegs = &(pTask->Regs);
	memset (&(pTask->Regs), 0, sizeof (pTask->Regs));

	pTask->Regs.r0 = (u32) pTask;		// pParam for TaskEntry()

	pTask->Regs.sp = (u32) (pTask->pStack) + pTask->StackSize;

	pTask->Regs.lr = (u32) &TaskEntry;
}

void TaskEntry (Task * pTask)
{

	pTask->pSysCall->print("Task is entering now");
	pTask->Run (pTask);

	pTask->State = TaskStateTerminated;
	pTask->pSysCall->printV("Task:ID=%d is going to terminate normally", pTask->ID);
	pTask->pSysCall->yield();
	//pTask->pSysCall->print("This should not appear on the screen");
}



void task1_run(Task * pTask)
{
	pTask->pSysCall->print("--- task1: Hello. ----");
	pTask->pSysCall->sleep(pTask, 2);
	pTask->pSysCall->print("--- task1: Hello. -----");
	pTask->pSysCall->sleep(pTask, 6);
	pTask->pSysCall->print("--- task1: GoodBye.----");
}


void task2_run(Task * pTask)
{
	pTask->pSysCall->print("--- task2: Hello.------");
	pTask->pSysCall->sleep(pTask, 5);
	pTask->pSysCall->print("--- task2: GoodBye.----");



	pTask->pSysCall->print("Task2 begins to run . ------");
	pTask->pSysCall->sleep(pTask, 3);
	pTask->pSysCall->print("Task2 begins to run . ------");
	pTask->pSysCall->sleep(pTask, 2);
	pTask->pSysCall->print("Task2 begins to run . ------");
	pTask->pSysCall->sleep(pTask, 3);
	pTask->pSysCall->print("Task2 has been done.-----");
}


void task3_run(Task * pTask)
{
	pTask->pSysCall->print("Now we are in task3 .*****");
	//pTask->pSysCall->sleep(pTask, 10);
	pTask->pSysCall->print("task3 is working .");
	pTask->pSysCall->print("Task3 has been done.*****");
}


void taskMatrix_run(Task * pTask)
{
	int value = 10;
	pTask->pSysCall->printV("Hello from Matrix Task");
	int result=0;
	int firstMatrix[3][4] = {
			{6, 7, 8, 9},
			{10, 11, 12, 13},
			{14, 15, 16, 17}
	};
	int secondMatrix[4][3] = {
			{61, 57, 8},
			{10, 9, 13},
			{14, 15, 17},
			{101, 0, 1}
	};
	int thirdMatrix[3][3];
	int i, j, k;

	// Initializing elements of matrix mult to 0.
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			thirdMatrix[i][j] = 0;
		}
	}

	// Multiplying matrix firstMatrix and secondMatrix and storing in array.
	for(i = 0; i < 3; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			for(k=0; k<4; ++k)
			{
				thirdMatrix[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
			}
		}
	}
	pTask->pSysCall->printV("Now print the third matrix");
	for (i=0; i<3; i++){
		pTask->pSysCall->printV("\t%d\t%d\t%d\t", thirdMatrix[i][0],thirdMatrix[i][1],thirdMatrix[i][2]);
	}
}


void task4_run(Task * pTask)
{
	pTask->pSysCall->print("From task4   1111");

}





void task5_run(Task * pTask)
{
	//print on screen, "This is hello from task %d, currently I'm running. I am printing one character every 1 ms"
	//call delay(20us), not using timer or system call to sleep; loop, every time print one letter
	//different task have different string to print
	//when times interrupts, timer handler save current CPSD; kernel setup next task to run
	pTask->pSysCall->print("From task5   1111");
	pTask->pSysCall->yield();

}











