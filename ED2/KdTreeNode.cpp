#include "KdTreeNode.h"
#include "KdTree.h"

void KdTreeInternalNode::Traverse( TraversalData* pData )
{
	 //Use the PerSplitFunc from the TraversalData to determine the direction
	 //of the traversal through the tree from this node.
	
	 //If the result is 0, traverse only on the left child.
	 //If the result is 1, traverse only on the right child.
	 //If the result is 2, traverse on the left and then right child.
	 //If the result is 3, traverse on the right and then left child.
	unsigned int result = pData->m_PerSplitFunc(this, pData);
	
	if (result == 0)
		m_LeftChild->Traverse(pData);
	else if (result == 1)
		m_RightChild->Traverse(pData);
	else if (result == 2)
	{
		m_LeftChild->Traverse(pData);
		m_RightChild->Traverse(pData);
	}
	else if (result == 3)
	{
		m_RightChild->Traverse(pData);
		m_LeftChild->Traverse(pData);
	}

}

void KdTreeLeafNode::Traverse( TraversalData* pData )
{
	// The PerLeafFunc of the TraversalData should be used on this node. 
	// If the PerLeafFunc is null OR if the PerLeafFunc returns true,
	// check the PerObjectFunc pointer, and if valid call the 
	// PerObjectFunc on each object in this leaf
	if ((pData->m_PerLeafFunc == nullptr) || (pData->m_PerLeafFunc(this, pData) == true))
	{
		if (pData->m_PerObjectFunc != nullptr)
		{
			for (SceneObject* each: m_OverlappingObjects)
			{
				if (each != nullptr)
				{
					pData->m_PerObjectFunc(each, pData);
				}
			}

		}
	}
}

