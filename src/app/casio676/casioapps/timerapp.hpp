#ifndef SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_

#include "casioapp.hpp"

class TimerStateMachine;

class TimerState{
public:
	TimerState(TimerStateMachine* sm):sm(sm){}

	virtual ~TimerState(){}
	virtual void activate(void) = 0;
	virtual bool processEvent(CasioEvent_t event) = 0;
	virtual void periodic(void) = 0;
protected:
	TimerStateMachine* sm;
};

class TimerStateMachine:public GenericStateMachine<TimerState>{
public:
	TimerStateMachine(void);
	virtual ~TimerStateMachine(void){}

	virtual void setText(const char* text) = 0;
	virtual void setBlink(uint8_t p) = 0;
	virtual void startBlinking(void) = 0;
	virtual void stopBlinking(void) = 0;
};

class TimerApp:public CasioApp, public TimerStateMachine{
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
			if(false == state->processEvent(event))
				sm->changeApp(false);
			break;
		default:
			state->processEvent(event);
			break;
		}
	}

	void setText(const char* text){
		sm->getDisplayManager().setLargeDigitText(text);
	}

	void setBlink(uint8_t p){
		sm->getDisplayManager().blinkLargeDigitText('_', p, 1);
	}

	void startBlinking(void){
		sm->getDisplayManager().startLargeDigitBlinking();
	}

	void stopBlinking(void){
		sm->getDisplayManager().stopBlinking(true);
	}
};

#endif /* SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_ */
