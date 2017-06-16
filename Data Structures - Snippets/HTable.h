#pragma once
#include "SLList.h"

template<typename Type> 
class HTable
{
public:
	HTable(unsigned int numOfBuckets, unsigned int(*hFunction) (const Type &v));
	~HTable();
	HTable<Type>& operator=(const HTable<Type>& that);
	HTable(const HTable<Type>& that);

	bool findAndRemove(const Type& v);
	int find(const Type& v) const;
	void insert(const Type& v);
	void clear();

	unsigned int(*pFunc) (const Type &v);
private:
	unsigned int buckets;
	SLList<Type> *HTab;

};

/////////////////////////////////////////////////////////////////////////////
// Function : Constructor 
// Parameters : numOfBuckets - the number of buckets
//              hFunction - pointer to the hash function for this table
// Notes : constructs an empty hash table
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
HTable<Type>::HTable(unsigned int numOfBuckets, unsigned int(*hFunction) (const Type &v))
{
	pFunc = hFunction;
	buckets = numOfBuckets;
	HTab = new SLList<Type>[numOfBuckets];
}
/////////////////////////////////////////////////////////////////////////////
// Function : Destructor
// Notes : destroys a hash table
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
HTable<Type>::~HTable()
{
	delete[] HTab;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Assignment Operator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
HTable<Type>& HTable<Type>::operator=(const HTable<Type>& that)
{
	if (this != &that)
	{
		HTable<Type>::clear();
		delete[] HTab;

		buckets = that.buckets;
		pFunc = that.pFunc;

		unsigned int x = 0;
		HTab = new SLList<Type>[buckets];

		for (; x < buckets; x++)
			HTab[x] = that.HTab[x];

	}

	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Copy Constructor
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
HTable<Type>::HTable(const HTable<Type>& that)
{
	buckets = that.buckets;
	pFunc = that.pFunc;

	unsigned int x = 0;
	HTab = new SLList<Type>[buckets];

	for (; x < buckets; x++)
		HTab[x] = that.HTab[x];
	
}
/////////////////////////////////////////////////////////////////////////////
// Function : insert
// Parameters : v - the item to insert into the hash table
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void HTable<Type>::insert(const Type& v)
{
	HTab[pFunc(v)].addHead(v);
}
/////////////////////////////////////////////////////////////////////////////
// Function : findAndRemove
// Parameters : v - the item to remove(if it is found)
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
bool HTable<Type>::findAndRemove(const Type& v)
{
	//Locate and Remove
	SLLIter<Type> loc(HTab[pFunc(v)]);
	loc.begin();

	while (loc.end() == false)
	{
		if (loc.current() == v)
		{
			HTab[pFunc(v)].remove(loc);
			return true;
		}

		++loc;
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////
// Function : clear
// Notes : clears the hash table
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void HTable<Type>::clear()
{
	unsigned int x = 0;
	for (; x < buckets; x++ )
		HTab[x].clear();
}
/////////////////////////////////////////////////////////////////////////////
// Function : find
// Parameters : v - the item to look for
// Return : the bucket we found the item in or -1 if we didn’t find the 
//item.
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
int HTable<Type>::find(const Type& v) const
{
	//Locate and Compare
	SLLIter<Type> loc(HTab[pFunc(v)]);
	loc.begin();

	while (loc.end() == false)
	{
		if (loc.current() == v)
			return pFunc(v);
		
		++loc;
	}

	return -1;
}

