#pragma once

#include "property.hpp"

namespace Core {

/// \brief Representation of nearly everything in this game.
class Object
{
public:
	/// \brief Returns the first occurency of a property with a matching name.
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
	/// \details This method is not case sensitive.
	/// \return An array with read access to all found properties.
	std::vector<const Property&> FilterByName( const std::string& _text );

	/// \brief Search all properties which have a certain text sequence in
	///		there value.
	/// \details This method is not case sensitive.
	/// \return An array with read access to all found properties.
	std::vector<const Property&> FilterByValue( const std::string& _text );
private:
	PropertyList m_properties;
};


} // namespace Core