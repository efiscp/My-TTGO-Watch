#ifndef SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_
#define SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_

#include "casioDisplayManager.hpp"

enum CasioEvent_t {
	BUTTON_B_PRESSED,		//top left button
	BUTTON_C_PRESSED,		//bottom left button
	BUTTON_L_PRESSED,		//top right button
	BUTTON_A_PRESSED,		//bottom right button
};

class CasioStateMachine{
public:
	virtual void changeApp(bool resetToDefault) = 0;
	virtual Casio676_DisplayManager& getDisplayManager(void) = 0;

	virtual ~CasioStateMachine(){}
};


#endif /* SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_ */
