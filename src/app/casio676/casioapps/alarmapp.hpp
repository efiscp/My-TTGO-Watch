#ifndef SRC_APP_CASIO676_CASIOAPPS_ALARMAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_ALARMAPP_HPP_

#include "casioapp.hpp"

class AlarmApp:public CasioApp{
public:
	AlarmApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		sm->getDisplayManager().setDotMatrixText("ALARM");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText(" 9:30");
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

#endif /* SRC_APP_CASIO676_CASIOAPPS_ALARMAPP_HPP_ */
