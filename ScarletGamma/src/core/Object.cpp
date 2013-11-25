#pragma once

#include "Object.hpp"
#include "fundamentals/Exception.hpp"

namespace Core {

	Property& Object::GetProperty( const std::string& _name )
	{
		Property* prop = m_properties.Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	const Property& Object::GetProperty( const std::string& _name ) const
	{
		const Property* prop = m_properties.Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	bool Object::HasProperty( const std::string& _name ) const
	{
		return m_properties.Get(_name) != nullptr;
	}


	std::vector<const Property*> Object::FilterByName( const std::string& _text ) const
	{
		return std::move(m_properties.FilterByName(_text));
	}


	std::vector<const Property*> Object::FilterByValue( const std::string& _text ) const
	{
		return std::move(m_properties.FilterByValue(_text));
	}

} // namespace Core