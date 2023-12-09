#include "Octant.h"
using namespace BTX;
//  Octant
uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	/*
	* This constructor is meant to be used ONLY on the root node, there is already a working constructor
	* that will take a size and a center to create a new space
	*/
	Init();//Init the default values
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	//create a rigid body that encloses all the objects in this octant, it necessary you will need
	//to subdivide the octant based on how many objects are in it already an how many you IDEALLY
	//want in it, remember each subdivision will create 8 children for this octant but not all children
	//of those children will have children of their own

	//The following is a made-up size, you need to make sure it is measuring all the object boxes in the world
	std::vector<vector3> lMinMax;

	//Add all the mins and maxes to a vector of vector3s
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		RigidBody* tempRigidBody = m_pEntityMngr->GetEntity(i)->GetRigidBody();
		lMinMax.push_back(tempRigidBody->GetMinGlobal());
		lMinMax.push_back(tempRigidBody->GetMaxGlobal());

	}

	//Creating a new rigid body with this list uses all of them to create an object with the correct size
	RigidBody* pRigidBody = new RigidBody(lMinMax);
	
	vector3 halfWidth = pRigidBody->GetHalfWidth();
	float fMax = halfWidth.x;

	//Get the greatest halfwidth of the shape
	for (int i = 0; i < 3; i++)
	{
		if (fMax < halfWidth[i])
			fMax = halfWidth[i];
	}

	vector3 v3Center = pRigidBody->GetCenterLocal();


	//Set the size, center, min, and max of the root of the tree
	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	SafeDelete(pRigidBody);
	lMinMax.clear();
	m_uOctantCount++; //When we add an octant we increment the count
	ConstructTree(m_uMaxLevel); //Construct the children

}

bool Octant::IsColliding(uint a_uRBIndex)
{
	//Get how many objects there are in the world
	//If the index given is larger than the number of elements in the bounding object there is no collision
	//As the Octree will never rotate or scale this collision is as easy as an Axis Alligned Bounding Box
	//Get all vectors in global space (the octant ones are already in Global)
	int numObjects = m_pEntityMngr->GetEntityCount();

	//There is no collision if the index given is larger than the number of elements in the bounding object
	if(a_uRBIndex >= numObjects)
	{
		return false;
	}

	//globalize the vectors
	Entity* currentEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	RigidBody* currentRigidBody = currentEntity->GetRigidBody();

	vector3 v3Min = currentRigidBody->GetMinGlobal();
	vector3 v3Max = currentRigidBody->GetMaxGlobal();

	//check the vectors of x,y,z
	if (m_v3Max.x < v3Min.x)
		return false;
	if (m_v3Min.x > v3Max.x)
		return false;
	if (m_v3Max.y < v3Min.y)
		return false;
	if (m_v3Min.y > v3Max.y)
		return false;
	if (m_v3Max.z < v3Min.z)
		return false;
	if (m_v3Min.z > v3Max.z)
		return false;


	return true; // they are colliding
}
void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// Display the specified octant
	if (m_uID == a_nIndex)
	{
		m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color);
		return;
	}
	//Display all the children of this octant recursively 
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_nIndex);
	}
}
void Octant::Display(vector3 a_v3Color)
{
	//this is meant to be a recursive method, in starter code will only display the root
	//even if other objects are created
	
	//Run this method on all children until it gets to a root octant, then display it
	for (int c = 0; c < m_uChildren; c++)
	{
		m_pChild[c]->Display(a_v3Color);
	}
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color);
	
}
void Octant::Subdivide(void)
{
	//If this node has reach the maximum depth return without changes
	if (m_uLevel >= m_uMaxLevel)
		return;

	//If this node has been already subdivided return without changes
	if (m_uChildren != 0)
		return;

	
	//Subdivide the space and allocate 8 children
	m_uChildren = 8;

	//Create the sizes and the center for the children
	float fSize = m_fSize / 4.0f;
	float childSize = fSize * 2.0f;
	vector3 v3Center;
	v3Center = m_v3Center;
	
	v3Center.x -= fSize;
	v3Center.y += fSize;
	v3Center.z -= fSize;

	//Top left back
	m_pChild[0] = new Octant(v3Center, childSize);

	
	//Top right back
	v3Center.x += childSize;
	m_pChild[1] = new Octant(v3Center, childSize);

	//Top left front
	v3Center.z += childSize;
	v3Center.x -= childSize;
	m_pChild[2] = new Octant(v3Center, childSize);

	//Top right front
	v3Center.x += childSize;
	m_pChild[3] = new Octant(v3Center, childSize);
	
	//Bottom left back
	v3Center.x -= childSize;
	v3Center.y -= childSize;
	v3Center.z -= childSize;
	m_pChild[4] = new Octant(v3Center, childSize);

	//Bottom right back
	v3Center.x += childSize;
	m_pChild[5] = new Octant(v3Center, childSize);

	//Bottom left front
	v3Center.z += childSize;
	v3Center.x -= childSize;
	m_pChild[6] = new Octant(v3Center, childSize);

	//Bottom right front
	v3Center.x += childSize;
	m_pChild[7] = new Octant(v3Center, childSize);


	//Create eachh child and set the roots, parents, and levels.
	for (int i = 0; i < 8; i++) {

		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		//Subdivide again if the child contains more objects than the ideal entity count set at the start
		if (m_pChild[i]->ContainsAtLeast(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}

	}

}
bool Octant::ContainsAtLeast(uint a_nEntities)
{
	int collisionCount = 0;

	//checks each object and returns true if the number of objects inside this octant is greater than
	//the number inputted into the function
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		if (IsColliding(i))
			collisionCount++;
		if (collisionCount > a_nEntities)
			return true;
	}
	
	return false; //return false if there aren't too many objects inside the octant
}
void Octant::AssignIDtoEntity(void)
{
	//Runs the function on each child
	for (int c = 0; c < m_uChildren; c++)
	{
		m_pChild[c]->AssignIDtoEntity();
	}
	//If the current octant is a leaf
	if (m_uChildren == 0)
	{
		//Checks all the entities to see if any are inside this octant, if so, assign the id of this
		//octant to the entity.
		int numEntities = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < numEntities; i++) 
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}

	//Recursive method
	//Have to traverse the tree and make sure to tell the entity manager
	//what octant (space) each object is at
	//m_pEntityMngr->AddDimension(0, m_uID);//example only, take the first entity and tell it its on this space
}
//-------------------------------------------------------------------------------------------------------------------
// You can assume the following is fine and does not need changes, you may add onto it but the code is fine as is
// in the proposed solution.
void Octant::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}
void Octant::Swap(Octant& other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
void Octant::Release(void)
{
	//this is a special kind of release, it will only happen for the root
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}
void Octant::ConstructTree(uint a_nMaxLevel)
{
	//If this method is tried to be applied to something else
	//other than the root, don't.
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel; //Make sure you mark which is the maximum level you are willing to reach
	m_uOctantCount = 1;// We will always start with one octant

	//If this was initialized before make sure its empty
	m_EntityList.clear();//Make sure the list of entities inside this octant is empty
	KillBranches();
	m_lChild.clear();

	//If we have more entities than those that we ideally want in this octant we subdivide it
	if (ContainsAtLeast(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();//Add octant ID to Entities
	ConstructList();//construct the list of objects
}
//The big 3
Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	//Will create the octant object based on the center and size but will not construct children
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}
Octant::Octant(Octant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}
Octant& Octant::operator=(Octant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}
	return *this;
}
Octant::~Octant() { Release(); };
//Accessors
float Octant::GetSize(void) { return m_fSize; }
vector3 Octant::GetCenterGlobal(void) { return m_v3Center; }
vector3 Octant::GetMinGlobal(void) { return m_v3Min; }
vector3 Octant::GetMaxGlobal(void) { return m_v3Max; }
uint Octant::GetOctantCount(void) { return m_uOctantCount; }
bool Octant::IsLeaf(void) { return m_uChildren == 0; }
Octant* Octant::GetParent(void) { return m_pParent; }
//--- other methods
Octant * Octant::GetChild(uint a_nChild)
{
	//if its asking for more than the 8th children return nullptr, as we don't have one
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}
void Octant::KillBranches(void)
{
	/*This method has recursive behavior that is somewhat hard to explain line by line but
	it will traverse the whole tree until it reaches a node with no children and
	once it returns from it to its parent it will kill all of its children, recursively until
	it reaches back the node that called it.*/
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}
void Octant::DisplayLeaves(vector3 a_v3Color)
{
	/*
	* Recursive method
	* it will traverse the tree until it find leaves and once found will draw them
	*/
	uint nLeafs = m_lChild.size(); //get how many children this will have (either 0 or 8)
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeaves(a_v3Color);
	}
	//Draw the cube
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
}
void Octant::ClearEntityList(void)
{
	/*
	* Recursive method
	* It will traverse the tree until it find leaves and once found will clear its content
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}
void Octant::ConstructList(void)
{
	/*
	* Recursive method
	* It will traverse the tree adding children
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}
	//if we find a non-empty child add it to the tree
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
