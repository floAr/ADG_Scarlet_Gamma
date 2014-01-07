#pragma once

#include "Prerequisites.hpp"
#include "Property.hpp"
#include "SFML\Graphics\Color.hpp"

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

	/// \brief Returns the floating position within the tile grid.
	/// \details The position exists only if the object is located on a map.
	///		Otherwise this method will return a NoSuchProperty exception.
	/// 
	///		This method is relatively slow because it contains a string->float
	///		cast. So buffer the position if used more than once.
	/// \throws Exception::NoSuchProperty
	sf::Vector2f GetPosition() const;

	/// \brief Returns a color if the according property exists and white
	///		otherwise.
	sf::Color GetColor() const;

	/// \brief Creates the color property if not existing and sets a color
	///		which is multiplied during rendering.
	void SetColor( const sf::Color& _color );

	/// \brief Write the content of this object to a meta-file.
	/// \details Serialization contains the object ids.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	virtual void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const override;

	ObjectID ID() const { return m_id; }

	// The following constants are names of predefined properties.
	// This allows faster accesses because string("keks") would cause an
	// allocation and deallocation which the constants do not.
	static const std::string PROP_LAYER;	///< Name of layer property: rendering order
	static const std::string PROP_X;		///< Name of position.x: floating point position
	static const std::string PROP_Y;		///< Name of position.y: floating point position
	static const std::string PROP_SPRITE;	///< Name of sprite property: texture name
	static const std::string PROP_COLOR;	///< Name of color property: hexadecimal string
	static const std::string PROP_PATH;		///< Name of path property: object list + boolean value
	static const std::string PROP_TARGET;	///< Name of target property: next point to be reached linearly
	static const std::string PROP_OBSTACLE;	///< Name of obstacle property: this object collides with the player
	static const std::string PROP_NAME;		///< Name of name property: object's name
	static const std::string PROP_PLAYER;	///< Name of player: flags an object as player

	/// \brief Add a new way point to the path list.
	/// \details If the new object is equal to the first in the list the loop
	///		property is set to true. Otherwise it is false.
	///	\param [in] _wayPoint Any object which should be tracked or reached.
	void AppendToPath( ObjectID _wayPoint );

	/// \brief Is this object directly located on some map?
	bool IsLocatedOnAMap() const	{ return m_hasParent && HasProperty(PROP_X); }
	MapID GetParentMap() const		{ return m_parent.map; }
	void SetParentMap(MapID _map)	{ m_parent.map = _map; m_hasParent = true; }
private:
	// Hide some of the methods
	PropertyList::Clear;
	PropertyList::Get;
	PropertyList::GetNumElements;

	ObjectID m_id;	///< Unique identification number for this object.

	union {
		MapID map;			///< On which map is this object?
		ObjectID object;	///< In whose object list it is?
	} m_parent;
	bool m_hasParent;

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
};


} // namespace Core