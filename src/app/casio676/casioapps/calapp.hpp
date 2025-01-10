#ifndef SRC_APP_CASIO676_CASIOAPPS_CALAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_CALAPP_HPP_

#include "casioapp.hpp"

class CalApp:public CasioApp{
public:
	CalApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		sm->getDisplayManager().setDotMatrixText("CAL");
		sm->getDisplayManager().setSmallDigitText("     ");
		sm->getDisplayManager().setLargeDigitText("       0");
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

#endif /* SRC_APP_CASIO676_CASIOAPPS_CALAPP_HPP_ */
