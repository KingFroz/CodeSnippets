#pragma once
// forward declaration
template<typename Type> class SLLIter;

// class SLList
template<typename Type> class SLList
{
	// the iterator is the list’s friend
	friend class SLLIter<Type>;

	// add members/methods here…
private:
	struct Node
	{
		Node* next;
		Type element;
	};

	Node* head;
	int nSize;

public: 
	SLList();
	~SLList();

	SLList<Type>& operator=(const SLList<Type>& that);
	SLList(const SLList<Type>& that);

	void clear();
	void rec(Node* _point);
	void addHead(const Type& v);
	void insert(SLLIter<Type>& index, const Type& v);
	void remove(SLLIter<Type>& index);

	inline unsigned int size() const { return nSize; }
};

/////////////////////////////////////////////////////////////////////////////
// Function : Constructor
// Notes : constructs an empty list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLList<Type>::SLList()
{
	nSize = 0;
	head = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Destructor
// Notes : Destroys the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLList<Type>::~SLList()
{
	clear();
}

template<typename Type>
void SLList<Type>::rec(Node* _point)
{
	if (nullptr == _point)
		return;

	rec(_point->next);
	addHead(_point->element);
}

/////////////////////////////////////////////////////////////////////////////
// Function : Assignment Operator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLList<Type>& SLList<Type>::operator=(const SLList<Type>& that)
{
	if (&that != this)
	{
		clear();
		rec(that.head);
	}

	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Copy Constructor
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLList<Type>::SLList(const SLList<Type>& that)
{
	head = nullptr;
	*this = that;
}
/////////////////////////////////////////////////////////////////////////////
// Function : addHead
// Parameters :	v - the item to add to the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void SLList<Type>::addHead(const Type& v)
{
	Node* tmp = new Node;

	tmp->next = head;
	tmp->element = v;

	head = tmp;
	nSize += 1;
}
/////////////////////////////////////////////////////////////////////////////
// Function : clear
// Notes : clears the list, freeing any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void SLList<Type>::clear()
{
	Node* tmp;

	while (nullptr != head)
	{
		tmp = head;
		head = head->next;
		delete tmp;
	}
	
	nSize = 0;
}
/////////////////////////////////////////////////////////////////////////////
// Function : insert
// Parameters :	index - an iterator to the location to insert at
//				v - the item to insert
// Notes : do nothing on a bad iterator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void SLList<Type>::insert(SLLIter<Type>& index, const Type& v)
{
	if (nullptr == index.cur)
		return;

	if (nullptr != index.prev)
	{
		Node* tmp = new Node;
		tmp->element = v;
		index.prev->next = tmp;
		tmp->next = index.cur;
		index.cur = tmp;

		nSize += 1;
	}
	else
	{
		Node* tmp = new Node;
		tmp->element = v;
		tmp->next = head;
		head = tmp;
		index.cur = head;

		nSize += 1;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Function : remove
// Parameters :	index - an iterator to the location to remove from
// Notes : do nothing on a bad iterator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void SLList<Type>::remove(SLLIter<Type>& index)
{
	if (size() < 1)
		return;

	if (index.cur == head)
	{
		index.cur = head->next;
		delete head;
		head = index.cur;
		nSize -= 1;
	}
	else
	{
		index.prev->next = index.cur->next;
		delete index.cur;
		index.cur = index.prev->next;
		nSize -= 1;
	}
}

// class SLLIter
template<typename Type> class SLLIter
{
	// the list is the iterator’s friend
	friend class SLList<Type>;

	// add members/methods here…
public:
	void begin();
	bool end() const;

	Type& current() const { return cur->element; }
	SLLIter<Type>& operator++();
	SLLIter(SLList<Type>& listToIterate);

private:
	SLList<Type>* iter;
	typename SLList<Type>::Node* cur, *prev;
};


/////////////////////////////////////////////////////////////////////////////
// Function : Constructor
// Parameters :	listToIterate - the list to iterate
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLLIter<Type>::SLLIter(SLList<Type>& listToIterate)
{
	iter = &listToIterate;
	cur = nullptr, prev = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : begin
// Notes : moves the iterator to the head of the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void SLLIter<Type>::begin()
{
	prev = nullptr;
	cur = iter->head;
}
/////////////////////////////////////////////////////////////////////////////
// Function : end
// Notes : returns true if we are at the end of the list, false otherwise
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
bool SLLIter<Type>::end() const
{
	if (nullptr == cur)
		return true;
	
	return false;
}
/////////////////////////////////////////////////////////////////////////////
// Function : operator++
// Notes : move the iterator forward one node
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
SLLIter<Type>& SLLIter<Type>::operator++()
{
	if (nullptr == cur)
		return *this;
	else
	{
		prev = cur;
		cur = cur->next;

		return *this;
	}
}
