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
	}

	void changeApp(bool resetToDefault){
		(*appItr)->deactivate();

		if(resetToDefault || ++appItr == appList.end())
			appItr = appList.begin();

		(*appItr)->activate();
	}

	Casio676_DisplayManager& getDisplayManager(void){
		return dm;
	}

	void processEvent(CasioEvent_t event){
		(*appItr)->processEvent(event);
	}

	void periodic(void){
		(*appItr)->periodic();
	}

private:

	Casio676_DisplayManager dm;

	std::list<CasioApp*> appList;
	std::list<CasioApp*>::iterator appItr;
};

#endif
