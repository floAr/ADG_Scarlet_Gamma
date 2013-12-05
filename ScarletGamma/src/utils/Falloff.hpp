#pragma once

namespace Utils
{
	/// \brief Helper class that represents a value that decreases over time
	class Falloff
	{
	public:
		/// \brief Type of the falloff curve
		enum FalloffType
		{
			FT_LINEAR,
			FT_QUADRATIC
		};

		/// \brief Constructs a new Falloff with the given typen and half-life.
		/// \param [in] type      Sets the type of the falloff.
		/// \param [in] modVal    The modification value that is applied in each frame.
		/// \param [in] epsilon   Treshold below with the value will be zeroed.
		Falloff(FalloffType type, float modVal, float epsilon);

		/// \brief Updates the internal timers of the Falloff.
		/// \param [in] dt  Time since last update in seconds.
		void Update(float dt);

		/// \brief Set the current value of the falloff. Resets the internal timer.
		float operator=(float value)
		{
			m_value = value;
			m_timer = 0;
			return value;
		}

		/// \brief Get the current value of the falloff.
		operator float()
		{
			return m_value;
		}

	private:
		const float mFRAMETIME; //< The time of one internal frame.
		float m_value; //< Current value of the falloff
		FalloffType m_type; //< Type of the falloff
		float m_epsilon; //< Treshold below with the value will be zeroed
		float m_modVal; //< The modifier that is applied each frame, according to m_type
		float m_timer; //< An internal timer to be used by the update function
	};
}

