#include "Object.hpp"
#include "utils/Exception.hpp"

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


	void Object::Serialize( Jo::Files::MetaFileWrapper::Node& _node )
	{
		_node[string("ID")] = m_id;
		PropertyList::Serialize(_node[string("Properties")]);
	}

} // namespace Core