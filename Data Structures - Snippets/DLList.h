#pragma once
template<typename Type> class DLLIter;

template<typename Type> class DLList
{
	friend class DLLIter<Type>;
private:
	struct Node
	{
		Node* next, *prev;
		Type element;
	};

	Node* head, *tail;
public:
	DLList();
	~DLList();

	void clear();
	void recur(Node* _copy);
	void addHead(const Type& v);
	void addTail(const Type& v);

	void insert(DLLIter<Type>& index, const Type& v);
	void remove(DLLIter<Type>& index);

	DLList<Type>& operator=(const DLList<Type>& that);
	DLList(const DLList<Type>& that);
};



/////////////////////////////////////////////////////////////////////////////
// Function : Constructor
// Notes : constructs an empty list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLList<Type>::DLList()
{
	head = nullptr, tail = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Destructor
// Notes : Destroys a list
/////////////////////////////////////////////////////////////////////////////

//Recursive Copy
template<typename Type>
void DLList<Type>::recur(Node* _copy)
{
	if (nullptr != _copy)
	{
		addTail(_copy->element);
		recur(_copy->next);
	}
}

template<typename Type>
DLList<Type>::~DLList()
{
	clear();
}
/////////////////////////////////////////////////////////////////////////////
// Function : Assignment Operator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLList<Type>& DLList<Type>::operator=(const DLList<Type>& that)
{
	if (this != &that)
	{
		clear();
		recur(that.head);
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Copy Constructor
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLList<Type>::DLList(const DLList<Type>& that)
{
	head = nullptr, tail = nullptr;
	*this = that;
}
/////////////////////////////////////////////////////////////////////////////
// Function : addHead
// Parameters : v - the item to add to the head of the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLList<Type>::addHead(const Type& v)
{
	if (head == nullptr)
	{
		head = tail = nullptr;

		Node* tmp = new Node;
		tmp->element = v;

		head = tmp;
		tail = tmp;

		tmp->next = nullptr;
		tmp->prev = nullptr;
	}
	else
	{
		Node* tmp = new Node;
		tmp->element = v;

		head->prev = tmp;
		tmp->next = head;

		tmp->prev = nullptr;
		head = tmp;
	}

}
/////////////////////////////////////////////////////////////////////////////
// Function : addTail
// Parameters : v - the item to add to the tail of the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLList<Type>::addTail(const Type& v)
{
	if (tail == nullptr)
	{
		head = tail = nullptr;

		Node* tmp = new Node;
		tmp->element = v;

		head = tmp;
		tail = tmp;

		tmp->next = nullptr;
		tmp->prev = nullptr;
	}
	else
	{
		Node* tmp = new Node;
		tmp->element = v;
		
		tail->next = tmp;
		tmp->prev = tail;

		tmp->next = nullptr;
		tail = tmp;
	}

}
/////////////////////////////////////////////////////////////////////////////
// Function : clear
// Notes : clears the list, freeing any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLList<Type>::clear()
{
	Node* tmp;

	while (nullptr != head)
	{

		tmp = head;
		head = head->next;
		delete tmp;
	}

	tail = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : insert
// Parameters :	index - an iterator to the location to insert at
//				v - the item to insert
// Notes : do nothing on a bad iterator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLList<Type>::insert(DLLIter<Type>& index, const Type& v)
{
	if (nullptr != index.iter)
	{
		Node* tmp = new Node;
		tmp->element = v;

		tmp->prev = index.iter->prev;
		tmp->next = index.iter;

		if (index.iter == head)
			head = tmp;

		if (nullptr != tmp->prev)
			index.iter->prev->next = tmp;

		tmp->next->prev = tmp;
		index.iter = tmp;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Function : remove
// Parameters :	index - an iterator to the location to remove from
// Notes : do nothing on a bad iterator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLList<Type>::remove(DLLIter<Type>& index)
{
	if (nullptr != index.iter)
	{
		Node* tmp = index.iter;

		if (tmp->next)
			tmp->next->prev = tmp->prev;

		if (tmp->prev)
			tmp->prev->next = tmp->next;

		if (index.iter == head)
			head = head->next;

		if (index.iter == tail)
			tail = tail->prev;

		index.iter = tmp->next;
		delete tmp;
	}
}

//class DLL
template<typename Type> class DLLIter
{
	friend class DLList<Type>;

private:
	DLList<Type>* list;
	typename DLList<Type>::Node *iter;
public:
	bool end() const;

	void beginHead();
	void beginTail();

	Type& current() const;
	DLLIter<Type>& operator++();
	DLLIter<Type>& operator--();
	DLLIter(DLList<Type>& listToIterate);
};

/////////////////////////////////////////////////////////////////////////////
// Function : Constructor
// Parameters :	listToIterate - the list to iterate
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLLIter<Type>::DLLIter(DLList<Type>& listToIterate)
{
	iter = nullptr;
	list = &listToIterate;
}
/////////////////////////////////////////////////////////////////////////////
// Function : beginHead
// Notes : moves the iterator to the head of the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLLIter<Type>::beginHead()
{
	iter = list->head;
}
/////////////////////////////////////////////////////////////////////////////
// Function : beginTail
// Notes : moves the iterator to the tail of the list
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void DLLIter<Type>::beginTail()
{
	iter = list->tail;
}
/////////////////////////////////////////////////////////////////////////////
// Function : end
// Notes : returns true if we are at the end of the list, false otherwise
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
bool DLLIter<Type>::end() const
{
	if (nullptr == iter)
		return true;

	return false;
}
/////////////////////////////////////////////////////////////////////////////
// Function : operator++
// Notes : move the iterator forward one node
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLLIter<Type>& DLLIter<Type>::operator++()
{
	if (nullptr != iter)
	{
		iter = iter->next;
		return *this;
	}
	else
		return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function : operator--
// Notes : move the iterator backward one node
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
DLLIter<Type>& DLLIter<Type>::operator--()
{
	if (nullptr != iter)
	{
		iter = iter->prev;
		return *this;
	}
	else
		return *this;
}

template<typename Type>
Type& DLLIter<Type>::current() const
{
	return iter->element;
}