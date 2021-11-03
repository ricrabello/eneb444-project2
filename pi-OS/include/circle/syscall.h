/*
 * syscall.h
 *
 *  Created on: Feb 9, 2017
 *      Author: wangfakai
 */

#ifndef PI_OS_INCLUDE_CIRCLE_SYSCALL_H_
#define PI_OS_INCLUDE_CIRCLE_SYSCALL_H_
typedef void TKernelTimerHandler (unsigned hTimer, void *pParam, void *pContext);

typedef struct
{
	void (*sleep)(void *pTask, unsigned nSeconds);
	void (*yield)();
	void (*exit)();
	void (*print)(const char* message);
	void (*printV)(const char* message,...);
	void (*addKernelTimer)(unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam);

}sysCall;



#endif /* PI_OS_INCLUDE_CIRCLE_SYSCALL_H_ */
