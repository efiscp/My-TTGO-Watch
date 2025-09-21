#ifndef SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_
#define SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_

#include "casioDisplayManager.hpp"
#include "keypad/keypad.hpp"

class CasioStateMachine{
public:
	virtual void changeApp(bool resetToDefault) = 0;
	virtual Casio676_DisplayManager& getDisplayManager(void) = 0;
	virtual Keypad* getNumericKeypad(void) = 0;
	virtual Keypad* getAlphaNumericKeypad(void) = 0;

	virtual ~CasioStateMachine(){}
};


#endif /* SRC_APP_CASIO676_CASIOSTATEMACHINE_HPP_ */
