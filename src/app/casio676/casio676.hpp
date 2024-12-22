#ifndef CASIO_676_HPP
#define CASIO_676_HPP

#include "lvgl.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>

enum CasioEvent_t {
	BUTTON_B_PRESSED,		//top left button
	BUTTON_C_PRESSED,		//bottom left button
	BUTTON_L_PRESSED,		//top right button
	BUTTON_A_PRESSED,		//bottom right button
};

class CasioStateMachine;
class CasioApp;
class TelApp;

class Casio676_DisplayManager{
public:
	Casio676_DisplayManager(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):
		dotMatrix(dotMatrix), smallDigit(smallDigit), largeDigit(largeDigit){}

	void setDotMatrixText(const std::string& text){
		lv_label_set_text(dotMatrix, text.c_str());
	}

	void setDotMatrixText(const char* text){
		lv_label_set_text(dotMatrix, text);
	}

	void setSmallDigitText(const std::string& text){
		lv_label_set_text(smallDigit, text.c_str());
	}

	void setSmallDigitText(const char* text){
		lv_label_set_text(smallDigit, text);
	}

	void setLargeDigitText(const std::string& text){
		lv_label_set_text(largeDigit, text.c_str());
	}

	void setLargeDigitText(const char* text){
		lv_label_set_text(largeDigit, text);
	}

private:
		lv_obj_t* dotMatrix;
		lv_obj_t* smallDigit;
		lv_obj_t* largeDigit;
};

class CasioApp{
public:
	CasioApp(CasioStateMachine* sm):sm(sm){}

	virtual void processEvent(CasioEvent_t event) = 0;
	virtual void periodic(void){}

	virtual ~CasioApp(void){}
protected:

	int64_t getElapsedMs(void) const{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - constTime);

		return elapsed.count();
	}

	CasioStateMachine* sm;

	const std::chrono::time_point<std::chrono::steady_clock> constTime = std::chrono::steady_clock::now();

	static constexpr char DAYS[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
};

constexpr char CasioApp::DAYS[7][4];

class CasioStateMachine{
public:
	virtual void changeApp(CasioApp* app) = 0;
	virtual Casio676_DisplayManager& getDisplayManager(void) = 0;

	virtual ~CasioStateMachine(){}
};

class WatchApp:public CasioApp{
public:
	WatchApp(CasioStateMachine* sm):CasioApp(sm){
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
		sm->getDisplayManager().setLargeDigitText(oss.str());

		oss.clear();
		oss.str("");
		oss<<std::setw(2)<<std::setfill(' ')<<info.tm_mon+1<<'-'<<std::setw(2)<<info.tm_mday;
		sm->getDisplayManager().setSmallDigitText(oss.str());

		oss.clear();
		oss.str("");
		oss<<DAYS[info.tm_wday]<<"'"<<info.tm_year-100;
		sm->getDisplayManager().setDotMatrixText(oss.str());
	}
};

class StwApp:public CasioApp{
public:
	StwApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("ST-W");
		sm->getDisplayManager().setSmallDigitText("   00");
		sm->getDisplayManager().setLargeDigitText(" 0:00 00");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new WatchApp(sm));
			break;
		default:
			break;
		}
	}
};

class TimerApp:public CasioApp{
public:
	TimerApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("TIMERg");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText(" 0:00 00");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new StwApp(sm));
			break;
		default:
			break;
		}
	}
};

class AlarmApp:public CasioApp{
public:
	AlarmApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("ALARM");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText(" 9:30");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new TimerApp(sm));
			break;
		default:
			break;
		}
	}
};

class WtApp:public CasioApp{
public:
	WtApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("fWT");
		sm->getDisplayManager().setSmallDigitText(" 5:35");
		sm->getDisplayManager().setLargeDigitText("12:34 56");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new AlarmApp(sm));
			break;
		default:
			break;
		}
	}
};

class CalApp:public CasioApp{
public:
	CalApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("CAL");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText("       0");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new WtApp(sm));
			break;
		default:
			break;
		}
	}
};

class SchedApp:public CasioApp{
public:
	SchedApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("SCHED");
		sm->getDisplayManager().setSmallDigitText("-----");
		sm->getDisplayManager().setLargeDigitText("--:--   ");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new CalApp(sm));
			break;
		default:
			break;
		}
	}
};

class TelApp:public CasioApp{
public:
	TelApp(CasioStateMachine* sm):CasioApp(sm){
		sm->getDisplayManager().setDotMatrixText("eTEL");
		sm->getDisplayManager().setSmallDigitText("-- --");
		sm->getDisplayManager().setLargeDigitText("--------");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(new SchedApp(sm));
			break;
		default:
			break;
		}
	}

	void periodic(void){
		if(initialText){
			if(getElapsedMs() > 500){
				initialText = false;
				sm->getDisplayManager().setDotMatrixText("");
			}
		}
	}

private:
	bool initialText = true;
};

void WatchApp::processEvent(CasioEvent_t event){
	switch(event){
	case BUTTON_C_PRESSED:
		sm->changeApp(new TelApp(sm));
		break;
	default:
		break;
	}
}

class Casio676:public CasioStateMachine{

public:
	Casio676(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):dm(dotMatrix, smallDigit, largeDigit){
		app = new WatchApp(this);
	}

	void changeApp(CasioApp* newApp){
		if(NULL != newApp){
			delete app;
			app = newApp;
		}
	}

	Casio676_DisplayManager& getDisplayManager(void){
		return dm;
	}

	void processEvent(CasioEvent_t event){
		if(NULL != app)
			app->processEvent(event);
	}

	void periodic(void){
		if(NULL != app)
			app->periodic();
	}

private:

	CasioApp* app;
	Casio676_DisplayManager dm;
};

#endif
