#include "Property.hpp"
#include "utils/Exception.hpp"
#include "utils/StringUtil.hpp"
#include "network/ObjectMessages.hpp"
#include <algorithm>

using namespace std;

namespace Core {
	Property::Property( ObjectID _parent, const std::string& _name, const std::string& _value ) :
		m_name(_name),
		m_isObjectList(false),
		m_value(_value),
		m_parent(_parent)
	{
	}

	Property::Property( ObjectID _parent, const std::string& _name, const ObjectList& _list ) :
		m_name(_name),
		m_isObjectList(true),
		m_value(""),
		m_objects(_list),
		m_parent(_parent)
	{
	}

	Property::Property( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node ) :
		m_parent(_parent)
	{
		m_name = _node[0].GetName();
		m_value = _node[0];
		const Jo::Files::MetaFileWrapper::Node* objects;
		if( _node.HasChild( string("objects"), &objects ) )
		{
			m_objects = ObjectList(*objects);
		}
	}

	const ObjectList& Property::GetObjects()
	{
		if( !m_isObjectList ) throw Exception::NoObjectList();
		return m_objects;
	}

	void Property::AddObject( ObjectID _id )
	{
		if( !m_isObjectList ) throw Exception::NoObjectList();

		m_objects.Add(_id);
		Network::MsgPropertyChanged( m_parent, this ).Send();
	}

	void Property::RemoveObject( ObjectID _id )
	{
		if( !m_isObjectList ) throw Exception::NoObjectList();

		m_objects.Remove(_id);
		Network::MsgPropertyChanged( m_parent, this ).Send();
	}

	void Property::ClearObjects()
	{
		if( m_objects.Size() > 0 )
		{
			m_objects.Clear();
			Network::MsgPropertyChanged( m_parent, this ).Send();
		}
	}

	float Property::Evaluate() const
	{
		return 1.0f;	// TODO: Implement
	}

	const std::string& Property::Value() const
	{
		return m_value;
	}

	void Property::SetValue( const std::string& _new )
	{
		if( m_value != _new )
		{
			m_value = _new;
			Network::MsgPropertyChanged( m_parent, this ).Send();
		}
	}

	void Property::Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const
	{
		_node[m_name] = m_value;
		if( m_isObjectList ) {
			m_objects.Serialize(_node[std::string("objects")]);
		}
	}






	PropertyList::PropertyList( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node )
	{
		for( uint64_t i=0; i<_node.Size(); ++i )
		{
			Add( Property( _parent, _node[i] ) );
		}
	}

	void PropertyList::Add( const Property& _property )
	{
		// Create a case insensitive key
		std::string key( _property.Name() );
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		// No duplicates! Automatically overwrite if exists.
		m_map.insert( std::make_pair(key, _property) );
		Network::MsgPropertyChanged( _property.ParentObject(), &_property ).Send();
	}


	void PropertyList::Remove( const std::string& _name )
	{
		// Create a case insensitive key
		std::string key( _name );
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		m_map.erase( key );
	}

	Property* PropertyList::Get( const std::string& _name )
	{
		// A trick to avoid redundant implementation. The const_cast is valid
		// in this context because we know the current list isn't const.
		return const_cast<Property*>(const_cast<const PropertyList*>(this)->Get(_name));
	}

	const Property* PropertyList::Get( const std::string& _name ) const
	{
		// Create a case insensitive key
		std::string key( _name );
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		auto it = m_map.find( key );
		if( it != m_map.end() ) return &it->second;

		// not in list
		return nullptr;
	}

	void PropertyList::Clear()
	{
		m_map.clear();
	}


	std::vector<const Property*> PropertyList::FilterByName( const std::string& _text ) const
	{
		std::vector<const Property*> _results;
		for(auto current = m_map.begin(); current != m_map.end(); ++current )
		{
			// Test if the name contains the correct part
			if( Utils::IStringContains(current->second.Name(), _text) )
				// Add reference to output
				_results.push_back( &current->second );
		}
		return _results;
	}

	std::vector<const Property*> PropertyList::FilterByValue( const std::string& _text ) const
	{
		std::vector<const Property*> _results;
		for(auto current = m_map.begin(); current != m_map.end(); ++current )
		{
			// Test if the name contains the correct part
			if( Utils::IStringContains(current->second.Value(), _text) )
				// Add reference to output
				_results.push_back( &current->second );
		}
		return _results;
	}

	void PropertyList::Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const
	{
		// Would also run without preallocation but so its faster.
		_node.Resize(m_map.size(), Jo::Files::MetaFileWrapper::ElementType::NODE);
		int i=0;
		for(auto current = m_map.begin(); current != m_map.end(); ++current ) {
			current->second.Serialize( _node[i] );
			++i;
		}
	}

} // namespace Core