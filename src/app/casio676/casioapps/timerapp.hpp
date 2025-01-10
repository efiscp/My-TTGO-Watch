#ifndef SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_

#include "casioapp.hpp"

class TimerApp:public CasioApp{
public:
	TimerApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		sm->getDisplayManager().setDotMatrixText("TIMERg");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText(" 0:00 00");
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

#endif /* SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_ */
