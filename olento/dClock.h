#ifndef DCLOCK_H
#define DCLOCK_H

#include <time.h>
#include <SDL2/SDL.h>

class dClock{
public:
	clock_t lastTime;

	void reset() { lastTime = clock(); }

	float get() {
		clock_t time = clock() - lastTime;
		return ((float)time / CLOCKS_PER_SEC);
	}

	bool delay(float target_fps) {
		int delay_ms = (1000 / target_fps) - get() * 1000;
		reset();

		if (delay_ms > 0) {
			if (delay_ms > 1000)
				delay_ms = 1000;
			SDL_Delay(delay_ms);
			return true;
		}

		else return false; //if lag, return false
	}

	dClock(){
		reset();
	}

};

#endif