#include "wtapp.hpp"

const WtApp::TimeZone WtApp::timezones[24] = {
			TimeZone("LON", 0),
			TimeZone("PAR", 1),
			TimeZone("CAI", 2),
			TimeZone("MOW", 3),
			TimeZone("DXB", 4),
			TimeZone("KHI", 5),
			TimeZone("DAC", 6),
			TimeZone("BKK", 7),
			TimeZone("HKG", 8),
			TimeZone("TYO", 9),
			TimeZone("SYD", 10),
			TimeZone("NOU", 11),
			TimeZone("WLG", 12),
			TimeZone("---", -11),	//PPG
			TimeZone("HNL", -10),
			TimeZone("ANC", -9),
			TimeZone("LAX", -8),
			TimeZone("DEN", -7),
			TimeZone("CHI", -6),
			TimeZone("NYC", -5),
			TimeZone("CCS", -4),
			TimeZone("RIO", -3),
			TimeZone("---", -2),	//RAI
			TimeZone("---", -1),
	};
