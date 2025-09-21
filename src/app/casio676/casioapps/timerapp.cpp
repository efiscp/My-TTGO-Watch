#include "timerapp.hpp"

namespace TimerStateNs{
class StopState:public TimerState{
public:
	StopState(TimerStateMachine* s):TimerState(s){}

	void activate(void){
		//sm->updateTime();
	}

	bool processEvent(CasioEvent_t event);

	void periodic(void) {

	}
};

class RunState:public TimerState{
public:
	RunState(TimerStateMachine* s):TimerState(s){}

	void activate(void){

	}

	bool processEvent(CasioEvent_t event);

	void periodic(void) {
		if(sm->recalcRemainingAndUpdateDisplay())
			transitionToStop();
	}

private:
	void transitionToStop(void){
		sm->stopBlinking();
		sm->changeState(new StopState(sm));
	}
};

class SetState:public TimerState{
public:
	SetState(TimerStateMachine* s):TimerState(s){}

	void activate(void){

	}

	bool processEvent(CasioEvent_t event);

	void periodic(void) {

	}
private:

	uint8_t moveRight(void){
		++pos;
		if(2 == pos) return ++pos;
		if(pos > 4) pos = 0;

		return pos;
	}

	uint8_t moveLeft(void){
		--pos;
		if(2 == pos) return --pos;
		if(pos < 0) pos = 4;

		return pos;
	}

	void setChar(const char c){
		sm->setChar(c, pos);
		digits[(pos>1)?pos-1:pos] = c - '0';
	}

	int8_t pos = 0;
};
}
bool TimerStateNs::StopState::processEvent(CasioEvent_t event){
	bool retval = false;

	switch(event){
	case BUTTON_B_PRESSED:
		sm->restoreRemaining();
		sm->editMode(true);
		sm->setBlink(0);
		sm->startBlinking();

		sm->changeState(new SetState(sm));
		retval = true;
		break;
	case BUTTON_L_PRESSED:
		sm->restoreRemainingAndUpdateDisplay();
		break;
	case BUTTON_A_PRESSED:
		sm->startBlinking();
		sm->startTimer();
		sm->changeState(new RunState(sm));
		break;
	case BUTTON_C_PRESSED:
		break;
	default:
		break;
	}

	return retval;
}

bool TimerStateNs::RunState::processEvent(CasioEvent_t event){
	bool retval = false;

	switch(event){
	case BUTTON_B_PRESSED:
		break;
	case BUTTON_L_PRESSED:
		break;
	case BUTTON_A_PRESSED:
		sm->stopTimer();
		transitionToStop();
		break;
	case BUTTON_C_PRESSED:
		break;
	default:
		break;
	}

	return retval;
}

bool TimerStateNs::SetState::processEvent(CasioEvent_t event){
	bool retval = false;

	switch(event){
	case BUTTON_B_PRESSED:
		//sm->setText(" 0:00 00");
		sm->calcTimeAndUpdateDisplay(digits);
		sm->stopBlinking();

		sm->changeState(new StopState(sm));
		retval = true;
		break;
	case BUTTON_L_PRESSED:
		sm->switchAutoRepeat();
		break;
	case BUTTON_A_PRESSED:
		sm->setBlink(moveRight());
		retval = true;
		break;
	case BUTTON_C_PRESSED:
		sm->setBlink(moveLeft());
		retval = true;
		break;
	default:{
		char key;
		if(sm->getKey(event, &key)){
			setChar(key);
			sm->setBlink(moveRight());
		}
		break;
	}
	}

	return retval;
}

TimerStateMachine::TimerStateMachine(void):GenericStateMachine<TimerState>(new TimerStateNs::StopState(this)){}
uint8_t TimerState::digits[4];



