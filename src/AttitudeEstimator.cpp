//
// Created by MStefan99 on 21.3.22.
//

#include "AttitudeEstimator.hpp"

constexpr static float ALPHA {0.1f};

AttitudeEstimator::AttitudeEstimator(float roll, float pitch):
  _roll {roll},
  _pitch {pitch} {
	// Nothing to do
}

void AttitudeEstimator::update(const float* rot, const float* acc, float dt) {
	_pitch += rot[0] * dt * F_DEG_TO_RAD;
	_roll += rot[2] * dt * F_DEG_TO_RAD;

	float pitchAcc = atan2f(acc[1], -acc[2]);
	_pitch = std::fmod(_pitch * (1 - ALPHA) - pitchAcc * ALPHA, F_PI);

	float rollAcc = atan2f(-acc[1], acc[0]);
	_roll = std::fmod(_roll * (1 - ALPHA) + rollAcc * ALPHA, F_PI);
}

float AttitudeEstimator::getRoll() const {
	return _roll;
}

float AttitudeEstimator::getPitch() const {
	return _pitch;
}
