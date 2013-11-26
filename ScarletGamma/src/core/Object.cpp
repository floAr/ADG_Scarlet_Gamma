#include "Object.hpp"
#include "utils/Exception.hpp"

using namespace std;

namespace Core {

	Object::Object( float _x, float _y, const string& _sprite )
	{
		m_properties.Add( Property(string("X"), to_string(_x)) );
		m_properties.Add( Property(string("Y"), to_string(_y)) );
		m_properties.Add( Property(string("Sprite"), _sprite) );
	}

	Object::Object( const Jo::Files::MetaFileWrapper::Node& _parent ) :
		m_properties( _parent )
	{
	}

	Property& Object::GetProperty( const string& _name )
	{
		Property* prop = m_properties.Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	const Property& Object::GetProperty( const string& _name ) const
	{
		const Property* prop = m_properties.Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	bool Object::HasProperty( const string& _name ) const
	{
		return m_properties.Get(_name) != nullptr;
	}


	std::vector<const Property*> Object::FilterByName( const string& _text ) const
	{
		return std::move(m_properties.FilterByName(_text));
	}


	std::vector<const Property*> Object::FilterByValue( const string& _text ) const
	{
		return std::move(m_properties.FilterByValue(_text));
	}

	void Object::Serialize( Jo::Files::MetaFileWrapper::Node& _parent )
	{
		// Currently the object does not add any extra information.
		m_properties.Serialize(_parent);
	}

} // namespace Core