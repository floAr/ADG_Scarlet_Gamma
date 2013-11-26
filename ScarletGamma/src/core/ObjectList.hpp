#pragma once

#include <vector>
#include <jofilelib.hpp>
#include "../Prerequisites.hpp"

namespace Core {

/// \brief A dynamic list of object-references.
class ObjectList
{
public:
	/// \brief Standard constructor to create an empty list.
	ObjectList()	{}

	/// \brief Deserialize an object.
	/// \param [in] _node A serialized object node.
	ObjectList(const Jo::Files::MetaFileWrapper::Node& _node);

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

	/// \brief Write the content of this object to a meta-file.
	/// \param [inout] _node A node with ElementType::UNKNOWN which can
	///		be changed and expanded by serialize.
	void Serialize( Jo::Files::MetaFileWrapper::Node& _node );
private:
	std::vector<ObjectID> m_objects;
};

} // namespace Core