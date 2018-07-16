#include "KdTree.h"

// BuildTree
//
// This function will recursively construct a KdTree from an initial leaf node.
void BuildTree(KdTree* pTree, KdTreeLeafNode* pLeaf)
{
	// We must first determine if this leaf can be split, and on what axis to split it.
	// Find the largest axis and it's length on the AABB.

	// The maximum size of any object is given by SceneObject::MAX_OBJECT_SIZE.
	// A leaf must be at least twice this size in order to be safely split.
	// If a leaf can not be split, then it is a final leaf in the tree.
	//		-Final leaves should be added to the leaf list of the tree.
	//		-If this leaf has no parent then it will be the root of the tree.
	//		-If this leaf does have parent then we will set the parent's first available
	//			child pointer (Left or Right) to this leaf. ($)
	//		- No further steps are needed at this point.

	// If a leaf can be split, then we will need a new internal node to represent this split.
	// The internal node's parent will need to be the parent of this leaf.
	// The internal node's dividing plane axis will be the largest axis of the leaf's AABB.
	// The internal node's plane offset will be the distance from the origin at the halflength of the AABB.

	// If the internal node has no parent then it will be the root of the tree.
	// If the internal node does have a parent then we will set the parent's first available
	//	child pointer (Left of Right) to this internal node. ($)

	// The leaf is to be split, so we will need a new leaf node.
	// The new leaf's bounds should represent the second half of the original leaf's AABB.
	// The original leaf's bounds should be adjusted to represent the first half.
	// The parents of both leaves should be set to the new internal node.

	// Recursively build the tree, using both leaves
	// NOTE:
	// BUILD ORDER MATTERS.
	// In the above algorithm the left side of any split is favored when assigning children.
	// See lines marked with ($). This assumes we are building the tree depth-first, left to right.
	vec3f dimensions = pLeaf->GetBounds().max - pLeaf->GetBounds().min;
	
	//Find largest a-xis
	int split;
	if (dimensions.x >= dimensions.y && dimensions.x >= dimensions.z) { split = 0; }
	else if (dimensions.y >= dimensions.x && dimensions.y >= dimensions.z) { split = 1; }
	else { split = 2; }

	bool safe;
	if (dimensions.v[split] > (SceneObject::MAX_OBJECT_SIZE * 2.0f))
		safe = true;
	else
		safe = false;

	if (!safe)
	{
		pTree->m_LeafList.push_back(pLeaf);
		if (pLeaf->GetParent() == nullptr)
		{
			pTree->m_Root = pLeaf;
		}
		else
		{
			if (pLeaf->GetParent()->m_LeftChild == nullptr)
				pLeaf->GetParent()->m_LeftChild = pLeaf;
			
			else if (pLeaf->GetParent()->m_RightChild == nullptr)
				pLeaf->GetParent()->m_RightChild = pLeaf;
		}
	}
	else
	{
		KdTreeInternalNode *node = new KdTreeInternalNode();
		node->m_Parent = pLeaf->GetParent();
		node->m_PlaneAxis = split;
		node->m_PlaneOffset = (pLeaf->GetBounds().max.v[split] + pLeaf->GetBounds().min.v[split]) / 2.0f;
		if (node->GetParent() == nullptr)
		{
			pTree->m_Root = node;
		}
		else
		{
			if (node->GetParent()->m_LeftChild == nullptr)
				node->GetParent()->m_LeftChild = node;

			else if (node->GetParent()->m_RightChild == nullptr)
				node->GetParent()->m_RightChild = node;
		}

		// The leaf is to be split, so we will need a new leaf node.
		// The new leaf's bounds should represent the second half of the original leaf's AABB.
		// The original leaf's bounds should be adjusted to represent the first half.
		// The parents of both leaves should be set to the new internal node.

		KdTreeLeafNode *newLeaf = new KdTreeLeafNode();
		
		newLeaf->m_Bounds = pLeaf->GetBounds();
		newLeaf->m_Bounds.min.v[split] = node->m_PlaneOffset;
		pLeaf->m_Bounds.max.v[split] = node->m_PlaneOffset;

		pLeaf->m_Parent = node;
		newLeaf->m_Parent = node;
		
		BuildTree(pTree, pLeaf);
		BuildTree(pTree, newLeaf);

	}

}

// CreateTree
//
// This function will create a new KdTree based on the provided sceneBounds
KdTree* KdTree::CreateTree(const AABB& sceneBounds)
{
	// Create a new KdTree and an initial Leaf.
	// The bounds of BOTH should be set to the sceneBounds.
	// Build the tree.
	// Return the result.
	KdTree *Tree = new KdTree();
	KdTreeLeafNode * leaf = new KdTreeLeafNode();

	Tree->m_SceneBounds = sceneBounds;
	leaf->m_Bounds = sceneBounds;

	BuildTree(Tree, leaf);

	return Tree;
}

KdTree::~KdTree()
{
	// Remove all objects from the master object list and delete the root node.
	m_MasterObjectList.clear();
	delete m_Root;
}

void KdTree::Traverse( TraversalData* pData )
{
	// Call Traverse() from the root of the tree.
	m_Root->Traverse(pData);
}

void BuildAABB( AABB& aabb, const vec3f* pts, unsigned int ptCount )
{
	// For all the provided points, find the highest X, highest Y,
	// highest Z, lowest X, lowest Y, and lowest Z.
	// Use these values to build the AABB.
	aabb.max = { -FLT_MAX, -FLT_MAX , -FLT_MAX };
	aabb.min = { FLT_MAX, FLT_MAX , FLT_MAX };

	for (int i = 0; i < ptCount; i++)
	{
		for (int x = 0; x < 3; x++)
		{
			if (pts[i].v[x] > aabb.max.v[x])
			{
				aabb.max.v[x] = pts[i].v[x];
			}
			
			if (pts[i].v[x] < aabb.min.v[x])
			{
				aabb.min.v[x] = pts[i].v[x];
			}

		}
	}
}

FrustumAABB::FrustumAABB(Frustum* pFrustum)
{
	// This function is completed for you
	m_Frustum = pFrustum;
	BuildAABB( m_Aabb, pFrustum->corners, 8 );
}

unsigned int FrustumAABB::FrustumAABBtoSplit(KdTreeInternalNode* pNode, TraversalData* pData)
{
	// This function is completed for you
	FrustumAABB* pFrustumAABB = (FrustumAABB*)pData->m_pVoid;
	return KdTree::ClassifyAABBToAAPlane( pFrustumAABB->m_Aabb, pNode->GetPlaneAxis(), pNode->GetPlaneOffset() );
}

unsigned int KdTree::ClassifyAABBToAAPlane( const AABB& aabb, unsigned int planeIndex, float planeOffset )
{
	 // Classify an AABB to an axis aligned plane.
	 // If the AABB is completely behind the plane, return 0.
	 // If the AABB is completely in front of the plane, return 1.
	 // If the AABB straddles the plane, return 2.
	if (aabb.max.v[planeIndex] < planeOffset)
		return 0;
	else if(aabb.min.v[planeIndex] > planeOffset)
		return 1;
	else
		return 2;
	
	/* TESTING AN AXIS-ALIGNED PLANE IS SIMPLER THAN TESTING A GENERIC PLANE */
	/* DO NOT USE THE ClassifyAabbToPlane FUNCTION FROM THE COLLISION LIBRARY */
}

unsigned int KdTree::ClassifySphereToAAPlane( const Sphere& sphere, unsigned int planeIndex, float planeOffset )
{
	// Classify a sphere to an axis aligned plane.
	// If the sphere is completely behind the plane, return 0.
	// If the sphere is completely in front of the plane, return 1.
	// If the sphere straddles the plane, return 2.
	if (sphere.m_Center.v[planeIndex] + sphere.m_Radius < planeOffset)
		return 0;
	else if (sphere.m_Center.v[planeIndex] - sphere.m_Radius > planeOffset)
		return 1;
	else
		return 2;
	/* TESTING AN AXIS-ALIGNED PLANE IS SIMPLER THAN TESTING A GENERIC PLANE */
	/* DO NOT USE THE ClassifySphereToPlane FUNCTION FROM THE COLLISION LIBRARY */
}

unsigned int KdTree::ClassifyObjectPerSplitFunc( KdTreeInternalNode* pNode, TraversalData* pData )
{
	// In this function, the optional data member of the TraversalData represents a SceneObject*.
	// i.e. pData->m_pVoid points to a SceneObject.
	// Return the result of ClassifySphereToAAPlane using this object's bounds and the internal nodes plane.
	unsigned int result = ClassifySphereToAAPlane(((SceneObject*)pData->m_pVoid)->m_Bounds, pNode->GetPlaneAxis(), pNode->GetPlaneOffset());
	return result;
}

bool KdTree::InsertObjectPerLeafFunc( KdTreeLeafNode* pNode, TraversalData* pData )
{
	// In this function, the optional data member of the TraversalData represents a SceneObject*.
	// i.e. pData->m_pVoid points to a SceneObject.

	// Add the object to this leaf's object list.
	// Add this leaf to the object's leaf list at the first available spot.
	SceneObject* temp = ((SceneObject*)pData->m_pVoid);
	pNode->m_OverlappingObjects.push_back(temp);

	for (int i = 0; i < 8; i++)
	{
		if (temp->m_OverlappedLeaves[i] == NULL)
		{
			temp->m_OverlappedLeaves[i] = pNode;
			return true;
		}
	}

	return false;
}

void KdTree::UpdateObject( SceneObject* pObject )
{
	// Constrain this object to the scene bounds.
	// If the object is contained by its leaves then no further processing is needed.
	// If the object is not contained then it must be reinserted into the tree.
	//		- Remove the object from its current leaves
	//		- Use a TraversalData object and a call to KdTree::Traverse to reinsert the object into the KdTree.
	//			- The necessary PerSplitFunc and PerLeafFunc are static members of KdTree. 
	//			- All members of the TraversalData object must be set except the PerObjectFunc
	pObject->ConstrainToAABB(m_SceneBounds);

	if (!pObject->IsContained())
	{
		pObject->RemoveFromLeaves();
		TraversalData pData;

		pData.m_pVoid = pObject;
		pData.m_PerLeafFunc = InsertObjectPerLeafFunc;
		pData.m_PerSplitFunc = ClassifyObjectPerSplitFunc;
		KdTree::Traverse(&pData);
	}
	
}

void KdTree::InsertObject( SceneObject* pObject )
{
	// First try to remove this object from the tree.
	// If the object's tree member is not now null,
	// then it must belong to another tree and we can not
	// add it to this one under the current system.
	RemoveObject(pObject);
	// Otherwise, set the object's tree to this.
	// Add the object to the master list of objects.
	// Update the object to insert it into the tree.
	if (pObject->m_Tree == nullptr)
	{
		pObject->m_Tree = this;
		m_MasterObjectList.push_back(pObject);
		UpdateObject(pObject);
	}
}

void KdTree::RemoveObject( SceneObject* pObject )
{
	// If the tree of the object is not this tree
	//	then we can not remove it.
	if (pObject->m_Tree == this)
	{
		// Remove the object from it's current leaves.
		pObject->RemoveFromLeaves();

		// Remove the object from the master list of objects.
		// Set the objects tree to null.
		m_MasterObjectList.remove(pObject);
		pObject->m_Tree = NULL;
	}
}

void KdTree::Update(void)
{
	// For all objects in the master list, update each one
	auto iter = m_MasterObjectList.begin();
	for (; iter != m_MasterObjectList.end(); iter++)
	{
		UpdateObject(*iter);
	}
	
}