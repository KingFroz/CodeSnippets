#pragma once
#include "Card.h"
/*
	Stacks use FILO (First In, Last Out) ordering
			   LIFO (Last In, First Out)

	Ex:	standard deck of cards, toilet paper, bullet magazine, Pringles,
				PEZ dispenser, etc.

*/

// Gives an alias to datatype


// A singly linked list with FILO ordering
template<class Item>
class Stack
{
private:

	struct Node
	{
		Item m_data;	// The thing to store
		Node* m_next;	// The next node in the list
	};

	Node* m_top;		// The newest node in the list
	int m_currSize;		// The number of nodes currently allocated

public:

	// Default ctor
	Stack();

	// Dtor
	~Stack();

	// Get the current size
	int GetCurrSize() const { return m_currSize;  }

	// Add to the top of the stack
	// In:	_info		The value to add
	//
	// Return: True, if added
	bool Push(const Item& _info);

	// Remove something from the top of the stack
	// In:	_info		A "blank" item
	//
	// Out: _info		The value that was at the top
	// Return: True, if something was removed
	bool Pop(Item& _info);

	// Empty out the stack for re-use
	void Clear();

	// Look at the top thing without removing it
	const Item* Peek() const;
};

// Default ctor
template<class Item>
Stack<Item>::Stack()
{
	m_currSize = 0;
	m_top = NULL;
}

// Dtor
template<class Item>
Stack<Item>::~Stack()
{
	Clear();
}


// Add to the top of the stack
// In:	_info		The value to add
//
// Return: True, if added
template<class Item>
bool Stack<Item>::Push(const Item& _info)
{
	// Make space for this new data
	Node* newTop = new Node;

	// Did new fail?
	if (!newTop)
		return false;

	// Fill in the node
	newTop->m_data = _info;
	newTop->m_next = m_top;

	// Update the top pointer
	m_top = newTop;

	m_currSize++;
	return true;
}

// Remove something from the top of the stack
// In:	_info		A "blank" item
//
// Out: _info		The value that was at the top
// Return: True, if something was removed
template<class Item>
bool Stack<Item>::Pop(Item& _info)
{
	// Is the list empty?
	if (NULL == m_top)
		return false;

	// Copy over the value
	_info = m_top->m_data;

	// Update the stack
	Node* temp = m_top;
	m_top = m_top->m_next;
	delete temp;

	m_currSize--;
	return true;
}

// Empty out the stack for re-use
template<class Item>
void Stack<Item>::Clear()
{
	Node* temp = m_top;

	// Loop until all nodes have been de
	while (m_top)
	{
		m_top = m_top->m_next;
		delete temp;
		temp = m_top;
	}

	m_currSize = 0;
}

// Look at the top thing without removing it
template<class Item>
const Item* Stack<Item>::Peek() const
{
	return (m_top ? &m_top->m_data : NULL);
}