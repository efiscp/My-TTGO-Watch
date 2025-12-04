#ifndef SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_

#include "casioapp.hpp"

class WtApp:public CasioApp{
public:
	WtApp(CasioStateMachine* sm):CasioApp(sm){}

	void activate(void){
		initialText = true;
		CasioApp::activate();
		sm->getDisplayManager().setDotMatrixText("fWT");
		updateDisplay();
	}

	void processEvent(CasioEvent_t event){
		switch(event){
		case BUTTON_C_PRESSED:
			sm->changeApp(false);
			break;
		case KEY_DIV_MC_REV_PRESSED:
			decIndex();
			initialText = false;
			updateDisplay();
			break;
		case KEY_FWD_PLUS_MPLUS_PRESSED:
			incIndex();
			initialText = false;
			updateDisplay();
			break;
		default:
			break;
		}
	}

	void periodic(void){
		if(initialText){
			if(getElapsedMs() > 1000){
				initialText = false;
			}
		}

		updateDisplay();
	}

	struct TimeZone{

		TimeZone(const char* name, int8_t offset):name(name), offset(offset){}

		const char* getname(void) const {
			return name;
		}

		void getTime(struct tm* info, struct tm* local) const {
			time_t now;

			time(&now);
			gmtime_r(&now, info);
			localtime_r(&now, local);

			info->tm_hour += offset;
			if(info->tm_hour > 23) info->tm_hour -= 24;
			else if(info->tm_hour < 0) info->tm_hour += 24;
		}
	private:
		const char* name;
		int8_t offset;
	};

private:
	bool initialText = true;
	int8_t index = 0;

	void updateDisplay(void) const {

		struct tm info;
		struct tm local;

		timezones[index].getTime(&info, &local);

		std::ostringstream oss;
		oss<<std::setw(2)<<info.tm_hour<<':'<<std::setw(2)<<std::setfill('0')<<info.tm_min<<' '<<std::setw(2)<<info.tm_sec;
		sm->getDisplayManager().setLargeDigitText(oss.str());

		oss.clear();
		oss.str("");
		oss<<std::setw(2)<<local.tm_hour<<':'<<std::setw(2)<<std::setfill('0')<<local.tm_min;
		sm->getDisplayManager().setSmallDigitText(oss.str());

		if(!initialText)
			sm->getDisplayManager().setDotMatrixText(timezones[index].getname());

	}

	void incIndex(void){
		if(++index >= (sizeof(timezones)/sizeof(timezones[0])))
			index = 0;
	}

	void decIndex(void){
		if(--index < 0)
			index = (sizeof(timezones)/sizeof(timezones[0])) - 1;
	}

	static const TimeZone timezones[24];

};

#endif /* SRC_APP_CASIO676_CASIOAPPS_WTAPP_HPP_ */
