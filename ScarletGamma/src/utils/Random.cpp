#include "random.hpp"
#include "MathUtil.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace Utils {

	static uint32_t HashInt( uint32_t _a )
	{
		_a -= _a << 6;
		_a ^= _a >> 17;
		_a -= _a << 9;
		_a ^= _a << 4;
		_a -= _a << 3;
		_a ^= _a << 10;
		_a ^= _a >> 15;
		return _a;
	} 


	// ********************************************************************* //
	Random::Random( uint32_t _seed )
	{
		m_state[0] = 0xaffeaffe ^ _seed;
		m_state[1] = 0xf9b2a750 ^ (_seed + 1);
		m_state[2] = 0x485eac66 ^ (_seed - 1);
		m_state[3] = 0xcbd02714 ^ (_seed + 263083);
	}

	// ********************************************************************* //
	float Random::Uniform( float _min, float _max )
	{
		double scale = (_max - _min) / 0xffffffff;
		return float(_min + scale * Xorshift128());
	}

	// ********************************************************************* //
	int32_t Random::Uniform( int32_t _min, int32_t _max )
	{
		uint32_t interval = uint32_t(_max - _min + 1);
		// Do not use integer maximum bounds!
		assert(interval != 0);

		uint32_t value = Xorshift128();
		return _min + value % interval;
	}

	// ********************************************************************* //
	float Random::Normal( float _variance )
	{
		// Make several turns for a good quality (addition of two standard
		// normal distributions is again a standard normal distribution).
		double n = 0.0;
		for( int i=0; i<3; ++i )
		{
			// Box-Muller method
			float u1 = Uniform();
			float u2 = Uniform();
			double r;
			if( u1 <= 0.0f ) r = 1487.0;	// -2*log(e-323) == 1487.493849482
			else if( u1 >= 1.0f ) continue;
			else r = sqrt(-2.0*log(u1));
			// Use both independent samples to generate one sample of a
			// higher quality.
			n += r*cos(6.28318530718*u2) + r*sin(6.28318530718*u2);
		}
		return float(sqrt(_variance / 6.0) * n);
	}

	// ********************************************************************* //
	float Random::Exponential( float _lambda )
	{
		double n = 0.0;
		for( int i=0; i<5; ++i )
		{
			float u = Uniform();
			if( u <= 0.0f ) n += 743.7;	// -log(e-323) == 743.746924741
			else if( u >= 1.0f ) continue;
			n += -log(u);
		}
		return float(n / (_lambda * 5.0));
	}

	// ********************************************************************* //
	float Random::At( int32_t _x, int32_t _y, int32_t _z )
	{
		// Use states of xorshift to apply seeding
		_x ^= m_state[0]; _y ^= m_state[1]; _z ^= m_state[2];

		uint32_t value = HashInt(_x) ^ HashInt(_y) ^ HashInt(_z);
		return value * 2.328306437e-10f;
	}

	// ********************************************************************* //
	float Random::At( float _position )
	{
		int ix = (int)floor(_position);	float fx = Smooth(_position - ix);

		float s0 = (float)At(ix,   0, 0);
		float s1 = (float)At(ix+1, 0, 0);

		return Lerp( s0, s1, fx );
	}

	// ********************************************************************* //
	float Random::At( const sf::Vector2f& _position )
	{
		int ix = (int)floor(_position.x);		float fx = Smooth(_position.x - ix);
		int iy = (int)floor(_position.y);		float fy = Smooth(_position.y - iy);

		float s00 = (float)At(ix,   iy,   0);
		float s10 = (float)At(ix+1, iy,   0);
		float s01 = (float)At(ix,   iy+1, 0);
		float s11 = (float)At(ix+1, iy+1, 0);

		return Lerp(Lerp( s00, s10, fx ),
			        Lerp( s01, s11, fx ), fy);
	}

	// ********************************************************************* //
	float Random::AtQ( float _position, int _iy, int _iz )
	{
		int ix = (int)floor(_position);	float fx = _position - ix;

		float s0 = At(ix,   _iy, _iz);
		float s1 = At(ix+1, _iy, _iz);
		float s2 = At(ix+2, _iy, _iz);
		float s3 = At(ix+3, _iy, _iz);

		// the 1/2 is moved to the very out side
		float h0 = fx*((2-fx)*fx-1);
		float h1 = (fx*fx*(3*fx-5)+2);
		float h2 = fx*((4-3*fx)*fx+1);
		float h3 = (fx-1)*fx*fx;

		return 0.5f * ( s0*h0 + s1*h1 + s2*h2 + s3*h3 );
	}

	// ********************************************************************* //
	float Random::AtQ( const sf::Vector2f& _position, int _iz )
	{
		int iy = (int)floor(_position.y);	float fy = _position.y - iy;

		float s0 = AtQ(_position.x, iy,   _iz);
		float s1 = AtQ(_position.x, iy+1, _iz);
		float s2 = AtQ(_position.x, iy+2, _iz);
		float s3 = AtQ(_position.x, iy+3, _iz);

		// the 1/2 is moved to the very out side
		float h0 = fy*((2-fy)*fy-1);
		float h1 = (fy*fy*(3*fy-5)+2);
		float h2 = fy*((4-3*fy)*fy+1);
		float h3 = (fy-1)*fy*fy;

		return 0.5f * ( s0*h0 + s1*h1 + s2*h2 + s3*h3 );
	}


	
	// ********************************************************************* //
	uint32_t Random::Xorshift128()
	{
		uint32_t tmp = m_state[0] ^ (m_state[0] << 11);
		m_state[0] = m_state[1];
		m_state[1] = m_state[2];
		m_state[2] = m_state[3];
		m_state[3] ^= (m_state[3] >> 19) ^ tmp ^ (tmp >> 8);
 
		return m_state[3];
	}
};