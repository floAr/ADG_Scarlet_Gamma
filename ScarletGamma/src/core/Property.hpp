#pragma once

#include <string>
#include <list>
#include "ObjectList.hpp"
#include <jofilelib.hpp>

namespace Core {

/// \brief Any property of any object is ether a string or a list of objects.
class Property
{
public:
	/// \brief Deserialize an object.
	/// \param [in] _node A serialized object node.
	Property( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node );

	/// \brief Access the internal object list.
	/// \throws Exception::NoObjectList.
	const ObjectList& GetObjects() const;

	bool IsObjectList() const	{ return m_objects.Size() > 0; }

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

	/// \see ObjectList::PopFront();
	/// \details Additionally sends a message through the network.
	/// \throws Exception::NoObjectList.
	void PopFront();

	/// \brief Remove all objects from the list.
	/// \throws Exception::NoObjectList.
	void ClearObjects();

	/// \brief Interprets the value as formula/number and returns the result
	///		after evaluation.
	/// \throws Exception::NotEvaluateable
	int Evaluate() const;

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
	void SetParent(ObjectID _id)		{ m_parent = _id; }

	// A list of standard rights to be used in the constructor.
	enum Rights: uint32_t {
		R_SYSTEMONLY = 0x000,
		R_V00000000 = 0x001,	///< Master: See
		R_VC0000000 = 0x003,	///< Master: See, Change
		R_VCE000000 = 0x007,	///< Master: See, Change, Edit
		R_V00V00000 = 0x009,	///< Master: See; Owner: See
		R_V00V00V00 = 0x049,	///< Everybody: See
		R_V0E000000 = 0x005,	///< Master: See, Edit
		R_V0EV00000 = 0x00d,	///< Master: See, Edit; Owner: See
		R_VCEV00000 = 0x00f,	///< Master: See, Change, Edit; Owner: See
		R_VCEV0E000 = 0x02f,	///< Master: See, Change, Edit; Owner: See, Edit
		R_V0EV00V00 = 0x04d,	///< Master: See, Edit; Owner: See; Player: See
		R_VCEV0EV00 = 0x06f,	///< Master: See, Change, Edit; Owner: See, Edit; Player: See
		R_V0EV0EV0E = 0x16d,	///< Everybody: See, Edit
		R_VCEV0EV0E = 0x16f,	///< Master: Change; Everybody: See, Edit
		R_VCEVCEVCE = 0x1ff		///< Full rights for all
	};

	/// \brief Check visible right
	bool CanSee( PlayerID _player ) const;
	/// \brief Check right to rename, add and delete properties
	bool CanChange( PlayerID _player ) const;
	/// \brief Check edit value right
	bool CanEdit( PlayerID _player ) const;

	/// \brief Sets if the player have no rights or player rights
	//void ApplyRights( PlayerID _player, bool _hasAdvancedPlayer );

	/// \brief Resets the 9 rights flags but keep player levels.
	void SetRights( Rights _newRights );

private:
	std::string m_name;
	ObjectID m_parent;		///< The object to which this property belongs

	// union {... is not allowed in this context because both are complex objects
	std::string m_value;	///< The value string - might contain: text, number, formula, ...
	ObjectList m_objects;	///< List of object references. Not always available.

	/// \brief A system of rights who can do what.
	/// \details 3 Bits: Visible, Change (EditName, Add, Delete), EditValue.
	///		Once for the master and twice for players, 
	///		then 1 Bit per player if he has advanced player
	///		rights or basic rights
	uint32_t m_rights;


	/// \brief Create a name value pair property. This cannot be changed later.
	Property( uint32_t _rights, const std::string& _name, const std::string& _value );

	/// \brief Create a named object list property.
	/// \details It is possible to add a standard string property additional
	///		to the object list with SetValue.
	Property( uint32_t _rights, const std::string& _name, const std::string& _value, const ObjectList& _list );

	friend struct PROPERTY;
};


/// \brief A dynamic list of properties.
/// \details This is a custom container which contains Properties. Each
///		key (name) can only be used once.
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
	/// \param [in] _parent Who gets this new property?
	/// \return An editable reference to the property inside the object
	Property& Add( const Property& _property, ObjectID _parent );

	/// \brief Remove a property whose address comes from Get or Filter.
	/// \param [in] _property Reference to one property in this list. If
	///		the given object is not part of this container nothing happens.
	void Remove( Property* _property );

	/// \brief Removes all properties with given name from the list.
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
	int GetNumElements() const				{ return (int)m_list.size(); }

	/// \brief Indexed property read access for iteration purposes
	const Property* At( int _index ) const	{ return &m_list[_index];}

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
	std::vector<Property> m_list;
};

} // namespace Core