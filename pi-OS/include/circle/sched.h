/*
 * sched.h
 *
 *  Created on: Feb 8, 2017
 *      Author: wangfakai
 */

#ifndef PI_OS_INCLUDE_CIRCLE_SCHED_H_
#define PI_OS_INCLUDE_CIRCLE_SCHED_H_

#include "task.h"
#include "syscall.h"
#define MAXTASK 100
#define KERNELTASKNUM 0
//static Task * taskQueue[MAXTASK];

typedef enum {
	KERNELSWITCHING,
	APPENDS,
	APPSLEEPING,
	APPEXIT
}TaskSwitchReason;

typedef enum {
	SCHEDULING,
	NOREADYTASK,
	NEEDTOSCHED,
	SLEEPING
}SCHEDState;

typedef struct {
	Task * taskQueue[MAXTASK];
	Task * pCurrentTask;
	Task * pLastTask;
	TaskSwitchReason lastTaskSwitchReason;
	volatile SCHEDState SchedulerState ;

}Tscheduler;

typedef enum {
	SCHED_FIFO,
	SCHED_PRIORITY
}ScheduleAlgorithm;


Tscheduler * getScheduler();
sysCall * getSysCallPointer();
void testEntry(Task * pTask);
void InitializeScheduler();
void startScheduler(Task * pTask, int ALGORITHM);
void schedulerRun(Task * pKernelTask);
int scheduleFIFO();
int schedulePriority();
int  getTaskNumByTaskPointer(Task *pTask);
unsigned GetNextTaskByPosition (int CurTaskNum);
unsigned GetNumOfNextReadyTaskByPosition (int CurTaskNum);
unsigned GetNumOfNextReadyTask (void);

void switchToTask (int numTask);


int genTaskID();
void AddTask (Task *pTask);
void BlockTask (Task **ppTask);
void WakeTask (Task **ppTask);		// can be called from interrupt context

void RemoveTaskFromQueue (Task *pTask);
unsigned GetNextTask (void);

void sleep (Task *pTask, unsigned nSeconds);
void msSleep (Task *pTask, unsigned nMilliSeconds);
void usSleep (Task *pTask, unsigned nMicroSeconds);
void sleepTimerHandler (unsigned hTimer, void *pParam, void *pContext);
void schedulerSleepTimerHandler(unsigned hTimer, void *pParam, void *pContext);
void yield (void);
void kernelYield(void);
void kernelYieldByNum(int );
void exit();
void sleepYield (void);



#endif /* PI_OS_INCLUDE_CIRCLE_SCHED_H_ */
