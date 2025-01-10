#ifndef SRC_APP_CASIO676_CASIOAPPS_SCHEDAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_SCHEDAPP_HPP_

#include "casioapp.hpp"

class SchedApp:public CasioApp{
public:
	SchedApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		sm->getDisplayManager().setDotMatrixText("SCHED");
		sm->getDisplayManager().setSmallDigitText("-----");
		sm->getDisplayManager().setLargeDigitText("--:--   ");
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(false);
			break;
		default:
			break;
		}
	}
};

#endif /* SRC_APP_CASIO676_CASIOAPPS_SCHEDAPP_HPP_ */
