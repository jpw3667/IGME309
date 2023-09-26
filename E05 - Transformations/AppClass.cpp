#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Jason Weinberg - jpw3667@rit.edu";
	vector3 v3Position(0.0f, 0.0f, 10.0f);
	vector3 v3Target = ZERO_V3;
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);

	//Allocate the memory for the Meshes
	m_uMeshCount = 46;
	for (int i = 0; i < m_uMeshCount; i++)
	{
		MyMesh* pMesh = new MyMesh();
		m_pMeshList.push_back(pMesh);
		m_pMeshList[i]->GenerateCube(1.0f, C_BLACK);
	}
		
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
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Calculate the model, view and projection matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	/*
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f, 2.0f, 2.0f));
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 3.0f, 3.0f));
	matrix4 m4Model = m4Translate * m4Scale;
	*/
	static float value = -3.0f;
	

	matrix4 m4List[46];

	m4List[0] = glm::translate(vector3(-3.0f + value, 4.0f, 1.0f));
	m4List[1] = glm::translate(vector3(3.0f + value, 4.0f, 1.0f));
	m4List[2] = glm::translate(vector3(-2.0f + value, 3.0f, 1.0f));
	m4List[3] = glm::translate(vector3(2.0f + value, 3.0f, 1.0f));
	for(int i = -3; i < 4; i ++)
		m4List[i + 7] = glm::translate(vector3(float(i) + value, 2.0f, 1.0f));
	m4List[11] = glm::translate(vector3(-4.0f + value, 1.0f, 1.0f));
	m4List[12] = glm::translate(vector3(-3.0f + value, 1.0f, 1.0f));
	m4List[13] = glm::translate(vector3(-1.0f + value, 1.0f, 1.0f));
	m4List[14] = glm::translate(vector3(0.0f + value, 1.0f, 1.0f));
	m4List[15] = glm::translate(vector3(1.0f + value, 1.0f, 1.0f));
	m4List[16] = glm::translate(vector3(3.0f + value, 1.0f, 1.0f));
	m4List[17] = glm::translate(vector3(4.0f + value, 1.0f, 1.0f));
	for (int i = -5; i < 6; i++)
		m4List[i + 23] = glm::translate(vector3(float(i) + value, 0.0f, 1.0f));
	m4List[29] = glm::translate(vector3(-5.0f + value, -1.0f, 1.0f));
	for (int i = -3; i < 4; i++)
		m4List[i + 33] = glm::translate(vector3(float(i) + value, -1.0f, 1.0f));
	m4List[37] = glm::translate(vector3(5.0f + value, -1.0f, 1.0f));
	m4List[38] = glm::translate(vector3(-5.0f + value, -2.0f, 1.0f));
	m4List[39] = glm::translate(vector3(-3.0f + value, -2.0f, 1.0f));
	m4List[40] = glm::translate(vector3(3.0f + value, -2.0f, 1.0f));
	m4List[41] = glm::translate(vector3(5.0f + value, -2.0f, 1.0f));
	m4List[42] = glm::translate(vector3(-2.0f + value, -3.0f, 1.0f));
	m4List[43] = glm::translate(vector3(-1.0f + value, -3.0f, 1.0f));
	m4List[44] = glm::translate(vector3(1.0f + value, -3.0f, 1.0f));
	m4List[45] = glm::translate(vector3(2.0f + value, -3.0f, 1.0f));
	
	value += 0.01f;

	
	for (int i = 0; i < m_uMeshCount; i++)
	{
		m_pMeshList[i]->Render(m4Projection, m4View, m4List[i]);
	}
	
	

	 // ToMatrix4(m_qArcBall));

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();

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
	//Release meshes
	for (int i = 0; i < m_uMeshCount; i++)
		SafeDelete(m_pMeshList[i]);

	//release GUI
	ShutdownGUI();
}