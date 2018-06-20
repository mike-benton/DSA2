#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh


	for (int i = 0; i < 46; i++) {
		MyMesh* mesh = new MyMesh();
		mesh->GenerateCube(1.0f, C_BLACK);
		meshVector.push_back(mesh);

	}

	posVector.push_back(vector3(-2, 5, 0));
	posVector.push_back(vector3(-1, 4, 0));
	posVector.push_back(vector3(3, 4, 0));
	posVector.push_back(vector3(4, 5, 0));
	posVector.push_back(vector3(-2, 3, 0));
	posVector.push_back(vector3(-1, 3, 0));
	posVector.push_back(vector3(0, 3, 0));
	posVector.push_back(vector3(1, 3, 0));
	posVector.push_back(vector3(2, 3, 0));
	posVector.push_back(vector3(3, 3, 0));
	posVector.push_back(vector3(4, 3, 0));
	posVector.push_back(vector3(-3, 2, 0));
	posVector.push_back(vector3(-2, 2, 0));
	posVector.push_back(vector3(0, 2, 0));
	posVector.push_back(vector3(1, 2, 0));
	posVector.push_back(vector3(2, 2, 0));
	posVector.push_back(vector3(4, 2, 0));
	posVector.push_back(vector3(5, 2, 0));
	posVector.push_back(vector3(-4, 1, 0));
	posVector.push_back(vector3(-3, 1, 0));
	posVector.push_back(vector3(-2, 1, 0));
	posVector.push_back(vector3(-1, 1, 0));
	posVector.push_back(vector3(0, 1, 0));
	posVector.push_back(vector3(1, 1, 0));
	posVector.push_back(vector3(2, 1, 0));
	posVector.push_back(vector3(3, 1, 0));
	posVector.push_back(vector3(4, 1, 0));
	posVector.push_back(vector3(5, 1, 0));
	posVector.push_back(vector3(6, 1, 0));
	posVector.push_back(vector3(-4, 0, 0));
	posVector.push_back(vector3(-2, 0, 0));
	posVector.push_back(vector3(-1, 0, 0));
	posVector.push_back(vector3(0, 0, 0));
	posVector.push_back(vector3(1, 0, 0));
	posVector.push_back(vector3(2, 0, 0));
	posVector.push_back(vector3(3, 0, 0));
	posVector.push_back(vector3(4, 0, 0));
	posVector.push_back(vector3(6, 0, 0));
	posVector.push_back(vector3(-4, -1, 0));
	posVector.push_back(vector3(6, -1, 0));
	posVector.push_back(vector3(-2, -1, 0));
	posVector.push_back(vector3(4, -1, 0));
	posVector.push_back(vector3(-1, -2, 0));
	posVector.push_back(vector3(0, -2, 0));
	posVector.push_back(vector3(2, -2, 0));
	posVector.push_back(vector3(3, -2, 0));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen(); 
	
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = 0.0f;
	matrix4 m4Translate;	
	value += 0.01f;

	for (int i = 0; i < 46; i++) {
		m4Translate = glm::translate(IDENTITY_M4, vector3(posVector.at(i).x + value, posVector.at(i).y, posVector.at(i).z));
		matrix4 m4Model = m4Scale * m4Translate;
		meshVector.at(i)->Render(m4Projection, m4View, m4Model);

	}

	//matrix4 m4Model = m4Translate * m4Scale;

	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < 46; i++) {
		SafeDelete(meshVector.at(i));
	}

	//release GUI
	ShutdownGUI();
}