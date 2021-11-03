//
// kernel.cpp
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
#include "kernel.h"

#include <assert.h>
#include "circle/string.h"

static const char FromKernel[] = "kernel";

CKernel *CKernel::s_pThis = 0;

CKernel::CKernel (void)
:	m_Screen (1280, 1024),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer)
{
	s_pThis = this;

}

CKernel::~CKernel (void)
{
	s_pThis = 0;
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		CDevice *pTarget =  &m_Screen;

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}


	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);


	m_Logger.Write (FromKernel, LogNotice, "An exception will occur after 15 seconds from now");

	// start timer to elapse after 15 seconds
	m_Timer.StartKernelTimer (15 * HZ, TimerHandler);

	// generate a log message every second
	unsigned nTime = m_Timer.GetTime ();
	while (1)
	{
		while (nTime == m_Timer.GetTime ())
		{
			// just wait a second
		}

		nTime = m_Timer.GetTime ();

		m_Logger.Write (FromKernel, LogNotice, "Time is %u", nTime);
	}

	return ShutdownHalt;
}



void CKernel::addKernelTimer (unsigned nDelay,
		   TKernelTimerHandler *pHandler,
		   void *pParam)
{
	m_Timer.StartKernelTimer (nDelay, pHandler, pParam);
}




void CKernel::write_log(const char * pMessage, ...)
{
	//(const char *pSource, TLogSeverity Severity, const char *pMessage, ...)
	//m_Logger.Write (FromKernel, LogNotice, (const char *) pMessage);
	va_list var;
	va_start (var, pMessage);

	m_Logger.WriteV (FromKernel, LogNotice, pMessage, var);

	va_end (var);
}


void CKernel::write_logV(const char * pMessage, va_list Args)
{
	//(const char *pSource, TLogSeverity Severity, const char *pMessage, ...)
	//m_Logger.Write (FromKernel, LogNotice, (const char *) pMessage);
	//va_list var;
	//va_start (var, pMessage);

	m_Logger.WriteV (FromKernel, LogNotice, pMessage, Args);

	//va_end (var);
}


void CKernel::set_timer(unsigned nDelay )
{
	//CKernel::kernel_timer = (TKernelTimer * )
	m_Timer.StartKernelTimer (nDelay, TimerHandler, this);
	//this->write_log("timer eclipse = %d", CKernel::kernel_timer->m_nElapsesAt);


}

void CKernel::TimerHandler (unsigned hTimer, void *pParam, void *pContext)
{
	CKernel *pThis = (CKernel *) pParam;
	assert (pThis != 0);
	pThis->write_log("timer eclipse = asdf");
	//pThis->write_log("this is from kernel timer handler");
}


