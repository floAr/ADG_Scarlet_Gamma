#include "Property.hpp"
#include "utils/Exception.hpp"
#include "utils/StringUtil.hpp"
#include "network/ObjectMessages.hpp"

using namespace std;

namespace Core {
	Property::Property( ObjectID _parent, const std::string& _name, const std::string& _value ) :
		m_name(_name),
		m_isObjectList(false),
		m_value(_value),
		m_parent(_parent)
	{
	}

	Property::Property( ObjectID _parent, const std::string& _name, const std::string& _value, const ObjectList& _list ) :
		m_name(_name),
		m_isObjectList(true),
		m_value(_value),
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

	void Property::PopFront()
	{
		{ if( !m_isObjectList ) throw Exception::NoObjectList(); m_objects.PopFront(); Network::MsgPropertyChanged( m_parent, this ).Send(); }
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
		// No duplicates!
		Remove( _property.Name() );
		// Append at the end
		m_list.push_back(_property);
		Network::MsgPropertyChanged( _property.ParentObject(), &_property ).Send();
	}

	void PropertyList::Remove( Property* _property )
	{
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
			if( &(*current) == _property ) {
				Network::MsgRemoveProperty( _property->ParentObject(), _property ).Send();
				m_list.erase( current );
				return;
			}
		// Not in list
	}

	void PropertyList::Remove( const std::string& _name )
	{
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
		{
			if( Utils::IStringEqual( current->Name(), _name ) )
			{
				Network::MsgRemoveProperty( current->ParentObject(), &(*current) ).Send();
				m_list.erase(current);
				return;
			}
		}
	}

	Property* PropertyList::Get( const std::string& _name )
	{
		// A trick to avoid redundant implementation. The const_cast is valid
		// in this context because we know the current list isn't const.
		return const_cast<Property*>(const_cast<const PropertyList*>(this)->Get(_name));
	}

	const Property* PropertyList::Get( const std::string& _name ) const
	{
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
		{
			if( Utils::IStringEqual( (*current).Name(), _name ) )
				return &(*current);
		}

		// not in list
		return nullptr;
	}

	void PropertyList::Clear()
	{
		m_list.clear();
	}


	std::vector<const Property*> PropertyList::FilterByName( const std::string& _text ) const
	{
		std::vector<const Property*> _results;
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
		{
			// Test if the name contains the correct part
			if( Utils::IStringContains(current->Name(), _text) )
				// Add reference to output
				_results.push_back( &(*current) );
		}
		return _results;
	}

	std::vector<const Property*> PropertyList::FilterByValue( const std::string& _text ) const
	{
		std::vector<const Property*> _results;
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
		{
			// Test if the name contains the correct part
			if( Utils::IStringContains(current->Value(), _text) )
				// Add reference to output
				_results.push_back( &(*current) );
		}
		return _results;
	}

	void PropertyList::Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const
	{
		// Would also run without preallocation but so its faster.
		_node.Resize(m_list.size(), Jo::Files::MetaFileWrapper::ElementType::NODE);
		int i=0;
		for(auto current = m_list.begin(); current != m_list.end(); ++current ) {
			current->Serialize( _node[i] );
			++i;
		}
	}

} // namespace Core