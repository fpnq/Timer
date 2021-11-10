
#ifndef ____LB_WND_H____
#define ____LB_WND_H____

#include <sys/time.h>

#include <iostream>
//#include <math.h>

#include <queue>
#include <unordered_map>
#include <set>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>


using namespace std;


class CTimer;
extern CTimer	g_Timer;

class CTimerObject
{
public:
	CTimerObject();
	virtual ~CTimerObject();

public:
	virtual void OnTimer(unsigned long nIDEvent);	
};

#endif
