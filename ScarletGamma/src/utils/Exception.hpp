#pragma once

#include <string>

namespace Exception {

	/// \brief Thrown if something cannot be interpreted as number or formula but
	///		is expected.
	class NotEvaluateable {};

	/// \brief Thrown if something is expected to contain a (nonempty) object list.
	class NoObjectList {};

	/// \brief Thrown if the existence of a property was expected but is not there.
	class NoSuchProperty {};

    /// \brief Thrown if the existence of an action was expected but is not there.
	class NoSuchAction {};

	/// \brief Thrown if an expression cannot be evaluated.
	class InvalidFormula {
	public:
		InvalidFormula( const std::string& _msg ) : 
			m_msg( _msg )
		{}

		/// \brief Generate the message.
		std::string to_string() const	{ return m_msg; }
	private:
		std::string m_msg;
	};

} // namespace Exception