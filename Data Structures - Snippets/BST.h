#pragma once

template<typename Type>
class BST
{
private:
	struct Node
	{
		Node* left;
		Node* right;
		Type element;

		Node()
		{
			left = nullptr, right = nullptr;
		}
	};

	Node * root;
public:
	BST();
	~BST();

	BST& operator=(const BST& that);
	BST(const BST& that);

	void insert(const Type& v);
	void clear();

	bool findAndRemove(const Type& v);
	bool find(const Type& v) const;

	void inOrder(const Node *n) const;
	void postOrder(const Node *n) const;
	void preOrder(const Node *n);
	void printInOrder() const;
};

//Post Order
template<typename Type>
void BST<Type>::postOrder(const Node *n) const
{
	if (nullptr != n)
	{
		postOrder(n->left);
		postOrder(n->right);
		delete n;
	}
}

//Pre Order
template<typename Type>
void BST<Type>::preOrder(const Node *n)
{
	if (nullptr != n)
	{
		insert(n->element);
		preOrder(n->left);
		preOrder(n->right);
	}
}

//In Order
template<typename Type>
void BST<Type>::inOrder(const Node *n) const
{
	if (nullptr != n)
	{
		inOrder(n->left);
		cout << n->element << ' ';
		inOrder(n->right);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Function : Constuctor
// Notes : constucts an empty BST
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
BST<Type>::BST()
{
	root = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : Destructor
// Notes : destroys the BST cleaning up any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
BST<Type>::~BST()
{
	clear();
}
/////////////////////////////////////////////////////////////////////////////
// Function : assignment operator
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
BST<Type>& BST<Type>::operator=(const BST& that)
{
	if (this != &that)
	{
		preOrder(that.root);
	}

	return *this;
}
/////////////////////////////////////////////////////////////////////////////
// Function: copy constructor
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
BST<Type>::BST(const BST& that)
{
	root = nullptr;

	clear();
	preOrder(that.root);
}
/////////////////////////////////////////////////////////////////////////////
// Function : insert
// Parameters :  v - the item to insert 
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void BST<Type>::insert(const Type& v)
{
	Node* tmp = new Node();
	tmp->element = v;

	Node* cur;

	if (nullptr == root)
	{
		root = tmp;
		return;
	}

	cur = root;

	if (tmp->element < cur->element)
	{
		if (nullptr == root->left)
		{
			cur->left = tmp;
			return;
		}
		else
		{
			cur = root->left;

			while (true)
			{
				if (tmp->element < cur->element)
				{
					if (nullptr == cur->left )
					{
						cur->left = tmp;
						return;
					}
					else
						cur = cur->left;
				}

				if (tmp->element >= cur->element)
				{
					if (nullptr == cur->right)
					{
						cur->right = tmp;
						return;
					}
					else
						cur = cur->right;
				}
			}
		}
	}
	
	if (tmp->element >= cur->element)
	{
		if (nullptr == cur->right)
		{
			cur->right = tmp;
			return;
		}
		else
		{
			cur = root->right;

			while (true)
			{
				if (tmp->element < cur->element)
				{
					if (nullptr == cur->left)
					{
						cur->left = tmp;
						return;
					}
					else
						cur = cur->left;
				}

				if (tmp->element >= cur->element)
				{
					if (nullptr == cur->right)
					{
						cur->right = tmp;
						return;
					}
					else
						cur = cur->right;
				}
			}
		}
		
	}
}
/////////////////////////////////////////////////////////////////////////////
// Function : findAndRemove
// Parameters : v - the item to find (and remove if it is found)
// Return : bool - true if the item was removed, false otherwise
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
bool BST<Type>::findAndRemove(const Type& v)
{ 
	if (find(v) != true)
		return false;
	
	Node *parent;
	Node *cur = root;

	while (nullptr != cur)
	{
		if (v == cur->element)
			break;

		parent = cur;

		if (cur->element < v)
			cur = cur->right;
		else
			cur = cur->left;
	}

	if (nullptr == cur)
		return false;

	if (nullptr == cur->right && nullptr == cur->left)
	{
		if (root == cur)
			root = nullptr;

		else if (parent->right == cur)
			parent->right = nullptr;
		else
			parent->left = nullptr;

		delete cur;

		return true;
	}

	if (nullptr != cur->right && nullptr == cur->left || nullptr == cur->right && nullptr != cur->left)
	{
		if (nullptr != cur->right && nullptr == cur->left)
		{
			if (root == cur)
				root = cur->right;

			else if (parent->right == cur)
				parent->right = cur->right;
			else
				parent->left = cur->right;

			delete cur;
			return true;
		}
		else if (nullptr == cur->right && nullptr != cur->left)
		{
			if (root == cur)
				root = cur->left;

			else if (parent->right == cur)
				parent->right = cur->left;
			else
				parent->left = cur->left;

			delete cur;
			return true;
		}
	}

	if (nullptr != cur->right && nullptr != cur->left)
	{
		Node *rMp;
		Node *rM = cur->right;

		cur->element = rM->element;

		if (rM->left != nullptr)
		{

			while (rM->left != nullptr)
			{
				rMp = rM;
				rM = rM->left;
			}

			cur->element = rM->element;

			if (rM->right == nullptr)
				rMp->left = nullptr;
			else
				rMp->left = rM->right;

			delete rM;
			return true;
		}
		else if (nullptr == rM->right && nullptr == rM->left)
		{
			cur->element = rM->element;
			delete rM;
			return true;
		}
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////
// Function : find
// Parameters : v - the item to find
// Return : bool - true if the item was found, false otherwise
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
bool BST<Type>::find(const Type& v) const
{
	Node *cur = root;

	while (nullptr != cur)
	{
		if (v == cur->element)
			return true;

		else if (cur->element < v)
			cur = cur->right;
		else if (cur->element > v)
			cur = cur->left;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Function : clear
// Notes : clears the BST, cleaning up any dynamic memory
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void BST<Type>::clear()
{
	postOrder(root);
	root = nullptr;
}
/////////////////////////////////////////////////////////////////////////////
// Function : printInOrder
// Notes : prints the contents of the BST to the screen, in ascending order
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void BST<Type>::printInOrder() const
{
	inOrder(root);
	cout << endl;
}
