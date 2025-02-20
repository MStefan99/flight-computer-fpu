// Adafruit Mahony filter implementation
// https://github.com/adafruit/Adafruit_AHRS
//
//=============================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=============================================================================================

#ifndef MAHONY_HPP
#define MAHONY_HPP

#include "Quaternion.hpp"
#include "util.hpp"

class Mahony {
public:
	constexpr static float defaultKp {0.5f};
	constexpr static float defaultKi {0.5f};

	Mahony(float Kp = defaultKp, float Ki = defaultKi);

	void update(Vector3<float, uint8_t> rot, Vector3<float, uint8_t> acc, Vector3<float, uint8_t> mag, float dt);
	void updateIMU(Vector3<float, uint8_t> rot, Vector3<float, uint8_t> acc, float dt);

	float getKp();
	float getKi();
	void  setKp(float Kp);
	void  setKi(float Ki);

	Quaternion getQuaternion() const;
	void       setQuaternion(const Quaternion& quat);

protected:
	float      _twoKp {};  // 2 * proportional gain (Kp)
	float      _twoKi {};  // 2 * integral gain (Ki)
	Quaternion _quat {};
	// Integral error terms scaled by Ki
	float      _integralFBx {};
	float      _integralFBy {};
	float      _integralFBz {};
};

#endif /* MAHONY_HPP */
