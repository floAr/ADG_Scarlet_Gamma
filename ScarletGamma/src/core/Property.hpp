#pragma once

#include <string>
#include "ObjectList.hpp"

namespace Core {

/// \brief Any property of any object is ether a string or a list of objects.
class Property
{
public:
	/// \brief Create a name value pair property. This cannot be changed later.
	Property( const std::string& _name, const std::string& _value );

	/// \brief Create a named object list property.
	/// \details It is possible to add a standard string property additional
	///		to the object list with SetValue.
	Property( const std::string& _name, const ObjectList& _list );

	/// \brief Access the internal object list.
	/// \throws Exception::NoObjectList.
	ObjectList& Objects();

	/// \brief Interpretes the value as formular/number and returns the result
	///		after evaluation.
	/// \throws Exception::NotEvaluateable
	float Evaluate() const;

	/// \brief Read access to the value string
	const std::string& Value() const;

	/// \brief Write a new value into this property.
	/// \details It is even allowed to do so if this is an object list property.
	void SetValue( const std::string& _new );
private:
	std::string m_name;

	bool m_isObjectList;	///< True if the object list is defined

	// union {... is not allowed in this context because both are complex objects
	std::string m_value;	///< The value string - might contain: text, number, formular, ...
	ObjectList m_objects;	///< List of object references. Not always available.
};

} // namespace Core