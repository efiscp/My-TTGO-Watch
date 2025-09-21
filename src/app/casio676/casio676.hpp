#ifndef CASIO_676_HPP
#define CASIO_676_HPP

#include <list>

//casio apps
#include "casioapps/watchapp.hpp"
#include "casioapps/telapp.hpp"
#include "casioapps/schedapp.hpp"
#include "casioapps/calapp.hpp"
#include "casioapps/wtapp.hpp"
#include "casioapps/alarmapp.hpp"
#include "casioapps/timerapp.hpp"
#include "casioapps/stwapp.hpp"

class Casio676:public CasioStateMachine{

public:
	Casio676(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):dm(dotMatrix, smallDigit, largeDigit){
		appList.push_back(new WatchApp(this));
		appList.push_back(new TelApp(this));
		appList.push_back(new SchedApp(this));
		appList.push_back(new CalApp(this));
		appList.push_back(new WtApp(this));
		appList.push_back(new AlarmApp(this));
		appList.push_back(new TimerApp(this));
		appList.push_back(new StwApp(this));

		appItr = appList.begin();

		num = new NumericKeypad();
		alphanum = new AlphanumericKeypad();
	}

	~Casio676(void){
		delete num;
		delete alphanum;
	}

	void changeApp(bool resetToDefault){
		(*appItr)->deactivate();

		dm.stopBlinking(false);
		alphanum->reset();
		num->reset();

		if(resetToDefault || ++appItr == appList.end())
			appItr = appList.begin();

		(*appItr)->activate();
	}

	Casio676_DisplayManager& getDisplayManager(void){
		return dm;
	}

	Keypad* getNumericKeypad(void){
		return num;
	}

	Keypad* getAlphaNumericKeypad(void){
		return alphanum;
	}

	void processEvent(CasioEvent_t event){
		(*appItr)->processEvent(event);
	}

	void periodic(void){
		CasioApp::Timeinfo info;
		CasioApp::convertDuration(info, std::chrono::steady_clock::now().time_since_epoch());

		if(info.milliseconds >= 500 && !blink){
			dm.blink();
			blink = true;
		}else if(info.milliseconds < 500 && blink){
			dm.blink();
			blink = false;
		}


		(*appItr)->periodic();
	}

private:

	Casio676_DisplayManager dm;
	Keypad* num;
	Keypad* alphanum;

	std::list<CasioApp*> appList;
	std::list<CasioApp*>::iterator appItr;

	bool blink = false;
};

#endif
