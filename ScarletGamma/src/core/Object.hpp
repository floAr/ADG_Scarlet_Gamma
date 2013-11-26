#pragma once

#include "Property.hpp"

namespace Core {

/// \brief Representation of nearly everything in this game.
class Object
{
public:
	/// \brief Create an object with the required standard properties
	/// \param [in] _x X coordinate of the object position. The integral part
	///		defines the location within the tile-grid.
	/// \param [in] _y Y coordinate of the object position. The integral part
	///		defines the location within the tile-grid.
	///	\param [in] _sprite Name of a texture file which is used for the
	///		rendering.
	Object(float _x, float _y, const std::string& _sprite);

	/// \brief Deserialize an object.
	/// \param [in] _parent A serialized object node.
	Object(const Jo::Files::MetaFileWrapper::Node& _parent);

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
	/// \param [inout] _parent A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	void Serialize( Jo::Files::MetaFileWrapper::Node& _parent );
private:
	PropertyList m_properties;
};


} // namespace Core