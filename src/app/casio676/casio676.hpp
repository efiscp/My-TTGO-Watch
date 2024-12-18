#ifndef CASIO_676_HPP
#define CASIO_676_HPP

#include "lvgl.h"
#include <ctime>
#include <sstream>
#include <iomanip>

enum CasioEvent_t {
	BUTTON_B_PRESSED,		//top left button
	BUTTON_C_PRESSED,		//bottom left button
	BUTTON_L_PRESSED,		//top right button
	BUTTON_A_PRESSED,		//bottom right button
};

class CasioStateMachine;
class CasioApp;
class TelApp;

class CasioApp{
public:
	CasioApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit)
	:sm(sm),dotMatrix(dotMatrix),smallDigit(smallDigit),largeDigit(largeDigit)
	{}

	virtual void processEvent(CasioEvent_t event) = 0;
	virtual void periodic(void){}

	virtual ~CasioApp(void){}
protected:
	CasioStateMachine* sm;

	lv_obj_t* dotMatrix;
	lv_obj_t* smallDigit;
	lv_obj_t* largeDigit;

	static constexpr char DAYS[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
};

constexpr char CasioApp::DAYS[7][4];

class CasioStateMachine{
public:
	virtual void changeApp(CasioApp* app) = 0;

	virtual ~CasioStateMachine(){}
};

class WatchApp:public CasioApp{
public:
	WatchApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		updateTime();
	}

	void processEvent(CasioEvent_t event);

	void periodic(void){
		updateTime();
	}

private:
	void updateTime(void){
		time_t now;
		struct tm info;

		time(&now);
		localtime_r(&now, &info);

		std::ostringstream oss;
		oss<<std::setw(2)<<info.tm_hour<<':'<<std::setw(2)<<std::setfill('0')<<info.tm_min<<' '<<std::setw(2)<<info.tm_sec;
		lv_label_set_text(largeDigit, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss<<std::setw(2)<<std::setfill(' ')<<info.tm_mon+1<<'-'<<std::setw(2)<<info.tm_mday;
		lv_label_set_text(smallDigit, oss.str().c_str());

		oss.clear();
		oss.str("");
		oss<<DAYS[info.tm_wday]<<"'"<<info.tm_year-100;
		lv_label_set_text(dotMatrix, oss.str().c_str());
	}
};

class StwApp:public CasioApp{
public:
	StwApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "ST-W");
		lv_label_set_text(smallDigit, "   00");
		lv_label_set_text(largeDigit, " 0:00 00");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new WatchApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class TimerApp:public CasioApp{
public:
	TimerApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "TIMERg");
		lv_label_set_text(smallDigit, "     ");
		lv_label_set_text(largeDigit, " 0:00 00");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new StwApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class AlarmApp:public CasioApp{
public:
	AlarmApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "ALARM");
		lv_label_set_text(smallDigit, "     ");
		lv_label_set_text(largeDigit, " 9:30");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new TimerApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class WtApp:public CasioApp{
public:
	WtApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "fWT");
		lv_label_set_text(smallDigit, " 5:35");
		lv_label_set_text(largeDigit, "12:34 56");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new AlarmApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class CalApp:public CasioApp{
public:
	CalApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "CAL");
		lv_label_set_text(smallDigit, "     ");
		lv_label_set_text(largeDigit, "       0");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new WtApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class SchedApp:public CasioApp{
public:
	SchedApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "SCHED");
		lv_label_set_text(smallDigit, "-----");
		lv_label_set_text(largeDigit, "--:--   ");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new CalApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

class TelApp:public CasioApp{
public:
	TelApp(CasioStateMachine* sm, lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):CasioApp(sm,dotMatrix,smallDigit,largeDigit){
		lv_label_set_text(dotMatrix, "eTEL");
		lv_label_set_text(smallDigit, "-- --");
		lv_label_set_text(largeDigit, "--------");
	}

	void processEvent(CasioEvent_t event){
		sm->changeApp(new SchedApp(sm,dotMatrix,smallDigit,largeDigit));
	}
};

void WatchApp::processEvent(CasioEvent_t event){
	sm->changeApp(new TelApp(sm,dotMatrix,smallDigit,largeDigit));
}

class Casio676:public CasioStateMachine{

public:
	Casio676(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit){
		app = new WatchApp(this,dotMatrix,smallDigit,largeDigit);
	}

	void changeApp(CasioApp* newApp){
		if(NULL != newApp){
			delete app;
			app = newApp;
		}
	}

	void processEvent(CasioEvent_t event){
		app->processEvent(event);
	}

	void periodic(void){
		app->periodic();
	}

private:
	CasioApp* app;
};

#endif
