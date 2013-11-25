#include "Property.hpp"
#include "../fundamentals/Exception.hpp"
#include "../fundamentals/StringUtil.hpp"

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
} // namespace Core