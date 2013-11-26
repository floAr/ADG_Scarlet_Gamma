#include "Property.hpp"
#include "../utils/Exception.hpp"
#include "../utils/StringUtil.hpp"

using namespace std;

namespace Core {
	Property::Property( const std::string& _name, const std::string& _value ) :
		m_name(_name),
		m_isObjectList(false),
		m_value(_value)
	{
	}

	Property::Property( const std::string& _name, const ObjectList& _list ) :
		m_name(_name),
		m_isObjectList(true),
		m_value(""),
		m_objects(_list)
	{
	}

	Property::Property( const Jo::Files::MetaFileWrapper::Node& _parent )
	{
		m_name = _parent[string("name")];
		m_value = _parent[string("value")];
		const Jo::Files::MetaFileWrapper::Node* objects;
		if( _parent.HasChild( string("objects"), &objects ) )
		{
			m_objects = ObjectList(*objects);
		}
	}

	ObjectList& Property::Objects()
	{
		if( !m_isObjectList ) throw Exception::NoObjectList();
		return m_objects;
	}

	float Property::Evaluate() const
	{
		return 0.0f;	// TODO: Implement
	}

	const std::string& Property::Value() const
	{
		return m_value;
	}

	void Property::SetValue( const std::string& _new )
	{
		m_value = _new;
	}

	void Property::Serialize( Jo::Files::MetaFileWrapper::Node& _parent )
	{
		_parent[std::string("name")] = m_name;
		_parent[std::string("value")] = m_value;
		if( m_isObjectList ) {
			m_objects.Serialize(_parent[std::string("objects")]);
		}
	}






	PropertyList::PropertyList( const Jo::Files::MetaFileWrapper::Node& _parent )
	{
		for( uint64_t i=0; i<_parent.Size(); ++i )
		{
			Add( Property( _parent[i] ) );
		}
	}

	void PropertyList::Add( const Property& _property )
	{
		// Append at the end
		m_list.push_back(_property);
	}

	void PropertyList::Remove( Property* _property )
	{
		for(auto current = m_list.begin(); current != m_list.end(); ++current )
			if( &(*current) == _property ) {
				m_list.erase( current );
				return;
			}
		// Not in list
	}

	void PropertyList::Remove( const std::string& _name )
	{
		for(auto current = m_list.begin(); current != m_list.end(); )
		{
			// erase from list returns an iterator to the first element after
			// the deleted sequence. In this case the iterator should not be
			// increase -> not in for loop.
			if( Utils::IStringEqual( (*current).Name(), _name ) )
				current = m_list.erase(current);
			else ++current;
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

	void PropertyList::Serialize( Jo::Files::MetaFileWrapper::Node& _parent )
	{
		// Would also run without preallocation but so its faster.
		_parent.Resize(m_list.size(), Jo::Files::MetaFileWrapper::ElementType::NODE);
		int i=0;
		for(auto current = m_list.begin(); current != m_list.end(); ++current ) {
			current->Serialize( _parent[i] );
			++i;
		}
	}

} // namespace Core