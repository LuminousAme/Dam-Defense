#include "Titan/Scene.h"
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"

using namespace Titan;

int main() {
	TTN_Application::Init("Dam Defense", 800, 800);

	//create a shader program object
	TTN_Shader::sshptr shaderProgam = TTN_Shader::Create(); 
	//load the shaders into the shader program 
	shaderProgam->LoadShaderStageFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	shaderProgam->LoadShaderStageFromFile("shaders/blinn_phong_frag_shader.glsl", GL_FRAGMENT_SHADER);
	shaderProgam->Link(); 

	//create mesh pointers and set up their vaos
	TTN_Mesh* boatMesh = TTN_ObjLoader::LoadFromFile("boat.obj"); // boat
	boatMesh->SetUpVao();
	TTN_Mesh* treeMesh = TTN_ObjLoader::LoadFromFile("tree.obj"); // tree
	treeMesh->SetUpVao();

	//create a new scene
	TTN_Scene testScene = TTN_Scene();

	//entity for the camera in testScene
	entt::entity camera;
	{
		//create an entity in the scene for the camera
		camera = testScene.CreateEntity();
		testScene.SetCamEntity(camera);
		testScene.Attach<TTN_Transform>(camera);
		testScene.Attach<TTN_Camera>(camera);
		auto& camTrans = testScene.Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		testScene.Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.0f, 0.01f, 100.f);
		testScene.Get<TTN_Camera>(camera).View();
	}

	//entity for the first tree in testScene
	entt::entity tree1;
	{
		//create an entity in the scene for the first tree
		tree1 = testScene.CreateEntity();
		
		//setup a mesh renderer for the first tree
		TTN_Renderer treeRenderer = TTN_Renderer(treeMesh, shaderProgam);
		//attach that renderer to the tree entity
		testScene.AttachCopy<TTN_Renderer>(tree1, treeRenderer);

		//setup a transform for the first tree
		TTN_Transform treeTrans = TTN_Transform();
		treeTrans.SetPos(glm::vec3(-2.0f, -3.0f, 5.f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
		//attach that transform to the tree entity
		testScene.AttachCopy<TTN_Transform>(tree1, treeTrans);
	}

	//entity for the second tree in testScene
	entt::entity tree2;
	{
		//create an entity in the scene for the second tree
		tree2 = testScene.CreateEntity();

		//attach a mesh renderer to the second tree 
		testScene.Attach<TTN_Renderer>(tree2);
		//grab a referencce to that mesh renderer and set it up
		auto& treeRenderer = testScene.Get<TTN_Renderer>(tree2);
		treeRenderer = TTN_Renderer(treeMesh, shaderProgam);

		//attach a transform to the second tree
		testScene.Attach<TTN_Transform>(tree2);
		//grab a reference to that transform and set it up
		auto& treeTrans = testScene.Get<TTN_Transform>(tree2);
		treeTrans.SetPos(glm::vec3(2.0f, -3.0f, 3.0f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
	}

	//entity for the boat in testScene
	entt::entity boat;
	{
		//create an entity in the scene for the boat
		boat = testScene.CreateEntity();

		//attach a mesh renderer to the boat
		testScene.Attach<TTN_Renderer>(boat);
		//grab a referencce to that mesh renderer and set it up
		auto& boatRenderer = testScene.Get<TTN_Renderer>(boat);
		boatRenderer = TTN_Renderer(boatMesh, shaderProgam);

		//attach a transform to the boat
		testScene.Attach<TTN_Transform>(boat);
		//grab a reference to that transform and set it up
		auto& boatTrans = testScene.Get<TTN_Transform>(boat);
		boatTrans.SetPos(glm::vec3(0.f, -3.0f, 5.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 270.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
	}

	//add the scene to the application
	TTN_Application::scenes.push_back(testScene);
	//set the background to a blue
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 8.0f, 1.0f));
	
	float speed = -1.0f;


	while (!TTN_Application::GetIsClosing()) {
		//get the change in time for the frame
		float dt = TTN_Application::GetDeltaTime();

		//move the boat 
		auto& boatTrans = testScene.Get<TTN_Transform>(boat);
		boatTrans.SetPos(glm::vec3(boatTrans.GetPos().x, boatTrans.GetPos().y, boatTrans.GetPos().z + speed * dt));
		//flip the speed if it gets to a certain point
		if (boatTrans.GetPos().z < 3.0f || boatTrans.GetPos().z > 7.0f)
			speed *= -1;

		auto& tree2Trans = testScene.Get<TTN_Transform>(tree2);
		tree2Trans.RotateFixed(glm::vec3(0, 5.0f * dt, 0));

		auto& camTrans = testScene.Get<TTN_Transform>(testScene.GetCamEntity());
		camTrans.RotateFixed(glm::vec3(0, 5.0f * dt, 0));
		
		//note:: always call keydown first
		if (TTN_Application::TTN_Input::GetKeyDown(TTN_Application::TTN_KeyCode::Space)) {
			printf("Space down\n");
		}

		if (TTN_Application::TTN_Input::GetKey(TTN_Application::TTN_KeyCode::Space)) {
			printf("Space held\n");
		}

		if (TTN_Application::TTN_Input::GetKeyUp(TTN_Application::TTN_KeyCode::Space)) {
			printf("Space released\n");
		}

		//printf("fps: %f\n", 1.0f/dt);
		//render the screen
		TTN_Application::Update();
	}
		

	return 0;
}