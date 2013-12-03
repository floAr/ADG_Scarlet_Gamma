#pragma once

#include <Prerequisites.hpp>
#include <cstdint>

namespace Utils {

	/// \brief A pseudo random number generator.
	/// \details The random number generator objects are independent of each
	///		other. This allows deterministic sequences even in a multi-threaded
	///		application.
	///
	class Random {
	public:
		/// \brief Create a new independent random number generator with a seed.
		Random( uint32_t _seed );

		/// \brief Create the next uniform distributed sample in the given
		///		range (inclusive both boundaries). Default is [0,1].
		float Uniform( float _min = 0.0f, float _max = 1.0f );

		/// \brief Create the next uniform distributed sample in the given
		///		integer range (inclusive both boundaries).
		int32_t Uniform( int32_t _min, int32_t _max );

		/// \brief Create the next normal distributed sample with mu=0.
		float Normal( float _variance );

		/// \brief Create a random sample of an exponential distribution.
		float Exponential( float _lambda );

		/// \brief Compute a "random" value at a position in a static white
		///		noise with values in [0,1].
		/// \details Sampling the same position will return the same value.
		///
		///		To create 1D or 2D samples just set the other dimensions to
		///		a constant value.
		/// \warning The white noise space will change if one of the create
		///		next sample functions is called. Do not use the same generator
		///		for static space filling noise and random sampling.
		float At( int32_t _x, int32_t _y, int32_t _z );

		/// \brief Compute an interpolated value in the static white noise
		///		space with values in [0,1].
		/// \details Sampling the same position will return the same value.
		/// \warning The white noise space will change if one of the create
		///		next sample functions is called. Do not use the same generator
		///		for static space filling noise and random sampling.
		float At( float _position );
		float At( const sf::Vector2f& _position );

		/// \brief Compute a spline interpolated value in the static white
		///		noise space with a higher continuous differentiability.
		/// \details Sampling the same position will return the same value.
		/// \warning The white noise space will change if one of the create
		///		next sample functions is called. Do not use the same generator
		///		for static space filling noise and random sampling.
		float AtQ( float _position, int _iy=0, int _iz=0 );
		float AtQ( const sf::Vector2f& _position, int _iz=0 );
	private:
		/// \brief The real generator used in all sample functions.
		uint32_t Xorshift128();
		uint32_t m_state[4];	///< Internal state of the generator
	};

} // namespace Utils