#include "stwapp.hpp"

class StopState:public StwState{
public:
	StopState(StwStateMachine* sm):StwState(sm){}

	void activate(void){
		sm->printStw();
		sm->printAccumulatedTime();
	}

	void processEvent(CasioEvent_t event);

	void periodic(void){
		//nothing to do here
	}
};

class RunState:public StwState{
public:
	RunState(StwStateMachine* sm):StwState(sm){}

	void activate(void){
		sm->printStw();
		sm->printCurrentTime();
	}

	void processEvent(CasioEvent_t event);

	void periodic(void){
		//refresh screen periodically
		sm->printCurrentTime();
	}
};

class SplState:public StwState{
public:
	SplState(StwStateMachine* sm):StwState(sm){}

	void activate(void){
		sm->printSpl();
		sm->printSplitTime();
	}

	void processEvent(CasioEvent_t event);

	void periodic(void){
		//nothing to do here
	}
};


void StopState::processEvent(CasioEvent_t event){
	switch(event){
	case BUTTON_L_PRESSED:
		//clear accumulator & display
		sm->clearAccumulator();
		sm->clearDisplay();

		//keep current state
		break;
	case BUTTON_A_PRESSED:
		//start stopwatch -> store current time and switch to run state
		sm->storeRefTime();

		//enter run state
		sm->changeState(new RunState(sm));
		break;
	default:
		break;
	}
}

void RunState::processEvent(CasioEvent_t event){
	switch(event){
	case BUTTON_L_PRESSED:
		//activate split mode
		sm->storeSplitTime();
		sm->printSpl();
		sm->printSplitTime();

		sm->changeState(new SplState(sm));
		break;
	case BUTTON_A_PRESSED:
		//accumulate and print elapsed time
		sm->accumulate();
		sm->printAccumulatedTime();

		//switch to stop state
		sm->changeState(new StopState(sm));
		break;
	default:
		break;
	}
}

void SplState::processEvent(CasioEvent_t event){
	switch(event){
	case BUTTON_L_PRESSED:
		//deactivate split mode & resume run mode
		sm->printStw();
		sm->printCurrentTime();

		sm->changeState(new RunState(sm));
		break;
	case BUTTON_A_PRESSED:
		//accumulate and switch to stop state & print elapsed time
		sm->printStw();
		sm->accumulate();
		sm->printAccumulatedTime();

		//switch to stop state
		sm->changeState(new StopState(sm));
		break;
	default:
		break;
	}
}

//set initial state to STOP
StwStateMachine::StwStateMachine(void){
	state = new StopState(this);
}
