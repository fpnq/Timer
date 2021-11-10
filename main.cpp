#include "lbtimer.h"
#include "test.h"
#include "test.cpp"
#define fdebug() freopen("out.log","a",stdout)
void func()
{
	std::cout<<"call func"<<std::endl;
}

int main()
{	
	CTimerObject _Wnd[5];
	
	//_Wnd.
	//_Wnd.func1(1);
	//TIMERPROC fun = &_Wnd.func1; ERROR
	//TIMERPROC fun = &CTimerObject::func1;
	//g_Timer.SetTimer(&_Wnd,3,3000, fun);
	for(int i = 0 ;i< 5;++i){
		g_Timer.SetTimer(_Wnd+i,i+1,i+5000,nullptr, 1);
	}
	
	//g_Timer.SetTimer(&_Wnd,1,2000, nullptr, 1);
	/*
	while(1){
		long long endtime = time(NULL);
		if(endtime - begintime >= 1){
			begintime = endtime;
			g_Timer.ProcessTimer();
		}
	}	
	*/
	while(1);
	return 0;
}

