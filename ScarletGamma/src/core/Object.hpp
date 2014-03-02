#pragma once

#include "Prerequisites.hpp"
#include "Property.hpp"
#include "SFML\Graphics\Color.hpp"
#include "Constants.hpp"

namespace Core {

/// \brief Representation of nearly everything in this game.
class Object: public PropertyList
{
public:
	/// \brief C++11 move construction (auto generated)
	//Object(Object&& _object);

	/// \brief Returns the first occurrence of a property with a matching name.
	/// \details This method allows read and write access.
	///
	///		A name matches without case sensitivity.
	/// \throws Exception::NoSuchProperty
	Property& GetProperty( const std::string& _name );

	/// \brief Read access only (see GetProperty() for more details)
	const Property& GetProperty( const std::string& _name ) const;

	/// \brief Checks if at least one property with that name exists.
	bool HasProperty( const std::string& _name ) const;

	/// \brief Adds a copy of a property to the end of this list.
	/// \param [in] _property Some property object from another list or a new one.
	/// \return An editable reference to the property inside the object
	Property& Add( const Property& _property )	{ Property& ref = Add(_property, m_id); OnPropertyAdd(ref.Name()); return ref; }

	/// \brief Remove a property from this object.
	void Remove( const std::string& _name )		{ PropertyList::Remove(_name); OnPropertyRemove(_name); }

	/// \brief A saver variant to change object properties. If something depends
	///		on the change it is modified too.
	///	\throws Exception::NoSuchProperty
	void SetPropertyValue( const std::string& _name, const std::string& _value );

	/// \brief Returns the floating position within the tile grid.
	/// \details The position exists only if the object is located on a map.
	///		Otherwise this method will return a NoSuchProperty exception.
	/// 
	///		This method is relatively slow because it contains a string->float
	///		cast. So buffer the position if used more than once.
	/// \throws Exception::NoSuchProperty
	sf::Vector2f GetPosition() const;

	/// \brief Return the integer index of the layer where the object is placed.
	/// \details The layer only exits if the object is placed on a map.
	/// \throws Exception::NoSuchProperty
	int GetLayer() const	{ return atoi(GetProperty(STR_PROP_LAYER).Value().c_str()); }

	/// \brief Returns the value of the name property and the id as string
	///		if there is no such property.
	std::string GetName() const;

	/// \brief Returns a color if the according property exists and white
	///		otherwise.
	sf::Color GetColor() const;

	/// \brief Returns a color with full opaqueness if the according property
	///		exists and white otherwise.
	sf::Color GetColorOpaque() const;

	/// \brief Creates the color property if not existing and sets a color
	///		which is multiplied during rendering.
	void SetColor( const sf::Color& _color );

	/// \brief Write the content of this object to a meta-file.
	/// \details Serialization contains the object ids.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	virtual void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const override;

	ObjectID ID() const { return m_id; }

	/// \brief Add a new way point to the path list.
	/// \details If the new object is equal to the first in the list the loop
	///		property is set to true. Otherwise it is false.
	///	\param [in] _wayPoint Any object which should be tracked or reached.
	void AppendToPath( ObjectID _wayPoint );

	/// \brief Is this object directly located on some map?
	bool IsLocatedOnAMap() const;
	MapID GetParentMap() const		{ return m_parent.map; }
	void SetParentMap(MapID _map)	{ m_parent.map = _map; m_hasParent = true; }

	/// \brief Is this object being updated?
	bool IsActive() const;

	/// \brief Clear the path and target of active objects.
	void ResetTarget();
private:
	// Hide some of the methods
	PropertyList::Clear;
	PropertyList::Get;
	PropertyList::Add;
	PropertyList::Remove;

	ObjectID m_id;	///< Unique identification number for this object.

	union {
		MapID map;			///< On which map is this object?
		ObjectID object;	///< In whose object list it is?
	} m_parent;
	bool m_hasParent;		///< m_parent is defined. Which one is used depends on the existence of the "X"-property.

	/// Use private constructors such that only the world can create objects.
	friend class Core::World;
	friend void UnitTest::TestObjects();

	/// \brief Create an object with the required standard properties
	/// \param [in] _id Unique id. Once the id is given it should never be
	///		changed or reused.
	///	\param [in] _sprite Name of a texture file which is used for the
	///		rendering.
	Object(ObjectID _id, const std::string& _sprite);

	/// \brief Deserialize an object.
	/// \param [in] _node A serialized object node.
	Object(const Jo::Files::MetaFileWrapper::Node& _node);

	/// \brief This methods get called if a property is edited
	///		and handles special cases.
	///	\return false if the change should not happen.
	bool OnPropertyChanged( const std::string& _name, const std::string& _newValue );

	/// \brief Called after adding to do special work.
	void OnPropertyAdd( const std::string& _name );

	/// \brief Called after deleting to do special work.
	void OnPropertyRemove( const std::string& _name );
};


} // namespace Core