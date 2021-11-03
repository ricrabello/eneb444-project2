//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2016  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _kernel_h
#define _kernel_h

// #include "../../../pi-OS/include/circle/actled.h"
// #include "../../../pi-OS/include/circle/devicenameservice.h"
// #include "../../../pi-OS/include/circle/exceptionhandler.h"
// #include "../../../pi-OS/include/circle/input/touchscreen.h"
// #include "../../../pi-OS/include/circle/interrupt.h"
// #include "../../../pi-OS/include/circle/koptions.h"
// #include "../../../pi-OS/include/circle/logger.h"
// #include "../../../pi-OS/include/circle/memory.h"
// #include "../../../pi-OS/include/circle/screen.h"
// #include "../../../pi-OS/include/circle/serial.h"
// #include "../../../pi-OS/include/circle/timer.h"
// #include "../../../pi-OS/include/circle/types.h"

#include "circle/devicenameservice.h"
#include "circle/exceptionhandler.h"
#include "circle/input/touchscreen.h"
#include "circle/interrupt.h"
#include "circle/koptions.h"
#include "circle/logger.h"
#include "circle/memory.h"
#include "circle/screen.h"
#include "circle/serial.h"
#include "circle/timer.h"
#include "circle/types.h"
// for project 2 Jun Wang
// #include "circle/syscall.h"

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

struct TKernelTimer
{
#ifndef NDEBUG
	unsigned	     m_nMagic;
#define KERNEL_TIMER_MAGIC	0x4B544D43
#endif
	TKernelTimerHandler *m_pHandler;
	unsigned	     m_nElapsesAt;
	void 		    *m_pParam;
	void 		    *m_pContext;
};


class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);
	static TKernelTimer * kernel_timer;
	TShutdownMode Run (void);
	void write_log(const char * message, ...);
	void write_logV(const char * message, va_list Args);
	void set_timer(unsigned nDelay);
	static void TimerHandler (unsigned hTimer, void *pParam, void *pContext);
	void addKernelTimer (unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam);

private:
	static void TouchScreenEventHandler (TTouchScreenEvent Event,
					     unsigned nID, unsigned nPosX, unsigned nPosY);
	
private:
	// do not change this order
	CMemorySystem		m_Memory;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CScreenDevice		m_Screen;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CLogger			m_Logger;

	static CKernel *s_pThis;
};

#endif
