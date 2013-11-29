#pragma once

#include "Property.hpp"

namespace Core {

/// \brief Representation of nearly everything in this game.
class Object: public PropertyList
{
public:
	/// \brief Create an object with the required standard properties
	/// \param [in] _id Unique id. Once the id is given it should never be
	///		changed or reused.
	///	\param [in] _sprite Name of a texture file which is used for the
	///		rendering.
	Object(ObjectID _id, const std::string& _sprite);

	/// \brief C++11 move construction (auto generated)
	//Object(Object&& _object);

	/// \brief Deserialize an object.
	/// \param [in] _node A serialized object node.
	Object(const Jo::Files::MetaFileWrapper::Node& _node);

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

	/// \brief Updates the two position properties
	/// \param [in] _x X coordinate of the object position. The integral part
	///		defines the location within the tile-grid.
	/// \param [in] _y Y coordinate of the object position. The integral part
	///		defines the location within the tile-grid.
	void SetPosition( float _x, float _y );

	/// \brief Write the content of this object to a meta-file.
	/// \details Serialization contains the object ids.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	virtual void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) override;

	ObjectID ID() const { return m_id; }
private:
	// Hide some of the methods
	PropertyList::Clear;
	PropertyList::Get;
	PropertyList::GetNumElements;

	ObjectID m_id;	///< Unique identification number for this object.
};


} // namespace Core