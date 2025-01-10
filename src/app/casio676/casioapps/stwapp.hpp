#ifndef SRC_APP_CASIO676_CASIOAPPS_STWAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_STWAPP_HPP_

#include "casioapp.hpp"

class StwStateMachine;

//abstract state class
class StwState{
public:
	StwState(StwStateMachine* sm):sm(sm){}

	virtual ~StwState(){}
	virtual void activate(void) = 0;
	virtual void processEvent(CasioEvent_t event) = 0;
	virtual void periodic(void) = 0;
protected:
	StwStateMachine* sm;
};

//abstract state machine class
class StwStateMachine{
public:
	StwStateMachine(void);

	virtual ~StwStateMachine(void){
		delete state;
		state = 0;
	}

	virtual void changeState(StwState* s){
		if(s != NULL){
			delete state;
			state = s;
		}
	}

	//time functions
	virtual void storeRefTime(void) = 0;
	virtual void clearAccumulator(void) = 0;
	virtual void accumulate(void) = 0;
	virtual void storeSplitTime(void) = 0;


	//display management
	virtual void clearDisplay(void) const = 0;
	virtual void printCurrentTime(void) const = 0;
	virtual void printAccumulatedTime(void) const = 0;
	virtual void printSplitTime(void) const = 0;

	virtual void printStw(void) const = 0;
	virtual void printSpl(void) const = 0;

protected:
	StwState* state;
};

//ST-W application
class StwApp:public CasioApp, public StwStateMachine{
public:
	StwApp(CasioStateMachine* sm):CasioApp(sm),accumulator(std::chrono::nanoseconds(0)){}

	void changeState(StwState* s){
		if(s != NULL){
			delete state;
			state = s;
		}
	}

	//delegate activation event to state
	void activate(void){
		state->activate();
	}

	//delegate button press event to state
	void processEvent(CasioEvent_t event){

		if(BUTTON_C_PRESSED == event)
			sm->changeApp(false);
		else
			state->processEvent(event);
	}

	//delegate periodic call to state
	void periodic(void){
		state->periodic();
	}

	//register reference time
	void storeRefTime(void){
		ref = std::chrono::steady_clock::now();
	}

	//clear accumulator
	void clearAccumulator(void){
		accumulator = std::chrono::nanoseconds(0);
	}

	//add elapsed time to accumulator
	void accumulate(void){
		accumulator += getElapsed(ref);
	}

	//store elapsed time to split mode buffer
	void storeSplitTime(void){
		split = accumulator + getElapsed(ref);
	}

	//initialize display to initial state
	void clearDisplay(void) const{
		//print initial information
		sm->getDisplayManager().setDotMatrixText("ST-W");
		sm->getDisplayManager().setSmallDigitText("   00");
		sm->getDisplayManager().setLargeDigitText(" 0:00 00");
	}

	//print total elapsed time since last clear
	void printCurrentTime(void) const{
		Timeinfo info;
		(void)getElapsed(info, ref, accumulator);
		printTimeToDisplay(info);
	}

	//print duration stored in accumulator
	void printAccumulatedTime(void) const{
		Timeinfo info;
		convertDuration(info, accumulator);

		printTimeToDisplay(info);
	}

	//print duration stored in split buffer
	void printSplitTime(void) const{
		Timeinfo info;
		convertDuration(info, split);

		printTimeToDisplay(info);
	}

	//print ST-W label to dot matrix
	void printStw(void) const{
		sm->getDisplayManager().setDotMatrixText("ST-W");
	}

	//print SPL label to dot matrix
	void printSpl(void) const{
		sm->getDisplayManager().setDotMatrixText("SPL");
	}

private:

	//format elapsed time and print
	void printTimeToDisplay(Timeinfo& info) const{
		std::ostringstream oss;

		//print hundredth seconds
		oss<<"   "<<std::setw(2)<<std::setfill('0')<<info.milliseconds/10;
		sm->getDisplayManager().setSmallDigitText(oss.str());

		//clear buffer, then print hours, minutes and seconds
		oss.clear();
		oss.str("");
		oss<<std::setw(2)<<std::setfill(' ')<<info.hours<<':'<<std::setw(2)<<std::setfill('0')<<info.minutes<<' '<<std::setw(2)<<std::setfill('0')<<info.seconds;
		sm->getDisplayManager().setLargeDigitText(oss.str());

	}

	std::chrono::time_point<std::chrono::steady_clock> ref;
	std::chrono::nanoseconds accumulator;
	std::chrono::nanoseconds split;
};

#endif /* SRC_APP_CASIO676_CASIOAPPS_STWAPP_HPP_ */
