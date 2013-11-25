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



	PropertyList::PropertyList() :
		m_first(nullptr),
		m_last(nullptr)
	{
	}

	PropertyList::~PropertyList()
	{
		Clear();
	}

	void PropertyList::Add( const Property& _property )
	{
		// Append at the end
		if( m_last ) {
			m_last->m_next = new ListNode( _property );
			m_last = m_last->m_next;
		} else
			m_first = m_last = new ListNode( _property );
	}

	void PropertyList::Remove( Property* _property )
	{
		ListNode* current = m_first;
		ListNode* last = nullptr;
		while(current)
		{
			if( &current->m_property == _property )
			{
				// Found
				// Remove from list
				if( last ) last->m_next = current->m_next;
				else m_first = current->m_next;
				if( !m_first ) m_last = nullptr;	// Case: the current element was the only one

				delete current;
				return;
			}
			current = current->m_next;
		}
		// Not in list
	}

	void PropertyList::Remove( const std::string& _name )
	{
		// OPTIMIZE: Go only once through the list insted of repeated search
		Property* found = Get(_name);
		while(found) {
			Remove(found);
			found = Get(_name);
		}
	}

	Property* PropertyList::Get( const std::string& _name )
	{
		ListNode* current = m_first;
		while(current)
		{
			if( Utils::IStringEqual( current->m_property.Name(), _name ) )
				return &current->m_property;
			current = current->m_next;
		}

		// not in list
		return nullptr;
	}

	void PropertyList::Clear()
	{
		if( m_first )
		{
			// Delete iteratively (no recursive destructor!)
			ListNode* next = m_first->m_next;
			while( m_first )
			{
				delete m_first;
				m_first = next;
				next = next->m_next;
			}
			m_last = nullptr;
			// Both m_first and m_last are now nullptr
		}
	}
} // namespace Core