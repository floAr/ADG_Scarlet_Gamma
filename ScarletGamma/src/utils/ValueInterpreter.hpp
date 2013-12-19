#pragma once

#include <string>
#include "Prerequisites.hpp"

namespace Utils {

	/// \brief Evaluate a simple formula as 1W6+3.
	/// \details Following grammar is allowed. Upper case words are
	///		non-terminals and lower case are primitive types. The '|' is the
	///		'or' which defines alternatives. A whitespace is a symbol for
	///		concatenation of multiply symbols. Terminals are set in <>.
	///		
	///		- F := VAL | F OP F | <-> F | (F)
	///		- OP := <+> | <-> | <*> | </>
	///		- VAL := int | int<W>int | <W>int | int<w>int | <w>int
	///		
	///		The operator precedence is: +,- < *,/ < -(unary) < ()
	///	\param [in] _formula An string containing a valid formula F.
	///	\param [in] _generator The values with a <W/w> are dices. An expression
	///		like W8 creates a random number in [1,8].
	///	\throws Exception::InvalidFormula
	int EvaluateFormula( std::string _formula, Random* _generator );

} // namespace Utils