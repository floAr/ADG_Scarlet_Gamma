#include "Object.hpp"
#include "utils/Exception.hpp"
#include <iomanip>
#include <sstream>

using namespace std;

namespace Core {

	Object::Object( ObjectID _id, const string& _sprite ) :
		m_id(_id)
	{
		Add( Property("X", "0.0") );
		Add( Property("Y", "0.0") );
		Add( Property("Sprite", _sprite) );
	}

	Object::Object( const Jo::Files::MetaFileWrapper::Node& _node ) :
		PropertyList( _node[string("Properties")] )
	{
		m_id = _node[string("ID")];
	}

	Property& Object::GetProperty( const string& _name )
	{
		Property* prop = Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	const Property& Object::GetProperty( const string& _name ) const
	{
		const Property* prop = Get(_name);
		if( !prop ) throw Exception::NoSuchProperty();

		return *prop;
	}


	bool Object::HasProperty( const string& _name ) const
	{
		return Get(_name) != nullptr;
	}


	void Object::SetPosition( float _x, float _y )
	{
		Get("X")->SetValue( to_string(_x) );
		Get("Y")->SetValue( to_string(_y) );
	}


	sf::Vector2f Object::GetPosition() const
	{
		sf::Vector2f pos;
		pos.x = (float)atof(Get("X")->Value().c_str());
		pos.y = (float)atof(Get("Y")->Value().c_str());
		return pos;
	}


	sf::Color Object::GetColor() const
	{
		// The property does not always exists
		auto colorProp = Get("Color");
		if( colorProp ) {
			// Evaluate the hexadecimal number
			uint32_t color = strtoul(colorProp->Value().c_str(), nullptr, 16);
			return sf::Color(color>>24, (color>>16) & 0xff, (color>>8) & 0xff, color & 0xff);
		}
		return sf::Color::White;
	}


	void Object::SetColor( const sf::Color& _color )
	{
		std::stringstream value;
		value << std::hex << (int)_color.r << (int)_color.g << (int)_color.b << (int)_color.a;
		// Create or set property?
		auto colorProp = Get("Color");
		if( !colorProp ) Add(Property(string("Color"), value.str()));
		else colorProp->SetValue( value.str() );
	}


	void Object::Serialize( Jo::Files::MetaFileWrapper::Node& _node )
	{
		_node[string("ID")] = m_id;
		PropertyList::Serialize(_node[string("Properties")]);
	}

} // namespace Core