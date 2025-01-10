#ifndef SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_

#include "casioapp.hpp"

class WtApp:public CasioApp{
public:
	WtApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		sm->getDisplayManager().setDotMatrixText("fWT");
		sm->getDisplayManager().setSmallDigitText(" 5:35");
		sm->getDisplayManager().setLargeDigitText("12:34 56");
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

#endif /* SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_ */
