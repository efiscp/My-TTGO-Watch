#ifndef SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_

#include "casioapp.hpp"
#include <string>

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
	static uint8_t digits[4];
};

class TimerStateMachine:public GenericStateMachine<TimerState>{
public:
	TimerStateMachine(void);
	virtual ~TimerStateMachine(void){}

	virtual void setText(const char* text) = 0;
	virtual void setChar(const char c, uint8_t pos) = 0;
	virtual void setBlink(uint8_t p) = 0;
	virtual void startBlinking(void) = 0;
	virtual void stopBlinking(void) = 0;

	virtual void calcTimeAndUpdateDisplay(const uint8_t* const buf) = 0;
	virtual bool recalcRemainingAndUpdateDisplay(void) = 0;
	virtual void editMode(bool on) = 0;
	//virtual void updateDisplay(bool editMode) = 0;
	virtual void restoreRemaining(void) = 0;
	virtual void restoreRemainingAndUpdateDisplay(void) = 0;
	virtual void switchAutoRepeat(void) = 0;
	virtual void startTimer(void) = 0;
	virtual void stopTimer(void) = 0;

	virtual bool getKey(CasioEvent_t e, char* key) = 0;
};

class TimerApp:public CasioApp, public TimerStateMachine{
public:
	TimerApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		updateDotMatrix();
		sm->getDisplayManager().setSmallDigitText("     ");

		updateDisplay(false);
		//sm->getDisplayManager().setLargeDigitText(" 0:00 00");
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

	void periodic(void){
		state->periodic();
	}

	void setText(const char* text){
		sm->getDisplayManager().setLargeDigitText(text);
	}

	void setChar(const char c, uint8_t pos){
		sm->getDisplayManager().setLargeDigitChar(c, pos);
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


	void calcTimeAndUpdateDisplay(const uint8_t* const buf){
		period = std::chrono::hours((buf[0] * 10 + buf[1]) % 24) + std::chrono::minutes((buf[2] * 10 + buf[3]) % 60);
		remaining = period;
		updateDisplay(false);
	}

	void restoreRemainingAndUpdateDisplay(void){
		remaining = period;
		updateDisplay(false);
	}

	void restoreRemaining(void){
		remaining = period;
	}

	bool recalcRemainingAndUpdateDisplay(void){
		bool retval = false;
		auto now = std::chrono::steady_clock::now();
		if(now >= endTime){

			remaining = period;

			if(!autoRepeat)
				retval = true;
			else{
				remaining -= now - endTime;
				endTime = now + remaining;
			}

			//log_d("TIMER ELAPSED");
			//TODO: vibrate

		}else
			remaining = endTime - std::chrono::steady_clock::now();

		updateDisplay(false);

		return retval;
	}

	void editMode(bool on){
		updateDisplay(on);
	}

	bool getKey(CasioEvent_t e, char* key){
		return sm->getNumericKeypad()->get(e, key);
	}

	void switchAutoRepeat(void){
		autoRepeat = !autoRepeat;
		updateDotMatrix();
	}

	void startTimer(void){
		//treat 0:00:00 as 24:00:00
		if(remaining == std::chrono::nanoseconds(0))
			remaining = std::chrono::hours(24);
		endTime = std::chrono::steady_clock::now() + remaining;
	}

	void stopTimer(void){
		remaining = endTime - std::chrono::steady_clock::now();
	}

private:

	void roundup(Timeinfo& tm){
		if(tm.milliseconds){
			//round up
			//log_d("%02d:%02d:%02d.%03d", tm.hours, tm.minutes, tm.seconds, tm.milliseconds);
			int carry = tm.milliseconds ? 1 : 0;
			tm.seconds = (tm.seconds + carry) % 60;
			carry &= 0 == tm.seconds;
			tm.minutes = (tm.minutes + carry) % 60;
			carry &= 0 == tm.minutes;
			tm.hours = tm.hours + carry;
		}
	}

	void updateDisplay(bool editMode){

		Timeinfo tm;
		std::ostringstream oss;
		char filler = '0';

		convertDuration(tm, remaining);

		if(!editMode){
			filler = ' ';
			roundup(tm);

			//special case: timer started with 0:00:00 -> treated as 24:00:00 but displayed as 0:00:00
			if(tm.hours == 24 && tm.minutes == 0 && tm.seconds == 0)
				tm.hours = 0;
		}

		oss<<std::setw(2)<<std::setfill(filler)<<tm.hours<<':'<<std::setw(2)<<std::setfill('0')<<tm.minutes;

		if(!editMode){
			oss<<' '<<std::setw(2)<<std::setfill('0')<<tm.seconds;
			sm->getDisplayManager().blinkLargeDigitText(' ', 2, 1);
		}

		sm->getDisplayManager().setLargeDigitText(oss.str());
	}

	void updateDotMatrix(void){
		if(autoRepeat)
			sm->getDisplayManager().setDotMatrixText("TIMERg");
		else
			sm->getDisplayManager().setDotMatrixText("TIMER");
	}

	std::chrono::nanoseconds period;
	std::chrono::nanoseconds remaining;
	std::chrono::time_point<std::chrono::steady_clock> endTime;

	bool autoRepeat = false;
	bool started = false;

};

#endif /* SRC_APP_CASIO676_CASIOAPPS_TIMERAPP_HPP_ */
