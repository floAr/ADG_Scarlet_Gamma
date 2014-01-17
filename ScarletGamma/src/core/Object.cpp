#include "Object.hpp"
#include "Constants.hpp"
#include "utils/Exception.hpp"
#include <iomanip>
#include <sstream>

using namespace std;

namespace Core {

	const string Object::PROP_LAYER = string("Layer");
	const string Object::PROP_X = string("X");
	const string Object::PROP_Y = string("Y");
	const string Object::PROP_SPRITE = string("Bild");
	const string Object::PROP_COLOR = string("Farbe");
	const string Object::PROP_PATH = string("path");
	const string Object::PROP_TARGET = string("target");
	const string Object::PROP_OBSTACLE = string("Hindernis");
	const string Object::PROP_NAME = string("Name");
	const string Object::PROP_PLAYER = string("player");

	Object::Object( ObjectID _id, const string& _sprite ) :
		m_id(_id),
		m_hasParent(false)
	{
		Add( Property(_id, Property::R_V0E000000, PROP_SPRITE, _sprite) );
	}

	Object::Object( const Jo::Files::MetaFileWrapper::Node& _node ) :
		PropertyList( _node[STR_ID], _node[STR_PROPERTIES] )
	{
		m_id = _node[STR_ID];
		const Jo::Files::MetaFileWrapper::Node* parentNode;
		if( m_hasParent = _node.HasChild(STR_PARENT, &parentNode) )
		{
			if( IsLocatedOnAMap() )
				m_parent.map = *parentNode;
			else
				m_parent.object = *parentNode;
		}
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


	std::string Object::GetName() const
	{
		const Property* name = Get(PROP_NAME);
		if( !name ) return std::to_string(m_id);	// Will use move semantic
		return name->Value();						// Uses copy - bad but safe
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
		if( !colorProp ) Add(Property(m_id, Property::R_VCEV0EV00, PROP_COLOR, string(value)));
		else colorProp->SetValue( value );
	}


	void Object::Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const
	{
		_node[STR_ID] = m_id;
		if( m_hasParent )
		{
			if( IsLocatedOnAMap() )
				_node[STR_PARENT] = m_parent.map;
			else
				_node[STR_PARENT] = m_parent.object;
		}
		PropertyList::Serialize(_node[STR_PROPERTIES]);
	}

	void Object::AppendToPath( ObjectID _wayPoint )
	{
		Property& path = GetProperty( PROP_PATH );
		path.AddObject( _wayPoint );
		// Check for a loop: a single node cannot be a loop, first and last
		// node must be the same.
		bool hasLoop = path.GetObjects().Size() > 1;
		hasLoop &= path.GetObjects()[0] == path.GetObjects()[path.GetObjects().Size()-1];
		path.SetValue( hasLoop ? STR_TRUE : STR_FALSE );
	}

} // namespace Core