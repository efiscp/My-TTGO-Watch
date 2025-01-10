#ifndef SRC_APP_CASIO676_CASIOAPPS_CASIOAPP_HPP_
#define SRC_APP_CASIO676_CASIOAPPS_CASIOAPP_HPP_

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "../casioStateMachine.hpp"

class CasioApp{
public:
	CasioApp(CasioStateMachine* sm):sm(sm){}

	virtual void activate(void) {
		activationTime = std::chrono::steady_clock::now();
	}

	virtual void deactivate(void){}
	virtual void processEvent(CasioEvent_t event) = 0;
	virtual void periodic(void){}

	virtual ~CasioApp(void){}
protected:

	struct Timeinfo{
		int hours;
		int minutes;
		int seconds;
		int milliseconds;
	};

	int64_t getElapsedMs(void) const{
		return getElapsedMs(activationTime);
	}

	int64_t getElapsedMs(const std::chrono::time_point<std::chrono::steady_clock>& ref) const{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - ref);

		return elapsed.count();
	}

	std::chrono::nanoseconds getElapsed(Timeinfo& info, const std::chrono::time_point<std::chrono::steady_clock>& ref,
		const std::chrono::nanoseconds& accumulator) const{
		auto now = std::chrono::steady_clock::now();
		auto duration = now - ref + accumulator;

		convertDuration(info, duration);

		return now-ref;
	}

	std::chrono::nanoseconds getElapsed(const std::chrono::time_point<std::chrono::steady_clock>& ref) const{
		return std::chrono::steady_clock::now() - ref;
	}

	void convertDuration(Timeinfo& info, const std::chrono::nanoseconds& duration) const{
		info.hours= std::chrono::duration_cast<std::chrono::hours>(duration).count();
		info.minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count() - info.hours * 60;
		info.seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count() - info.hours * 3600 - info.minutes * 60;
		info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() - info.hours * 3600000 - info.minutes * 60000 - info.seconds * 1000;

	}

	CasioStateMachine* sm;

	std::chrono::time_point<std::chrono::steady_clock> activationTime = std::chrono::steady_clock::now();

	static constexpr char DAYS[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
};

constexpr char CasioApp::DAYS[7][4];



#endif /* SRC_APP_CASIO676_CASIOAPPS_CASIOAPP_HPP_ */
