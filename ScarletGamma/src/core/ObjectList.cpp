#include "ObjectList.hpp"
#include <cassert>

namespace Core {

	ObjectList::ObjectList( const Jo::Files::MetaFileWrapper::Node& _node )
	{
		m_objects.resize( _node.Size() );
		for( size_t i=0; i<m_objects.size(); ++i )
			m_objects[i] = _node[i];
	}


	void ObjectList::Add( ObjectID _id )
	{
		// If the program stops here something with object loading or 
		// synchronization failed (Every object id should exists only once).
		for( size_t i=0; i<m_objects.size(); ++i )
			assert(m_objects[i] != _id);

		m_objects.push_back(_id);
	}


	void ObjectList::Remove( ObjectID _id )
	{
		for( size_t i=0; i<m_objects.size(); ++i )
			if( m_objects[i] == _id ) {
				m_objects.erase(m_objects.begin() + i);
				// Stop search (everything is contained exactly once)
				return;
			}
		// You tried to remove an object which is not in the list.
		assert(false);
	}


	ObjectID ObjectList::operator[](int _index) const
	{
		assert(0<=_index && _index<(int)m_objects.size());
		return m_objects[_index];
	}

	void ObjectList::Serialize( Jo::Files::MetaFileWrapper::Node& _node )
	{
		// Would also run without preallocation but so its faster.
		_node.Resize(m_objects.size(), Jo::Files::MetaFileWrapper::ElementType::UINT32);
		for( size_t i=0; i<m_objects.size(); ++i )
			_node[i] = m_objects[i];
	}


} // namespace Core