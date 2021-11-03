/*
 * task.h
 *
 *  Created on: Feb 8, 2017
 *      Author: wangfakai
 */

#ifndef PI_OS_INCLUDE_CIRCLE_TASK_H_
#define PI_OS_INCLUDE_CIRCLE_TASK_H_

#include <circle/sysconfig.h>
#include <circle/types.h>
#include <circle/taskswitch.h>
#include <circle/syscall.h>


#ifdef __cplusplus
extern "C" {
#endif

#define STACKSIZE	100

typedef enum
{
	TaskStateReady,
	TaskStateBlocked,
	TaskStateSleeping,
	TaskStateTerminated,
	TaskStateUnknown
}TTaskState;


typedef struct TTask
{
	u32 		ID;
	char 	name[32];
	volatile 	TTaskState State;
	struct TTask * pSelf;
	unsigned priority;
	void (*Run)(void * pTask);
	void (*taks_entry)(void * pTask);
	unsigned	WakeTicks;
	TTaskRegisters	   Regs;

	u8		   *pStack;
	unsigned	    StackSize;
	sysCall * pSysCall;


}Task;



void initializeTask(Task * pTask, unsigned ID, void (* enter)(Task * task), void (* run)(Task * task), void * sysCall);
void InitializeRegs (Task * pTask);
void TaskEntry ( Task * pTask);
void task1_run(Task * task);
void task2_run(Task * task);
void task3_run(Task * task);
void task4_run(Task * task);
void task5_run(Task * task);
void taskMatrix_run(Task * pTask);



#ifdef __cplusplus
}
#endif




#endif /* PI_OS_INCLUDE_CIRCLE_TASK_H_ */
