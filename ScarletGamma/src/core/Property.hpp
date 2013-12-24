#pragma once

#include <string>
#include <unordered_map>
#include "ObjectList.hpp"
#include <jofilelib.hpp>

namespace Core {

/// \brief Any property of any object is ether a string or a list of objects.
class Property
{
public:
	/// \brief Create a name value pair property. This cannot be changed later.
	Property( ObjectID _parent, const std::string& _name, const std::string& _value );

	/// \brief Create a named object list property.
	/// \details It is possible to add a standard string property additional
	///		to the object list with SetValue.
	Property( ObjectID _parent, const std::string& _name, const ObjectList& _list );

	/// \brief Deserialize an object.
	/// \param [in] _node A serialized object node.
	Property( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node );

	/// \brief Access the internal object list.
	/// \throws Exception::NoObjectList.
	const ObjectList& GetObjects();

	bool IsObjectList() const	{ return m_isObjectList; }

	/// \brief Add a new object to the object list.
	/// \details \see ObjectList::Add
	///		Sends a message through the network.
	/// \throws Exception::NoObjectList.
	void AddObject( ObjectID _id );

	/// \brief Remove an object from the object list.
	/// \details \see ObjectList::Remove
	///		Sends a message through the network.
	/// \throws Exception::NoObjectList.
	void RemoveObject( ObjectID _id );

	/// \brief Remove all objects from the list.
	/// \throws Exception::NoObjectList.
	void ClearObjects();

	/// \brief Interprets the value as formula/number and returns the result
	///		after evaluation.
	/// \throws Exception::NotEvaluateable
	float Evaluate() const;

	/// \brief Read access to the value string
	const std::string& Value() const;

	/// \brief Write a new value into this property.
	/// \details It is even allowed to do so if this is an object list property.
	///		Sends a message through the network.
	void SetValue( const std::string& _new );

	const std::string& Name() const	{ return m_name; }

	/// \brief Write the content of this object to a meta-file.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const;

	ObjectID ParentObject() const		{ return m_parent; }
private:
	std::string m_name;
	ObjectID m_parent;		///< The object to which this property belongs

	bool m_isObjectList;	///< True if the object list is defined

	// union {... is not allowed in this context because both are complex objects
	std::string m_value;	///< The value string - might contain: text, number, formula, ...
	ObjectList m_objects;	///< List of object references. Not always available.
};


/// \brief A dynamic list of properties.
/// \details This is a custom container which contains Properties. Each
///		key (name) can only be used once. Names are used case insensitive.
class PropertyList
{
public:
	/// \brief Normal standard constructor.
	PropertyList()	{}

	/// \brief Deserialize an object.
	/// \param [in] _parent Required to access object fast from property.
	/// \param [in] _node A serialized object node.
	PropertyList( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node );

	/// \brief Adds a copy of a property to the end of this list.
	/// \param [in] _property Some property object from another list or a new one.
	void Add( const Property& _property );

	/// \brief Removes the property with given name from the list.
	/// \param [in] _name Name of one property in this list (case insensitive).
	///		If the given object is not part of this container nothing happens.
	void Remove( const std::string& _name );

	/// \brief Returns the first occurrence of a property with a matching name.
	/// \param [in] _name A full name of a property to search. The case is ignored.
	/// \return The property or a nullptr if the element does not exists.
	Property* Get( const std::string& _name );

	/// \brief Read only method of Get().
	const Property* Get( const std::string& _name ) const;

	/// \brief Remove all properties from the list.
	void Clear();

	/// \brief Returns the counter for the size of this list.
	int GetNumElements() const		{ return (int)m_map.size(); }

	/// \brief Search all properties which have a certain text sequence in
	///		there name.
	/// \details This method is case insensitive.
	/// \return An array with read access to all found properties.
	std::vector<const Property*> FilterByName( const std::string& _text ) const;

	/// \brief Search all properties which have a certain text sequence in
	///		there value.
	/// \details This method is case insensitive.
	/// \return An array with read access to all found properties.
	std::vector<const Property*> FilterByValue( const std::string& _text ) const;

	/// \brief Write the content of this object to a meta-file.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	virtual void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const;
private:
	std::unordered_map<std::string, Property> m_map;
};

} // namespace Core