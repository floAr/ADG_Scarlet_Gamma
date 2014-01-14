#pragma once

namespace Utils {

	/// \brief Linear interpolation.
	template<typename T> T Lerp( T a, T b, float f )	{ return a+(b-a)*f; }

	/// \brief Polynomial spline: smootherstep.
	inline float Smooth( float f )					{ return f*f*f*(f*(f*6.0f-15.0f)+10.0f); }

	template<typename T> T Sign( T a )				{ return (T)(a<0 ? -1 : (a>0 ? 1 : 0)); }

} // namespace Utils