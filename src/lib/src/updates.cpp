#include "lib/inc/updates.h"


void millisecondUpdate() {
	// Nothing to do here yet
}


void fastUpdate() {
	servo::setAngle(0, MAP(-1200, 1200, 0, 0xff, lsm303::getAccelerationY()));
	servo::setAngle(0, MAP(-1200, 1200, 0, 0xff, lsm303::getAccelerationX()));
}


void slowUpdate() {
	// Nothing to do here yet
}

