
#ifndef ____CTIMER_H____
#define ____CTIMER_H____

#include "lbwnd.h"

typedef  void  (CTimerObject:: * TIMERPROC)(unsigned long nIDEvent);

class CTimer
{
public:
	CTimer();
 	~CTimer();

protected:
	struct tagTimer
	{
		CTimerObject 	*pTimerObj;
		unsigned long 	nIDEvent;
		unsigned long 	uElapse;
		TIMERPROC 		lpTimerFunc;
		unsigned long 	stTime;
		bool 			bPeriod;
		friend bool operator < (const struct tagTimer& cmpA,const struct tagTimer& cmpB)
		{
			return cmpA.stTime + cmpA.uElapse > cmpB.stTime + cmpB.uElapse;
		}		
	};

	struct TimerUpdateInfo
	{
		unsigned long nIDEvent;
		unsigned long endTime;
		friend bool operator < (const struct TimerUpdateInfo& cmpA,const struct TimerUpdateInfo& cmpB)
		{
			return cmpA.nIDEvent < cmpB.nIDEvent;
		}
	};
	std::priority_queue<tagTimer> m_TimerPriorityQueue;									// 优先队列队首返回离结束时间最短的任务
	std::unordered_map<CTimerObject*, std::set<TimerUpdateInfo> > m_umpControlTrigger; // 定时器触发的判定（控制删除和修改）
	
public:
	void KillAllTimer(void);
	unsigned long SetTimer(CTimerObject *pTimerObj, unsigned long nIDEvent, unsigned long uElapse, TIMERPROC lpTimerFunc = nullptr,bool bPeriod = true);
	bool KillTimer(CTimerObject *pTimerObj, unsigned long nIDEvent);
	bool KillTimer(CTimerObject *pTimerObj);
	bool ProcessTimer();

	void run();

protected:
	  std::thread thread_;	  
	  std::condition_variable runningCondvar_;
	  
	  std::mutex mutex_;		// 锁保护成员变量
	  bool running_{false};		// 运行状态
	
};

#endif
