#ifndef SRC_APP_CASIO676_CASIOAPPS_WATCHAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_WATCHAPP_HPP_

#include "casioapp.hpp"

class WatchApp:public CasioApp{
public:
	WatchApp(CasioStateMachine* sm):CasioApp(sm){
		updateTime();
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
		updateTime();
	}

private:
	void updateTime(void){
		time_t now;
		struct tm info;

		time(&now);
		localtime_r(&now, &info);

		std::ostringstream oss;
		oss<<std::setw(2)<<info.tm_hour<<':'<<std::setw(2)<<std::setfill('0')<<info.tm_min<<' '<<std::setw(2)<<info.tm_sec;
		sm->getDisplayManager().setLargeDigitText(oss.str());

		oss.clear();
		oss.str("");
		oss<<std::setw(2)<<std::setfill(' ')<<info.tm_mon+1<<'-'<<std::setw(2)<<info.tm_mday;
		sm->getDisplayManager().setSmallDigitText(oss.str());

		oss.clear();
		oss.str("");
		oss<<DAYS[info.tm_wday]<<"'"<<info.tm_year-100;
		sm->getDisplayManager().setDotMatrixText(oss.str());
	}
};

#endif /* SRC_APP_CASIO676_CASIOAPPS_WATCHAPP_HPP_ */
