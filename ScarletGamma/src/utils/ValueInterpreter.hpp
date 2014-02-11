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
	///		- VAL := int | int<W>int | <W>int | int<w>int | <w>int | 'PropertyName'
	///		
	///		The operator precedence is: +,- < *,/ < -(unary) < ()
	///		
	///		In case of 'PropertyName' the evaluation is applied recursively.
	///	\param [in] _formula An string containing a valid formula F.
	///	\param [in] _generator The values with a <W/w> are dices. An expression
	///		like W8 creates a random number in [1,8].
	///	\param [in] _object The object from which properties are read.
	///	\throws Exception::InvalidFormula
	int EvaluateFormula( const std::string& _formula, Random* _generator, const Core::Object* _object );

} // namespace Utils