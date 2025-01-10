#ifndef SRC_APP_CASIO676_CASIOAPPS_TELAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_TELAPP_HPP_

#include "casioapp.hpp"

class TelApp:public CasioApp{
public:
	TelApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		initialText = true;
		CasioApp::activate();
		sm->getDisplayManager().setDotMatrixText("eTEL");
		sm->getDisplayManager().setSmallDigitText("-- --");
		sm->getDisplayManager().setLargeDigitText("--------");
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

	void periodic(void){
		if(initialText){
			if(getElapsedMs() > 1000){
				initialText = false;
				sm->getDisplayManager().setDotMatrixText("");
			}
		}
	}

private:
	bool initialText = true;
};

#endif /* SRC_APP_CASIO676_CASIOAPPS_TELAPP_HPP_ */
