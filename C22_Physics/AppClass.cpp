#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	vector3 v3Position = vector3(1, 0, 1);
	matrix4 m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
	m_pEntityMngr->UsePhysicsSolver();

		//m_pEntityMngr->SetMass(2);

		//m_pEntityMngr->SetMass(i+1);

	srand(time(NULL));

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (rand() % 2 == 1 && i + j > 1 && i + j < 11) {
				blocks[i][j] = true;

				m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Block_" + std::to_string(i) + "_" + std::to_string(j)); //randomly places cubes in steve's way
				v3Position = vector3(i * 2.01f, 0, j * 2.01f);
				m4Position = glm::translate(v3Position);
				m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
				m_pEntityMngr->UsePhysicsSolver();
			}
			else {
				blocks[i][j] = false;
			}
		}
	}

	optimalPath = ConstructPath();

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			bool isPath = false;

			for (int k = 0; k < optimalPath.size(); k++) {
				if (optimalPath[k] == vector2(i, j)) {
					m_pEntityMngr->AddEntity("Sorted\\Polycube.obj", "Floor_" + std::to_string(i) + "_" + std::to_string(j)); //polycubes are steve's path to greatness
					v3Position = vector3(i * 2.01f + 1, -1.01f, j * 2.01f + 1);

					isPath = true;
					break;
				}
			}

			if (!isPath) {
				m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Floor_" + std::to_string(i) + "_" + std::to_string(j)); //the other floor is there too i guess
				v3Position = vector3(i * 2.01f, -2.01f, j * 2.01f);
			}

			m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(2.0f)));
 		}
	}


}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");
	FollowPath(optimalPath);

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
std::vector<vector2> Application::ConstructPath(void) { //makes the path
	std::vector<vector2> path;
	int x = 0;
	int y = 0;

	while (vector2(x, y) != vector2(6, 6)) {
		path.push_back(vector2(x, y));

		if (x < 6 && !blocks[x + 1][y]) //steve tries to go through unblocked terrain first
			x++;

		else if (y < 6 && !blocks[x][y + 1])
			y++;

		else if (x < 6) { //eventually he pushes through
			x++;

			for (int i = 1; i < 7 - x; i++) { //steve is smart and tries to avoid terrain he just pushed in front of himself
				if (!blocks[x + i][y]) {
					blocks[x + i][y] = true;
					i = 7;
				}
			}
			blocks[x][y] = false;
		}
		else {
			y++;

			for (int i = 1; i < 7 - y; i++) {
				if (!blocks[x][y + i]) {
					blocks[x][y + i] = true;
					i = 7;
				}
			}
			blocks[x][y] = false;
		}
	}

	path.push_back(vector2(6, 6));

	return path;
}

void Application::FollowPath(std::vector<vector2> path) { //I tried followPath with both forces and velocities, if you want to watch steve bumble around you could look at the force code
	int steveIndex = m_pEntityMngr->GetEntityIndex("Steve");
	vector3 stevePos = m_pEntityMngr->GetEntity(steveIndex)->GetPosition();
	
	if (pathIndex < path.size()) {
		if (path[pathIndex].x * 2.01f - stevePos.x > path[pathIndex].y * 2.01f - stevePos.z) { //we need to be heading along X and not Y
			//m_pEntityMngr->ApplyForce(vector3(.01f, 0.0f, 0.0f), "Steve");
			m_pEntityMngr->GetEntity(steveIndex)->SetVelocity(vector3(.1f, 0, 0));

			if (stevePos.x - 2.01f > path[pathIndex].x * 2.01f) { //we have reached our destination
				pathIndex++;
				//m_pEntityMngr->GetEntity(steveIndex)->SetVelocity(vector3(0, 0, 0));
			}
		}
		else {
			//m_pEntityMngr->ApplyForce(vector3(.0f, 0.0f, 0.01f), "Steve");
			m_pEntityMngr->GetEntity(steveIndex)->SetVelocity(vector3(0, 0, .1f));

			if (stevePos.z > path[pathIndex].y * 2.01f) { //we have reached our destination
				pathIndex++;
				//m_pEntityMngr->GetEntity(steveIndex)->SetVelocity(vector3(0, 0, 0));

			}
		}
	}


	

}



void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}