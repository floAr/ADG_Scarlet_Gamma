#pragma once

namespace Exception {

	/// \brief Thrown if something cannot be interpreted as number or formular but
	///		is expected.
	class NotEvaluateable {};

	/// \brief Thrown if something is expeced to contain a (nonempty) object list.
	class NoObjectList {};

} // namespace Exception