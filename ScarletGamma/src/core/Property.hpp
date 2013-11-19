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

	// TODO: Serialze, Deserialize
private:
	std::string m_name;

	bool m_isObjectList;	///< True if the object list is defined

	// union {... is not allowed in this context because both are complex objects
	std::string m_value;	///< The value string - might contain: text, number, formular, ...
	ObjectList m_objects;	///< List of object references. Not always available.
};


/// \brief A dynamic list of properties.
/// \details This is a customcontainer which can contain one key
///		multiple times. Currently it is a list and all accesses are slow.
class PropertyList
{
public:
	/// \brief Adds a copy of a property to this list.
	/// \param [in] _property Some property object from another list or a new one.
	void Add( const Property& _property );

	/// \brief Remove a property whichs address comes from Get or GetAll.
	/// \param [in] _property Reference to one property in this list. If
	///		the given object is not part of this container nothing happens.
	void Remove( Property& _property );

	/// \brief Removes all properties sharing a name from the list.
	void Remove( const std::string& _name );

	/// \brief Returns the first occurency of a property with a matching name.
	/// \param [in] _name A full name of a property to search. The case is ignored.
	Property& Get( const std::string& _name );
private:
	struct ListNode {
		Property m_property;
		ListNode* m_next;

		ListNode( const Property& _prop ) : m_property(_prop), m_next(nullptr) {
		}
	};

	ListNode* m_first;	///< Single linked list start.
	ListNode* m_last;	///< Single linked list last node.
};

} // namespace Core