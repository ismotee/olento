#ifndef DCLOCK_H
#define DCLOCK_H

#include <time.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>

class dClock{
public:

	static int getTime_hours() {
		time_t t = time(0);
		struct tm* now = localtime(&t);
		return now->tm_hour;
	}

	static int getTime_minutes() {
		time_t t = time(0);
		struct tm* now = localtime(&t);
		return now->tm_min;
	}

	static int getTime_seconds() {
		time_t t = time(0);
		struct tm* now = localtime(&t);
		return now->tm_sec;
	}

	static std::string getTimeString() {
		std::stringstream ss;
		int hours = getTime_hours();
		int minutes = getTime_minutes();
		int seconds = getTime_seconds();

		if(hours < 10) ss << "0";
		ss << hours << ":";

		if(minutes < 10) ss << "0";
		ss << minutes << ":";

		if(seconds < 10) ss << "0";
		ss << seconds;

		return ss.str();
	}


	clock_t lastTime;

	void reset() { lastTime = clock(); }

	float get() {
		//palauta aika sekunteina
		clock_t time = clock() - lastTime;
		return ((float)time / CLOCKS_PER_SEC);
	}

	void delay_ms(int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms) );
	}

	bool delay(float target_fps) {
		int ms = (1000 / target_fps) - get() * 1000;
		reset();

//		using namespace std::this_thread; // sleep_for, sleep_until
//		using namespace std::chrono; // nanoseconds, system_clock, seconds

		if (ms > 0) {
			//if (ms > 1000)
			//	ms = 1000;
			
			delay_ms(ms);
			return true;
		}

		else return false; //if lag, return false
	}



	dClock(){
		reset();
	}

};

#endif