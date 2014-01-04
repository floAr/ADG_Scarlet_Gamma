#include "Object.hpp"
#include "utils/Exception.hpp"
#include <iomanip>
#include <sstream>

using namespace std;

namespace Core {

	const string Object::PROP_LAYER = string("layer");
	const string Object::PROP_X = string("x");
	const string Object::PROP_Y = string("y");
	const string Object::PROP_SPRITE = string("sprite");
	const string Object::PROP_COLOR = string("color");
	const string Object::PROP_PATH = string("path");
	const string Object::PROP_TARGET = string("target");
	const string Object::PROP_OBSTACLE = string("obstacle");

	Object::Object( ObjectID _id, const string& _sprite ) :
		m_id(_id)
	{
		Add( Property(_id, PROP_SPRITE, _sprite) );
	}

	Object::Object( const Jo::Files::MetaFileWrapper::Node& _node ) :
		PropertyList( _node[string("ID")], _node[string("Properties")] )
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



	sf::Vector2f Object::GetPosition() const
	{
		const Property* X = Get(PROP_X);
		const Property* Y = Get(PROP_Y);
		if( !X || !Y ) throw Exception::NoSuchProperty();
		sf::Vector2f pos;
		pos.x = (float)atof(X->Value().c_str());
		pos.y = (float)atof(Y->Value().c_str());
		return pos;
	}


	sf::Color Object::GetColor() const
	{
		// The property does not always exists
		auto colorProp = Get(PROP_COLOR);
		if( colorProp ) {
			// Evaluate the hexadecimal number
			uint32_t color = strtoul(colorProp->Value().c_str(), nullptr, 16);
			return sf::Color(color>>24, (color>>16) & 0xff, (color>>8) & 0xff, color & 0xff);
		}
		return sf::Color::White;
	}


	void Object::SetColor( const sf::Color& _color )
	{
		char value[9];
		sprintf(value, "%02x%02x%02x%02x", (int)_color.r, (int)_color.g, (int)_color.b, (int)_color.a);
//		std::stringstream value;
		//value << std::hex << (int)_color.r << (int)_color.g << (int)_color.b << (int)_color.a;
		// Create or set property?
		auto colorProp = Get(PROP_COLOR);
		if( !colorProp ) Add(Property(m_id, PROP_COLOR, string(value)));
		else colorProp->SetValue( value );
	}


	void Object::Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const
	{
		_node[string("ID")] = m_id;
		PropertyList::Serialize(_node[string("Properties")]);
	}

	void Object::AppendToPath( ObjectID _wayPoint )
	{
		Property& path = GetProperty( PROP_PATH );
		path.AddObject( _wayPoint );
		// Check for a loop: a single node cannot be a loop, first and last
		// node must be the same.
		bool hasLoop = path.GetObjects().Size() > 1;
		hasLoop &= path.GetObjects()[0] == path.GetObjects()[path.GetObjects().Size()-1];
		path.SetValue( hasLoop ? "true" : "false" );
	}

} // namespace Core