#pragma once

namespace Utils {

	/// \brief Linear interpolation.
	template<typename T> T Lerp( T a, T b, float f )	{ return a+(b-a)*f; }

	/// \brief Polynomial spline: smootherstep.
	inline float Smooth( float f )					{ return f*f*f*(f*(f*6.0f-15.0f)+10.0f); }

} // namespace Utils