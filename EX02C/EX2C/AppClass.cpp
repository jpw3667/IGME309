#include "AppClass.h"

bool Application::IsClose(vector3 a_v3A, vector3 a_v3B, float epsilon)
{
	if (fabs(a_v3A.x - a_v3B.x) > epsilon) return false;
	if (fabs(a_v3A.y - a_v3B.y) > epsilon) return false;
	if (fabs(a_v3A.z - a_v3B.z) > epsilon) return false;
	return true;
}
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	vector3 v3Position(5.0f, 5.0f, 15.0f);
	vector3 v3Target(5.0f, 5.0f, 14.0f);
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetCameraMode(BTXs::CAM_ORTHO_Z);
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);

	m_sObject1 = "ARBBTest.obj";
	m_sObject2 = "Minecraft\\Creeper.obj";

	//object 1
	m_pModelMngr->LoadModel(m_sObject1);
	m_pObject1 = new MyRigidBody(m_pModelMngr->GetVertexList(m_sObject1));

	//object 2
	m_pModelMngr->LoadModel(m_sObject2);
	m_pObject2 = new MyRigidBody(m_pModelMngr->GetVertexList(m_sObject2));

	m_v3Object = vector3(0.0f, 0.0f, 0.0f);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	//Set model matrix to the movable object
	matrix4 m4Movable = 
		glm::translate(m_v3Object) * 
		ToMatrix4(m_qArcBall) * 
		glm::rotate(glm::radians(55.0f), vector3(1.0f));
	m_pModelMngr->AddModelToRenderList(m_sObject1, m4Movable);
	m_pObject1->SetModelMatrix(m4Movable);
	m_pObject1->AddToRenderList();
	
	//Set model matrix to second object
	static float fHeight = -1.0f;
	static bool bReverse = true;

	matrix4 m4Creeper = glm::translate(vector3(4.0f, fHeight, 0.0f));
	m_pModelMngr->AddModelToRenderList(m_sObject2, m4Creeper);
	m_pObject2->SetModelMatrix(m4Creeper);
	m_pObject2->AddToRenderList();
	
	float fDelta = 0.05f;
	if (bReverse)
		fDelta *= -1.0f;
	fHeight += fDelta;
	if (fHeight > 1.0f || fHeight < -1.0f)
		bReverse = !bReverse;

	//check collision, if they are colliding render it in red
	m_bColliding = m_pObject1->IsColliding(m_pObject2);

	m_pObject1->Update();
	m_pObject2->Update();

	m_pObject1->AddToRenderList();
	m_pObject2->AddToRenderList();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();

	// add grid
	m_pModelMngr->AddGridToRenderList();

	//render list call
	m_uRenderCallCount = m_pModelMngr->Render();

	//clear the render list
	m_pModelMngr->ClearRenderList();

	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the rigid body for model 1
	SafeDelete(m_pObject1);

	//release the rigid body for model 2
	SafeDelete(m_pObject2);

	//release GUI
	ShutdownGUI();
}