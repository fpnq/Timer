#include "lbtimer.h"

CTimer::CTimer()
{
	while(!m_TimerPriorityQueue.empty())
	{
		m_TimerPriorityQueue.pop();
	}
	running_ = true;
	thread_ = std::thread([&] { this->run(); });
	
}

CTimer::~CTimer()
{
	KillAllTimer();
}

bool CTimer::ProcessTimer()
{	
	std::unique_lock<std::mutex> lock(mutex_);
	if (m_TimerPriorityQueue.empty())
	{
		runningCondvar_.wait(lock);
		return false;
	}
	
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long curTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	auto CheckAdd = [&](tagTimer* pTask) -> bool
	{		
		if(0 == m_umpControlTrigger.count(pTask->pTimerObj))
		{
			cout<<"LBTimer::ProcessTimer 对象被删除"<<endl;
			return false;	// 对象被删除
		}
		auto it_set = m_umpControlTrigger[pTask->pTimerObj].find({pTask->nIDEvent, 0});
		if(it_set == m_umpControlTrigger[pTask->pTimerObj].end())
		{
			cout<<"LBTimer::ProcessTimer 事件被删除"<<endl;
			return false;	// 事件被删除
		}
		else if(it_set->endTime != pTask->stTime + pTask->uElapse)
		{
			cout<<"LBTimer::ProcessTimer 时间被删除"<<endl;
			return false;	// 时间被修改
		}
		return true;
	};
	
	while(!m_TimerPriorityQueue.empty())
	{
		tagTimer topTask = m_TimerPriorityQueue.top();
		
		// 任务未响应
		if(curTime < (topTask.stTime + topTask.uElapse))
		{
			auto sleepTime = std::chrono::milliseconds(topTask.stTime + topTask.uElapse - curTime);
			
			runningCondvar_.wait_for(lock, sleepTime);
			
			//cout<<"LBTimer::ProcessTimer 任务未响应 线程等待时间: " <<topTask.stTime + topTask.uElapse - curTime<<endl;
			break;
		}
		else
		{
			// 队首出队
			m_TimerPriorityQueue.pop();
			
			// 已被删除或修改过的任务不生效
			if(!CheckAdd(&topTask))
				continue;

			// 运行任务
			if (topTask.pTimerObj != nullptr)
			{				
				if (topTask.lpTimerFunc != nullptr)
				{					
					//cout<<"LBTimer::ProcessTimer Call Func "<<topTask.nIDEvent<<endl;
					(topTask.pTimerObj->*(topTask.lpTimerFunc))(topTask.nIDEvent);
				}
				else
				{
					cout<<topTask.pTimerObj<<":  "<<topTask.nIDEvent<<" "<<topTask.stTime + topTask.uElapse<<endl;
					//cout<<"LBTimer::ProcessTimer Call OnTime "<<topTask.nIDEvent<<endl;
					topTask.pTimerObj->OnTimer(topTask.nIDEvent);
				}
			}
			// 定时执行任务
			if(topTask.bPeriod)
			{
				if(!CheckAdd(&topTask))
					continue;
				tagTimer newTask = std::move(topTask);
				gettimeofday(&tv, NULL);
				//cout<<"LBTimer::ProcessTimer 周期循环 "<<topTask.nIDEvent<<endl;
				cout<<"开始设置 了"<<endl;
				SetTimer(newTask.pTimerObj, newTask.nIDEvent, newTask.uElapse, newTask.lpTimerFunc, newTask.bPeriod);
			}			
		}
	}		
	return true;
}

void CTimer::run()
{
	while (running_)
	{		
		ProcessTimer();	
	}
}

void CTimer::KillAllTimer(void)
{
	mutex lock_;
	lock_.lock();
	while(!m_TimerPriorityQueue.empty())
	{
		m_TimerPriorityQueue.pop();
	}
	m_umpControlTrigger.clear();
	lock_.unlock();
}

unsigned long CTimer::SetTimer(CTimerObject *pTimerObj, unsigned long nIDEvent, unsigned long uElapse, TIMERPROC lpTimerFunc, bool bPeriod)
{
	mutex_.unlock();
	std::unique_lock<std::mutex> lock(mutex_);
	if (pTimerObj == NULL || uElapse <= 0 || nIDEvent <= 0)
	{
		//cout<< "SetTimer 异常退出"<<endl;
		return 0;
	}
			
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	tagTimer timTask;
	timTask.pTimerObj		= pTimerObj;
	timTask.nIDEvent		= nIDEvent;
	timTask.uElapse			= uElapse;
	timTask.lpTimerFunc		= lpTimerFunc;
	timTask.bPeriod			= bPeriod;
	timTask.stTime			= tv.tv_sec * 1000 + tv.tv_usec / 1000;

	//cout<< "LBTimer::SetTimer "<< nIDEvent<<" --- endTime: "<< uElapse + timTask.stTime<<" "<<bPeriod<<endl;
	
	m_TimerPriorityQueue.push(std::move(timTask));
	//cout<<m_TimerPriorityQueue.size()<<endl;
	
	//Update
	{
		auto it_set = m_umpControlTrigger[pTimerObj].find({nIDEvent, 0});
		if(it_set != m_umpControlTrigger[pTimerObj].end())
		{			
			m_umpControlTrigger[pTimerObj].erase(*it_set);
		}		
		TimerUpdateInfo tmp;
		tmp.endTime  = timTask.stTime + uElapse;
		tmp.nIDEvent = nIDEvent;
		m_umpControlTrigger[pTimerObj].insert(std::move(tmp));
	}
	runningCondvar_.notify_one();
	return nIDEvent;
}


bool CTimer::KillTimer(CTimerObject *pTimerObj, unsigned long nIDEvent)
{	
	std::mutex lock_;
	lock_.lock();
	if(0 == m_umpControlTrigger.count(pTimerObj))
	{
		return false;
	}
	auto it_set = m_umpControlTrigger[pTimerObj].find({nIDEvent, 0});
	if(it_set == m_umpControlTrigger[pTimerObj].end())
	{
		return false;
	}
	else
	{
		m_umpControlTrigger[pTimerObj].erase(*it_set);
	}	
	lock_.unlock();
	return true;
}

bool CTimer::KillTimer(CTimerObject *pTimerObj)
{	
	std::mutex lock_;
	lock_.lock();
	std::unique_lock<std::mutex> lock(mutex_);
	if(0 == m_umpControlTrigger.count(pTimerObj))
	{
		return false;
	}
	m_umpControlTrigger.erase(pTimerObj);
	lock_.unlock();
	return true;
}

