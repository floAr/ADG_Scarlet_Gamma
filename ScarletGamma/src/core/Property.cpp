#include "Property.hpp"
#include "utils/Exception.hpp"
#include "utils/StringUtil.hpp"
#include "network/ObjectMessages.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Random.hpp"
#include "Constants.hpp"
#include <iostream>

using namespace std;

namespace Core {
	Property::Property( ObjectID _parent, uint32_t _rights, const std::string& _name, const std::string& _value ) :
		m_name(_name),
		m_value(_value),
		m_parent(_parent),
		m_rights(_rights)
	{
	}

	Property::Property( ObjectID _parent, uint32_t _rights, const std::string& _name, const std::string& _value, const ObjectList& _list ) :
		m_name(_name),
		m_value(_value),
		m_objects(_list),
		m_parent(_parent),
		m_rights(_rights)
	{
	}

	Property::Property( ObjectID _parent, const Jo::Files::MetaFileWrapper::Node& _node ) :
		m_parent(_parent)
	{
		m_name = _node[0].GetName();
		m_value = _node[0];
		m_rights = _node[STR_RIGHTS];
		const Jo::Files::MetaFileWrapper::Node* objects;
		if( _node.HasChild( string(STR_OBJECTS), &objects ) )
		{
			m_objects = ObjectList(*objects);
		}
	}

	const ObjectList& Property::GetObjects()
	{
		return m_objects;
	}

	void Property::AddObject( ObjectID _id )
	{
		m_objects.Add(_id);
		Network::MsgPropertyChanged( m_parent, this ).Send();
	}

	void Property::RemoveObject( ObjectID _id )
	{
		if( !IsObjectList() ) throw Exception::NoObjectList();

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

	int Property::Evaluate() const
	{
        // TODO: Where would we store the random generator?
        static Utils::Random rand(6045800);

        try
        {
            return Utils::EvaluateFormula(Value(), &rand);
        }
        catch (Exception::InvalidFormula& e)
        {
            std::cerr << "Exception::InvalidFormula: " << e.to_string() << std::endl;
            throw Exception::NotEvaluateable();
        }
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
		_node[STR_RIGHTS] = m_rights;
		if( IsObjectList() ) {
			m_objects.Serialize(_node[std::string(STR_OBJECTS)]);
		}
	}

	void Property::PopFront()
	{
		if( !IsObjectList() ) throw Exception::NoObjectList();
		m_objects.PopFront();
		Network::MsgPropertyChanged( m_parent, this ).Send();
	}

	bool Property::CanSee( PlayerID _player ) const
	{
		if( _player == 0 )
		{			// Check master flags
			return (m_rights & 0x001) != 0;
		} else {	// Check if player has rights and if so check flags
			if( m_rights & (1 << (_player+8)) )
				return (m_rights & 0x008) != 0;
			else return (m_rights & 0x040) != 0;
		}
	}

	bool Property::CanChange( PlayerID _player ) const
	{
		if( _player == 0 )
		{			// Check master flags
			return (m_rights & 0x2) != 0;
		} else {	// Check if player has rights and if so check flags
			if( m_rights & (1 << (_player+8)) )
				return (m_rights & 0x010) != 0;
			else return (m_rights & 0x080) != 0;
		}
	}

	bool Property::CanEdit( PlayerID _player ) const
	{
		if( _player == 0 )
		{			// Check master flags
			return (m_rights & 0x004) != 0;
		} else {	// Check if player has rights and if so check flags
			if( m_rights & (1 << (_player+8)) )
				return (m_rights & 0x020) != 0;
			else return (m_rights & 0x100) != 0;
		}
	}

	void Property::ApplyRights( PlayerID _player, bool _hasAdvancedPlayer )
	{
		// Set or delete the advanced flag for the player
		uint32_t oldRights = m_rights;
		uint32_t flag = uint32_t(1) << (_player+8);
		if( _hasAdvancedPlayer ) m_rights |= flag;
		else m_rights &= ~flag;

		// Everybody has to know the property change.
		if(oldRights != m_rights)
			Network::MsgPropertyChanged( m_parent, this ).Send();
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