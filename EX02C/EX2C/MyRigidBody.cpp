#pragma region DO NOT CHANGE
#include "MyRigidBody.h"
using namespace BTX;

//Helper function, feel free to use it, it will take one vector from local to global space
vector3 MyRigidBody::Globalize(vector3 a_v3Input)
{
	return m_m4ToWorld * vector4(a_v3Input, 1.0f);
}
#pragma endregion

void MyRigidBody::Update(void)
{
#pragma region DO NOT CHANGE
	static quaternion qDirection;
	qDirection = qDirection * glm::angleAxis(glm::radians(1.0f), AXIS_Z);
#pragma endregion

	//Update the m_v3Direction of the arrow, this is the global Y axis oriented by qDirection (as you did Forward in camera assigment)
	m_v3Direction = qDirection * AXIS_Y;
}
//Allocation
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
#pragma region DO NOT CHANGE
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;
#pragma endregion

	//replace the following with your code

	//Set the basis for the mins and maxes
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Set the mins and maxes for each point
	for (int i = 0; i < uVertexCount; i++)
	{
		if (m_v3MaxL.x < a_pointList[i].x)
			m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x)
			m_v3MinL.x = a_pointList[i].x;
		if (m_v3MaxL.y < a_pointList[i].y)
			m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y)
			m_v3MinL.y = a_pointList[i].y;
		if (m_v3MaxL.z < a_pointList[i].z)
			m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z)
			m_v3MinL.z = a_pointList[i].z;
	}

	//Local and global are the same
	m_v3MaxG = m_v3MaxL;
	m_v3MinG = m_v3MinL;
	//Center is half of the min and max added
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;
	//half width is half of max - min
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Radius is the distance between the center and the furthest point from it
	m_fRadius = glm::distance(m_v3Center, m_v3MaxL);
}
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
#pragma region DO NOT CHANGE
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//replace the following with your code
	m_m4ToWorld = a_m4ModelMatrix;
#pragma endregion
	//Array of corners for the bounding box, first 4 are the front and the last four are the back
	vector3 corners[8];

	corners[0] = m_v3MinL;
	corners[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	corners[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	corners[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);
	corners[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	corners[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	corners[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	corners[7] = m_v3MaxL;

	//Globalize the points
	for (int i = 0; i < 8; i++)
	{
		corners[i] = Globalize(corners[i]);
	}

	//Set the basis for the max and min to the first corner
	m_v3MaxG = m_v3MinG = corners[0];


	//Set all the maxes and mins
	for (int i = 0; i < 8; i++)
	{
		if (m_v3MaxG.x < corners[i].x)
			m_v3MaxG.x = corners[i].x;
		else if (m_v3MinG.x > corners[i].x)
			m_v3MinG.x = corners[i].x;
		if (m_v3MaxG.y < corners[i].y)
			m_v3MaxG.y = corners[i].y;
		else if (m_v3MinG.y > corners[i].y)
			m_v3MinG.y = corners[i].y;
		if (m_v3MaxG.z < corners[i].z)
			m_v3MaxG.z = corners[i].z;
		else if (m_v3MinG.z > corners[i].z)
			m_v3MinG.z = corners[i].z;
	}
	//The size of the bounding box is the max minus the min
	m_v3ARBBSize = m_v3MaxG - m_v3MinG; 
}
bool MyRigidBody::TestARBB(MyRigidBody* const other)
{
	//Check if the max is less than the min and if the min is greater than the max for all the axes. 
	//This will 
	//Return false if it is.

	if (this->m_v3MaxG.x < other->m_v3MinG.x) 
		return false;
	if (this->m_v3MinG.x > other->m_v3MaxG.x) 
		return false;
	
	if (this->m_v3MaxG.y < other->m_v3MinG.y) 
		return false;
	
	if (this->m_v3MinG.y > other->m_v3MaxG.y) 
		return false;
	
	if (this->m_v3MaxG.z < other->m_v3MinG.z) 
		return false;
	
	if (this->m_v3MinG.z > other->m_v3MaxG.z) 
		return false;
	
	//Return true if none of the collision tests return false
	return true;
}
#pragma region DO NOT CHANGE
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	//check if spheres are colliding
	bool bColliding = TestARBB(other);
	
	//if they are check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		if (bColliding) //they are colliding with bounding box also
		{
			this->AddCollisionWith(other);
			other->AddCollisionWith(this);
		}
		else //they are not colliding with bounding box
		{
			this->RemoveCollisionWith(other);
			other->RemoveCollisionWith(this);
		}
	}
	else //they are not colliding with bounding sphere
	{
		this->RemoveCollisionWith(other);
		other->RemoveCollisionWith(this);
	}
	return bColliding;
}
void MyRigidBody::Init(void)
{
	m_pModelMngr = ModelManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = true;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pModelMngr, other.m_pModelMngr);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_CollidingRBSet, other.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pModelMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void) { return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }



//The big 3

MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pModelMngr = other.m_pModelMngr;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3Center = other.m_v3Center;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_CollidingRBSet = other.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };

//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(other);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(other);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	m_CollidingRBSet.erase(other);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pModelMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pModelMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pModelMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pModelMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pModelMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pModelMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
	m_pModelMngr->AddLineToRenderList(m_m4ToWorld * glm::translate(m_v3Center), vector3(0.0, 0.0, 0.0), m_v3Direction * 10.0f, C_BLUE, C_RED);
}
#pragma endregion