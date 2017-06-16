#pragma once
#include <cstdlib>

template<class Type>
class DynArray
{
public:
	DynArray();
	~DynArray();

	DynArray(const DynArray<Type>& that);
	DynArray<Type>&	operator=(const DynArray<Type>& that);
	Type& operator[](const unsigned int index);
	const Type& operator[](const unsigned int index) const;

	unsigned int size() const;
	unsigned int capacity() const;
	void clear();
	void append(const Type& item);
	void reserve(const unsigned int & newCap = 0);

private:
	unsigned int vSize, vCap;
	Type* pointer;
};

/////////////////////////////////////////////////////////////////////////////
// Function :	Constructor
// Notes : Constructs an empty array (Size 0 Capacity 0)
/////////////////////////////////////////////////////////////////////////////
template<class Type>
DynArray<Type>::DynArray()
{
	//Defaults
	vCap = 0, vSize = 0;
	pointer = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function :	Destructor
// Notes : cleans up any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<class Type>
DynArray<Type>::~DynArray()
{
	//Deleting Vector	
	delete[] pointer;
	pointer = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function :	Copy Constructor
/////////////////////////////////////////////////////////////////////////////
template<class Type>
DynArray<Type>::DynArray(const DynArray<Type>& that)
{
	//Copying variables
	vCap = that.vCap;
	vSize = that.vSize;

	unsigned int i = 0;
	pointer = new Type[vCap];

	for (; i < capacity(); i++)
		pointer[i] = that.pointer[i];

}
/////////////////////////////////////////////////////////////////////////////
// Function :	Assignment Operator
/////////////////////////////////////////////////////////////////////////////
template<class Type>
DynArray<Type>& DynArray<Type>::operator=(const DynArray<Type>& that)
{
	//Checking for self assignment
	if (this != &that)
	{
		//Assignment
		vSize = that.vSize;
		vCap = that.vCap;

		delete[] pointer;
		pointer = nullptr;

		unsigned int i = 0;
		pointer = new Type[vCap];

		for (; i < capacity(); i++)
			pointer[i] = that.pointer[i];
	}

	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function : operator[]
// Parameters : index - the index to access
// Return : Type & - returns the array element at [index]
// Notes : performs no error checking. user should ensure index is 
//		valid with the size() method
/////////////////////////////////////////////////////////////////////////////
template<class Type>
Type& DynArray<Type>::operator[](const unsigned int index)
{
	if (size() >= 0 && index <= size())
		return pointer[index];
}

/////////////////////////////////////////////////////////////////////////////
// Function : operator[]
// Parameters : index - the index to access
// Return : const Type & - returns the array element at [index]
// Notes : performs no error checking. user should ensure index is 
//		valid with the size() method
/////////////////////////////////////////////////////////////////////////////
template<class Type>
const Type& DynArray<Type>::operator[](const unsigned int index) const
{
	if (size() >= 0 && index <= size())
		return pointer[index];
}
/////////////////////////////////////////////////////////////////////////////
// Function :	size
// Returns : int - returns the number of items being stored
// Notes : this function returns the number of items being stored, 
/////////////////////////////////////////////////////////////////////////////
template<class Type>
unsigned int DynArray<Type>::size() const
{
	return vSize;
}
/////////////////////////////////////////////////////////////////////////////
// Function : capacity
// Returns : int - returns the number of items the array can store before 
//		the next resize
// Notes : this function returns the number of items the array can store, 
//		not the number of bytes
/////////////////////////////////////////////////////////////////////////////
template<class Type>
unsigned int DynArray<Type>::capacity() const
{
	return vCap;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Function :	clear
// Notes : cleans up any dynamic memory and resets size and capacity to 0
/////////////////////////////////////////////////////////////////////////////
template<class Type>
void DynArray<Type>::clear()
{
	//Delete vector
	delete[] pointer;
	pointer = nullptr;

	//Resizing
	vCap = 0, vSize = 0;
}
/////////////////////////////////////////////////////////////////////////////
// Function : append
// Parameters : item - the item to be appended to the next open spot
// Notes : this function will append the item to the next open spot. if 
//		no room exists, the array's capacity will be doubled and then 
//		the item will be added
/////////////////////////////////////////////////////////////////////////////
template<class Type>
void DynArray<Type>::append(const Type& item)
{
	//value to copy
	unsigned int i = 0;

	//Array is empty & not empty and size = cap
	if (capacity() == 0)
	{
		vCap += 1;
	}
	else if (capacity() != 0  && capacity() == size())
	{
		vCap *= 2;
	}

	Type* temp = new Type[vCap];

	for (; i < size(); i++)
	{
		temp[i] = pointer[i];
	}
	
	//Clean up
	delete[] pointer;
	pointer = nullptr;

	//Storing new Vector
	pointer = temp;
	pointer[vSize] = item;
	vSize += 1;
}
/////////////////////////////////////////////////////////////////////////////
// Function : reserve
// Parameters : newCap - the new capacity
// Notes : 	- default parameter - reserve more space in the array, based on //		the expansion rate (100%, 1 minimum).
//		- non-default parameter, expand to the specified capacity
//		- if newCap is LESS than the current capacity, do nothing. 
//		This function should NOT make the array smaller.
/////////////////////////////////////////////////////////////////////////////
template<class Type>
void DynArray<Type>::reserve(const unsigned int & newCap = 0)
{
	//newCap less than cap, return
	if (newCap > capacity())
		vCap = newCap;

	//Array is empty & not empty and size = cap
	if (vCap == 0)
	{
		vCap = 1;
	}
	else if (newCap == 0 && vCap != 0)
	{
		vCap *= 2;
	}

	if (newCap < capacity())
		return;

	unsigned int i = 0;
	Type* temp = new Type[vCap];

	for (; i < size(); i++)
		temp[i] = pointer[i];

	//Clean up
	delete[] pointer;
	pointer = nullptr;
	//Assignment
	pointer = temp;
}