#pragma once

#include <vector>
#include "../Prerequisites.hpp"

namespace Core {

/// \brief A dynamic list of object-references.
class ObjectList
{
public:
	/// \brief Add a new object to the list.
	/// \details This method does a linear search in debug mode and tests if
	///		the object is already in the list. If so it causes an assertion to fail.
	void Add( ObjectID _id );

	/// \brief Delete a object reference from this list.
	/// \details The object is searched linear to be removed.
	///
	///		This method does not change the original object.
	void Remove( ObjectID _id );

	/// \brief Number of elements in the list
	int Size() const { return (int)m_objects.size(); }

	/// \brief Index access to all elements in the list.
	/// \details Index out of bounds is checked by an assertion in debug mode.
	ObjectID operator[](int _index) const;

private:
	std::vector<ObjectID> m_objects;
};

} // namespace Core