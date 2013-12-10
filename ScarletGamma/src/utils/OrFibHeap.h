// ******************************************************************************** //
// OrFibHeap.h																		//
// ===========																		//
// This file is part of the OrBaseLib.												//
//																					//
// Author: Johannes Jendersie														//
//																					//
// Here is a quite easy licensing as open source:									//
// http://creativecommons.org/licenses/by/3.0/										//
// If you use parts of this project, please let me know what the purpose of your	//
// project is. You can do this by writing a comment at github.com/Jojendersie/.		//
//																					//
// For details on this project see: Readme.txt										//
// ******************************************************************************** //
// Implementing a priority queue (Fibonacci Heap).									//
//																					//
// HashMap:																			//
//	Insert()					O(1)												//
//	Delete(Element)				O(log n)											//
//	Min							O(1)												//
//	ChangeKey(Element)			O(1)												//
//	GetFirst()					O(1)												//
//	GetLast()					O(1)												//
//	GetNext()					O(1)												//
//	GetPrevious()				O(1)												//
//																					//
// It would be no problem to provide Search and key-Delete with O(n) but a key can	//
// occur multiple times in a heap. Therefore such an operation would be insecurely.	//
// To do some searching you can use the iterator.									//
// ******************************************************************************** //

#pragma once

namespace OrE {
namespace ADT {

	// ******************************************************************************** //
	// One node in the Fibonacci heap
	template<typename E> class FibHeapNode
	{
		// Make nodes unchangeable for all users except the heap.
		template<typename> friend class HeapT;

		// Prevent copy constructor and operator = being generated.
		FibHeapNode(const FibHeapNode&);
		const FibHeapNode& operator = (const FibHeapNode&);

		// Linked list of nodes which are siblings
		FibHeapNode* pLeft;
		FibHeapNode* pRight;
		// The parent node in the tree structure
		FibHeapNode* pParent;
		// One arbitrary child. (The other children can be reached through the linked list)
		FibHeapNode* pChild;
		//int iParam;					// Statistic information to mark nodes (marking = a child of this marked node was cut before)
		int iDegree;					// Number of children

		float iPriority;

		// Constructor inserting in the structure
		FibHeapNode(FibHeapNode* _pSibling, const E& _Obj, const float& _iKey):
			Object(_Obj), pLeft(nullptr), pRight(nullptr),
			pChild(nullptr), pParent(nullptr), iDegree(0),
			iPriority(_iKey)
		{
			if(_pSibling)
			{
				// Double linked list (ring)
				pLeft = _pSibling;
				pRight = _pSibling->pRight;
				_pSibling->pRight->pLeft = this;
				_pSibling->pRight = this;
			} else pLeft = pRight = this;
		}

		// Resets all pointers of _pNewChild and of other influenced nodes.
		// This operation should be possible for each two node, never mind
		// where they were before.
		void InsertToChildrenList( FibHeapNode<E>* _pNewChild );

	public:
		E Object;	///< The saved object in the heap
	};


	// ******************************************************************************** //
	/// \brief A well optimized priority queue where keys can be changed.
	/// \details This is an implementation of a fibonacci heap.
	///
	///		There is no base class for performance reasons. This improves cache
	///		performance by avoiding virtual functions.
	template<typename E> class HeapT
	{
	private:
		// Prevent copy constructor and operator = being generated.
		HeapT(const HeapT&);
		const HeapT& operator = (const HeapT&);

		typedef FibHeapNode<E>* FibHeapNodeP;

		/// \brief Internal management to increase allocation/deallocation and cache
		///		performance
		/// \details Each block has a capacity of 512 instances.
		///		Empty entries contain a pointer to the next free entry.
		FibHeapNode<E>** m_pMemoryBlocks;
		int m_iNumBlocks;
		FibHeapNode<E>* m_pFirstFreeNode;

		/// \brief Alloc space for a node with the internal system
		/// \returns A pointer to uninitialized memory.
		FibHeapNode<E>* Alloc();

	protected:
		/// One node in the first ring list.
		///
		FibHeapNodeP m_pRoot;

		/// Save number of contained elements
		///
		int m_iNumElements;

		/// \brief Merge of a part of a heap into the root list
		///
		void CutChildren(FibHeapNodeP _pPartition);

		/// \brief Ensure that no two roots have the same degree
		///
		void Consolidate();

		/// \brief Cuts one element and insert it to the root list. 
		/// Similar to meld, but do not affect the siblings of the element.
		void Cut(FibHeapNodeP _pElement);
	public:
		HeapT() : m_pMemoryBlocks(nullptr), m_iNumBlocks(0), m_pFirstFreeNode(nullptr), m_pRoot(nullptr), m_iNumElements(0) {}
		~HeapT();

		/// \brief Show the minimum element
		///
		FibHeapNodeP Min();

		/// \brief Change the key value and reorder the elements if necessary.
		/// \param [in] _iNewPriority Can be smaller or greater than the old one
		///		priority. (In contrast to the standard fibonacci heap, which
		///		allows decreases only).
		void ChangeKey(FibHeapNodeP _pElement, float _iNewPriority);

		/// \brief Standard operation insert
		/// \param [in] _iPriority The priority of the node.
		FibHeapNodeP Insert(const E& _Object, float _iPriority);

		/// \brief If the node itself is known it can be removed.
		/// \details The only arbitrary delete operation for the heap
		void Delete(FibHeapNodeP _pElement);

		/// \brief Remove everything
		///
		void Clear();

		
		FibHeapNodeP GetFirst();								///< First element = min element
		FibHeapNodeP GetLast();
		FibHeapNodeP GetNext(FibHeapNodeP _pCurrent);				// Preorder traverse
		FibHeapNodeP GetPrevious(FibHeapNodeP _pCurrent);			// Preorder traverse

		//typedef OrE::ADT::Iterator<E> Iterator;

		/// \brief Ask for the fill-state
		///
		int GetNumElements()		{return m_iNumElements;}
	};
















	// ************************** Implementation *************************** //

	template<typename E> void FibHeapNode<E>::InsertToChildrenList( FibHeapNode<E>* _pNewChild )
	{
		_pNewChild->pParent = this;
		// Remove the _pNewChild from an old list. Always succeeds, because pLeft and
		// pRight are always defined (_pNewChild if nothing else -> then no changes).
		_pNewChild->pLeft->pRight = _pNewChild->pRight;
		_pNewChild->pRight->pLeft = _pNewChild->pLeft;
		// Repair parent's child pointer if necessary
		if( pChild )
		{
			_pNewChild->pRight = pChild->pRight;
			pChild->pRight->pLeft = _pNewChild;
			_pNewChild->pLeft = pChild;
			pChild->pRight = _pNewChild;
		} else
		{
			pChild = _pNewChild;
			_pNewChild->pLeft = _pNewChild->pRight = _pNewChild;
		}
	} 


	// ******************************************************************************** //
	template<typename E> HeapT<E>::~HeapT()
	{
		Clear();
		for( int i=0; i<m_iNumBlocks; ++i )
			free(m_pMemoryBlocks[i]);
		free(m_pMemoryBlocks);
	}

	// ******************************************************************************** //
	// Alloc space for a node with the internal system
	template<typename E> FibHeapNode<E>* HeapT<E>::Alloc()
	{
		if( !m_pFirstFreeNode )
		{
			m_pMemoryBlocks = (FibHeapNode<E>**)realloc( m_pMemoryBlocks, (m_iNumBlocks+1) * sizeof(FibHeapNode<E>*) );
			m_pMemoryBlocks[m_iNumBlocks] = (FibHeapNode<E>*)malloc( sizeof(FibHeapNode<E>) * 512 );
			for(int i=0; i<511; ++i)
				*(FibHeapNode<E>**)(m_pMemoryBlocks[m_iNumBlocks]+i) = m_pMemoryBlocks[m_iNumBlocks]+i+1;
			*(FibHeapNode<E>**)(m_pMemoryBlocks[m_iNumBlocks]+511) = nullptr;
			m_pFirstFreeNode = m_pMemoryBlocks[m_iNumBlocks]+1;
			return m_pMemoryBlocks[m_iNumBlocks++];
		} else
		{
			FibHeapNode<E>* pNew = m_pFirstFreeNode;
			m_pFirstFreeNode = *(FibHeapNode<E>**)m_pFirstFreeNode;
			return pNew;
		}
	}

	// ******************************************************************************** //
	// Merge of a part of a heap into the root list
	template<typename E> void HeapT<E>::CutChildren(FibHeapNodeP _pPartition)
	{
		// Removing old connections
		if(_pPartition->pParent) {_pPartition->pParent->pChild = nullptr; --_pPartition->pParent->iDegree;}

		// Set all parent pointers to 0
		// pPartR is used later - do not change into pPartR = _pPartition;
		FibHeapNodeP pPartR = _pPartition->pRight;
		do {
			pPartR->pParent = nullptr;
			pPartR = pPartR->pRight;
		} while(pPartR != _pPartition->pRight);

		// Adding to the root list
		if(m_pRoot)
		{
			// A <-> Root | <-> B <-> C <-> A
			// D <-> Part | <-> E <-> F <-> D
			// Cut open the two ring lists and connect them
			_pPartition->pRight = m_pRoot->pRight;	// Part -> B
			m_pRoot->pRight->pLeft = _pPartition;	// B -> Part
			m_pRoot->pRight = pPartR;				// Root -> E
			pPartR->pLeft = m_pRoot;				// Root <- E
		} else m_pRoot = _pPartition;

		// Min update can only occur if the partition is of an other heap
		//if(_pPartition->qwKey < m_pRoot->qwKey) m_pRoot = _pPartition;
	}

	// ******************************************************************************** //
	// Ensure that now two roots have the same degree
	template<typename E> void HeapT<E>::Consolidate()
	{
		// Array to save trees with respect to there degree. This static form only
		// allows a certain number of elements in the heap. (TODO: could be dynamic)
		// Maximum degree is O(log n) (Theorem - can be proven).
		// -> O(exp(maxdegree)) nodes -> maxdegree=64 is high enough for everything
		// (constant factor is < 2)
		FibHeapNodeP aDegrees[64] = {nullptr};
		// Traverse root list
		FibHeapNodeP pCurrent = m_pRoot;
		do {
			assert(!pCurrent->pParent);
			// Is there an other node with the same degree?
			if(aDegrees[pCurrent->iDegree] && (aDegrees[pCurrent->iDegree] != pCurrent))
			{
			//	assert(aDegrees[pCurrent->iDegree] != pCurrent);
				do {
					// Remove the bigger one from the root list and insert this
					// as a new child from the smaller one.
					FibHeapNodeP pRef = aDegrees[pCurrent->iDegree];
					if(pRef->iPriority < pCurrent->iPriority)
					{
						// Assert that the root element is always in the root list
						if(m_pRoot == pCurrent)
							m_pRoot = pCurrent->pRight; // Take the right one to stop the while loop

						pRef->InsertToChildrenList( pCurrent );
						pCurrent = pRef;	// Skip back in traversing (degree of the smaller one will be updated after this if-else statement)
					} else {
						// Assert that the root element is always in the root list
						if(m_pRoot == pRef)
							m_pRoot = pRef->pRight;

						pCurrent->InsertToChildrenList( pRef );
					}
					aDegrees[pCurrent->iDegree] = nullptr;
					++pCurrent->iDegree;

					// Repeat if new degree is occupied too
				} while(aDegrees[pCurrent->iDegree]);
			} else
				aDegrees[pCurrent->iDegree] = pCurrent;

			// Traverse
			pCurrent = pCurrent->pRight;
		} while(pCurrent != m_pRoot);
	}

	// ******************************************************************************** //
	// Standard operation insert
	template<typename E> FibHeapNode<E>* HeapT<E>::Insert(const E& _Object, float _iPriority)
	{
		//FibHeapNode<E>* pMem = Alloc();
		FibHeapNode<E>* pNew = new (Alloc()) FibHeapNode<E>(m_pRoot, _Object, _iPriority);
		++m_iNumElements;
		// Min update
		if((!m_pRoot) || (pNew->iPriority<m_pRoot->iPriority))
			m_pRoot = pNew;
		return pNew;
	}

	// ******************************************************************************** //
	// Show the minimum element
	template<typename E> FibHeapNode<E>* HeapT<E>::Min()
	{
		return m_pRoot;
	}

	// ******************************************************************************** //
	// Cuts one element and insert it to the root list. (Similar to meld, but
	// do not affect the siblings of the element.)
	template<typename E> void HeapT<E>::Cut(FibHeapNodeP _pElement)
	{
		// Remove from sibling list
		_pElement->pLeft->pRight = _pElement->pRight;
		_pElement->pRight->pLeft = _pElement->pLeft;

		// Remove from parent
		--_pElement->pParent->iDegree;
		if(_pElement->pParent->pChild == _pElement) _pElement->pParent->pChild = _pElement->pLeft;
		// Since we safe ring buffers the left sibling could also be the node itself ->
		// check again -> this is/was the only child.
		if(_pElement->pParent->pChild == _pElement) _pElement->pParent->pChild = nullptr;
		_pElement->pParent = nullptr;

		// Insert to root list
		_pElement->pLeft = m_pRoot->pLeft;
		_pElement->pRight = m_pRoot;
		m_pRoot->pLeft->pRight = _pElement;
		m_pRoot->pLeft = _pElement;
	}

	// ******************************************************************************** //
	// Change the key value and reorder the elements if necessary
	template<typename E> void HeapT<E>::ChangeKey( FibHeapNodeP _pElement, float _iNewPriority )
	{
		// Set new value, it is changed anyway
		bool bIncrease = _pElement->iPriority<_iNewPriority;
		_pElement->iPriority = _iNewPriority;

		// Check if heap violated (decreased key)
		if( _pElement->pParent && (_pElement->iPriority < _pElement->pParent->iPriority) )
		{
			// Cut simple (cascading in original tree, but I don't understand the advantage of the cascading)
			Cut( _pElement );
		}
	
		// Check violation if key value was increased
		if( bIncrease && _pElement->pChild )
		{
			// Compare all child values (unsorted and independent)
			FibHeapNodeP pChild = _pElement->pChild;
			FibHeapNodeP pStop = _pElement->pChild;
			do {
				FibHeapNodeP pNextChild = pChild->pRight;
				if(pChild->iPriority < _iNewPriority)
					// Send child to root list
					Cut(pChild);
				pChild = pNextChild;
			} while( pChild != pStop );
		}

		// Check for new minimum
		if( _iNewPriority < m_pRoot->iPriority )
		{
			assert(!_pElement->pParent);
			m_pRoot = _pElement;
		}
	}


	// ******************************************************************************** //
	// The only arbitrary delete operation for the heap
	template<typename E> void HeapT<E>::Delete(FibHeapNodeP _pElement)
	{
		// Make minimum
		if(_pElement != m_pRoot)
			ChangeKey(_pElement, std::numeric_limits<float>::min());

		// Set the root pointer to an arbitrary other node
		if(m_pRoot->pLeft != m_pRoot)
		{
			m_pRoot->pLeft->pRight = m_pRoot->pRight;
			m_pRoot->pRight->pLeft = m_pRoot->pLeft;
			m_pRoot = m_pRoot->pLeft;
			assert( m_pRoot != _pElement );
		} else m_pRoot = nullptr;

		// Meld the children and the now consistent remaining root list
		if(_pElement->pChild) CutChildren(_pElement->pChild);

		if(m_pRoot)
		{
			// Restructure the heap
			Consolidate();

			// Find new minimum
			FibHeapNodeP pStart = m_pRoot;
			FibHeapNodeP pCurrent = m_pRoot;
			do {
				assert(!pCurrent->pParent);
				if(pCurrent->iPriority < m_pRoot->iPriority)
					m_pRoot = pCurrent;
				pCurrent = pCurrent->pRight;
			} while(pCurrent != pStart);
		}

		// "Delete"
		--m_iNumElements;

		// Because of placement new call destructor and free block entry
		_pElement->~FibHeapNode<E>();
		(*(FibHeapNode<E>**)_pElement) = m_pFirstFreeNode;
		m_pFirstFreeNode = _pElement;
	}

	// ******************************************************************************** //
	// Remove everything
	template<typename E> void HeapT<E>::Clear()
	{
		while(m_pRoot) Delete( Min() );
		m_pRoot = nullptr;
	}

	// ******************************************************************************** //
	// First element = min element
	template<typename E> FibHeapNode<E>* HeapT<E>::GetFirst()
	{
		return m_pRoot;
	}

	// ******************************************************************************** //
	// Traversing order goes right -> left is the last one in each ring
	template<typename E> FibHeapNode<E>* HeapT<E>::GetLast()
	{
		return m_pRoot->pLeft;
	}

	// ******************************************************************************** //
	// Preorder traverse
	template<typename E> FibHeapNode<E>* HeapT<E>::GetNext(FibHeapNodeP _pCurrent)
	{
		// Go to each child before traversing this level complete
		if(_pCurrent->pChild) return _pCurrent->pChild;
		// End of traversing in a ring: reached first element
		if(_pCurrent->pParent && (_pCurrent->pRight == _pCurrent->pParent->pChild))
			// Parent was seen before going to the children.
			// We need the pNode->pParent->pRight. Changing pNode
			// results in pRight, because this is the only case after
			// this line. Advantage: we have not to check the end of recursion here.
			_pCurrent = _pCurrent->pParent;
		// End of traversing we reached the root (start)
		if(_pCurrent->pRight == m_pRoot) return nullptr;
		// Default in a simple list - next one
		return _pCurrent->pRight;
	}

	// ******************************************************************************** //
	// Preorder traverse
	template<typename E> FibHeapNode<E>* HeapT<E>::GetPrevious(FibHeapNodeP _pCurrent)
	{
		// Go to each child before traversing this level complete
		if(_pCurrent->pChild) return _pCurrent->pChild;
		// End of traversing in a ring: reached first element
		if(_pCurrent->pParent && (_pCurrent->pLeft == _pCurrent->pParent->pChild))
			// Parent was seen before going to the children.
			// We need the pNode->pParent->pLeft. Changing pNode
			// results in pLeft, because this is the only case after
			// this line. Advantage: we have not to check the end of recursion here.
			_pCurrent = _pCurrent->pParent;
		// End of traversing we reached the root (start)
		if(_pCurrent->pLeft == m_pRoot) return nullptr;
		// Default in a simple list - next one
		return _pCurrent->pLeft;
	}

}; // namespace ADT
}; // namespace OrE
// **********************************EOF************************************ //