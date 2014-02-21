#include "ValueInterpreter.hpp"
#include "Exception.hpp"
#include "Random.hpp"
#include <cctype>
#include <algorithm>
#include <list>
#include <vector>
#include <cassert>
#include "core\Object.hpp"

namespace Utils {

	static bool IsValue( char _c )
	{
		return _c == 'w' || _c == 'W' || isdigit(_c);
	}

	static bool IsOperator( char _c )
	{
		return _c == '-' || _c == '+' || _c == '*' || _c == '/';
	}

	static bool IsFormula( char _c )
	{
		return _c == '(' || IsOperator(_c) || IsValue(_c);
	}

	static bool IsLimiter( char _c )
	{
		return IsOperator(_c) || _c == '(' || _c == ')' || _c == '\'';
	}

	// Handle operator precedence
	static int Priority( char _op )
	{
		switch(_op)
		{
			case '(': return 0;
			case 'n': return 4;
			case '*': return 3;
			case '/': return 3;
			case '+': return 2;
			case '-': return 2;
		}
		assert(false);
		return -1;
	}

	// Information of a terminal token from parsing
	struct Token
	{
		int value;
		bool evaluated;	// Is value a character [(,),+,-,*,/,n] or an result?

		Token(int _value, bool _evaluated) : value(_value), evaluated(_evaluated) {}
	};

	// Parse a pure value string
	static Token EvaluateValue( const std::string& _value, Random* _generator, const Core::Object* _object )
	{
		if( _value.find_last_not_of( "0123456789wW" ) != std::string::npos )
		{
			// It is a property - use recursive evaluation
			if( !_object ) throw Exception::InvalidFormula("Formel referenziert eine Eigenschaft, aber kein Objekt gegeben.");
			if( !_object->HasProperty(_value) ) throw Exception::InvalidFormula("Formel referenziert die Eigenschaft " + _value + ", welche nicht Teil des Objektes " + _object->GetName() + " ist.");
			return Token( _object->GetProperty( _value ).Evaluate(_object), true );
		}

		try {
			// Iterate and look for a 'w'. The buffer only contains integers before
			// or after the w.
			size_t pos = _value.find_first_of( "wW" );
			if( pos == std::string::npos )
			{
				return Token( std::stoi(_value), true );
			} else {
				// Split into two integers and generate a random value from the second.
				int mult = (pos==0) ? 1 : std::stoi(_value.substr( 0, pos ));
				int dice = std::stoi(_value.substr( pos+1 ));
				return Token( mult * _generator->Uniform(1,dice), true );
			}
		} catch( ... ) {
			throw Exception::InvalidFormula( "Falsches Zeichen in Wert: " + _value );
		}
	}

	// Calculate the result of one special operation.
	static void Calculate( std::vector<int>& _valueStack, char _op )
	{
		int rhs = _valueStack.back();
		_valueStack.pop_back();
		if( _op == 'n' ) { _valueStack.push_back( -rhs ); return; }
		int lhs = _valueStack.back();
		_valueStack.pop_back();
		switch( _op )
		{
			case '+': _valueStack.push_back( lhs + rhs ); break;
			case '-': _valueStack.push_back( lhs - rhs ); break;
			case '*': _valueStack.push_back( lhs * rhs ); break;
			case '/': _valueStack.push_back( (int)floor(float(lhs) / rhs) ); break;
		}
	}

	static int EvaluateFormula( const std::list<Token>& _tokens )
	{
		std::vector<int> valueStack;
		std::vector<char> operatorStack;
		bool awaitsOperator = false;	// Find unary - awaitsOperator==false and current token == -
		for( auto it = _tokens.begin(); it != _tokens.end(); ++it )
		{
			if( it->evaluated )
			{
				valueStack.push_back(it->value);
				awaitsOperator = true;
			} else {
				if( it->value == '(' )
				{
					// Put bracket on operator stack to block operations beyond this point
					operatorStack.push_back(it->value);
					awaitsOperator = false;
				} else if( it->value == ')' ) {
					// Evaluate sub expression
					while( !operatorStack.empty() && operatorStack.back() != '(' )
					{
						Calculate( valueStack, operatorStack.back() );
						operatorStack.pop_back();
					}
					if(operatorStack.empty()) throw Exception::InvalidFormula( "Mehr schließende als öffnende Klammern gefunden." );
					operatorStack.pop_back();
					awaitsOperator = true;
				} else if( IsOperator(it->value) ) {
					if( !awaitsOperator )
					{
						if( it->value != '+' )	// In case of additional + just skip
						{
							if( it->value != '-' ) throw Exception::InvalidFormula( "Vorzeichen - oder + erwartet, gefunden: " + it->value );
							// Current operator is unary minus
							operatorStack.push_back('n');
						}
					} else {
						// As long as the stack operator has a higher priority
						// evaluate it first.
						while( !operatorStack.empty() && Priority(operatorStack.back()) > Priority(it->value) )
						{
							Calculate( valueStack, operatorStack.back() );
							operatorStack.pop_back();
						}
						operatorStack.push_back(it->value);
					}
					awaitsOperator = false;
				} else throw Exception::InvalidFormula( "Unerwartetes Zeichen gefunden: " + it->value );
			}
		}

		// Evaluate last things on stack.
		while( !operatorStack.empty() )
		{
			if( operatorStack.back() == '(' ) throw Exception::InvalidFormula( "Mehr öffnende als schließende Klammern gefunden." );
			Calculate( valueStack, operatorStack.back() );
			operatorStack.pop_back();
		}

		return valueStack.front();
	}

	int EvaluateFormula( const std::string& _formula, Random* _generator, const Core::Object* _object )
	{
		// Remove whitespaces
		//_formula.erase( std::remove_if( _formula.begin(), _formula.end(), ::isspace ), _formula.end() );

		// Build a token sequence
		std::list<Token> tokens;
		std::string buffer = "";
		for( auto it = _formula.begin(); it != _formula.end(); ++it )
		{
			if( !isspace(*it) )
			{
				if( *it == '\'' )
				{
					if(!buffer.empty())	tokens.push_back( EvaluateValue(buffer, _generator, _object) );
					buffer = "";
					// Buffer the full name and do not interpret inside names
					++it;
					while(it != _formula.end() && *it != '\'')
					{
						buffer += *it;
						++it;
					}
					tokens.push_back( EvaluateValue(buffer, _generator, _object) );
					buffer = "";
				} else if( IsLimiter(*it) )
				{
					if(!buffer.empty())	tokens.push_back( EvaluateValue(buffer, _generator, _object) );
					tokens.push_back( Token(*it, false) );
					buffer = "";
				} else
					buffer += *it;
			}
		}
		if(!buffer.empty()) tokens.push_back( EvaluateValue(buffer, _generator, _object) );
	 
		// Validate
		if( tokens.empty() ) throw Exception::InvalidFormula( "Formel ist leer." );

		// Call recursive
		return EvaluateFormula( tokens );
	}

} // namespace Utils