#include "lbwnd.h"
#include "lbtimer.h"
//#include "lbmsgqueue.h"
//#include "../netdriver/StatusDebug.h"
#include<iostream>

CTimer	g_Timer;

CTimerObject::CTimerObject()
{

}

CTimerObject::~CTimerObject()
{

}

void CTimerObject::OnTimer(unsigned long nIDEvent)
{
	cout<<"Call OnTimer "<<nIDEvent<<endl;
	g_Timer.KillTimer(this, nIDEvent);
}
