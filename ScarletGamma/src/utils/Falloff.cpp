#include "utils/Falloff.hpp"
#include <cmath>

Utils::Falloff::Falloff(FalloffType type, float modVal, float epsilon)
	: m_type(type), m_timer(0), m_modVal(modVal), m_epsilon(epsilon), mFRAMETIME(1/60.f), m_value(0)
{
}

void Utils::Falloff::Update(float dt)
{
	if (abs(m_value) <= m_epsilon)
	{
		m_value = 0;
	}
	else
	{
		m_timer += dt;

		if (m_timer >= mFRAMETIME)
		{
			// Set timer back
			m_timer -= mFRAMETIME;

			// Update value
			switch (m_type)
			{
			case Utils::Falloff::FT_LINEAR:
				m_value -= m_modVal;
				break;
			case Utils::Falloff::FT_QUADRATIC:
				m_value *= m_modVal;
				break;
			}
		}
	}
}
