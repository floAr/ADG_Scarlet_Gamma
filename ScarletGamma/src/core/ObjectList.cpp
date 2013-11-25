#include "ObjectList.hpp"
#include <cassert>

namespace Core {

	void ObjectList::Add( ObjectID _id )
	{
		// If the program stops here somthing with object loading or 
		// synchronisation failed (Every object id should excists only once).
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
		assert(0<=_index && _index<m_objects.size());
		return m_objects[_index];
	}

} // namespace Core