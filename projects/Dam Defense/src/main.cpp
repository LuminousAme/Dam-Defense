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
<<<<<<< HEAD
	//create a mesh for the triangle
	TTN_Mesh triMesh = TTN_Mesh();
	//create the vertex position
	std::vector<glm::vec3> triVerts;
	triVerts.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	triVerts.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	triVerts.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
	triMesh.setVertices(triVerts);
	//create the vertex normals
	std::vector<glm::vec3> triNorms;
	for (int i = 0; i < 3; i++) triNorms.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	triMesh.setNormals(triNorms);
	//create the vertex uvs
	std::vector<glm::vec3> triUVs;
	for (int i = 0; i < 3; i++) triUVs.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	triMesh.setUVs(triUVs);

	//setup a transform for the trinagle
	TTN_Transform triTrans = TTN_Transform();
	triTrans.SetPos(glm::vec3(1.0f, 0.5f, 1.0f));
	triTrans.SetRot(glm::vec3(0.0f, 0.0f, 1.0f));
	triTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

	//setup a mesh renderer for the trinagle
	TTN_Renderer triRenderer = TTN_Renderer(triMesh, shaderProgam);
=======

	//create mesh pointers and set up their vaos
	TTN_Mesh* boatMesh = TTN_ObjLoader::LoadFromFile("boat.obj"); // boat
	boatMesh->SetUpVao();
	TTN_Mesh* treeMesh = TTN_ObjLoader::LoadFromFile("tree.obj"); // tree
	treeMesh->SetUpVao();
>>>>>>> Ame

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
		camTrans.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
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
		treeTrans.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));
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
		treeTrans.SetRot(glm::vec3(0.0f, 90.0f, 0.0f));
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
		boatTrans.SetRot(glm::vec3(0.0f, 270.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
	}

	//add the scene to the application
	TTN_Application::scenes.push_back(testScene);
	//set the background to a blue
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 8.0f, 1.0f));
	
	float speed = -1.0f;
	float rotY = 0;
	float rotSpeed = 5;

	while (!TTN_Application::GetIsClosing()) {
		//get the change in time for the frame
		float dt = TTN_Application::GetDeltaTime();

		//move the boat 
		auto& boatTrans = testScene.Get<TTN_Transform>(boat);
		boatTrans.SetPos(glm::vec3(boatTrans.GetPos().x, boatTrans.GetPos().y, boatTrans.GetPos().z + speed * dt));
		//flip the speed if it gets to a certain point
		if (boatTrans.GetPos().z < 3.0f || boatTrans.GetPos().z > 7.0f)
			speed *= -1;

		//rotate the second tree 
		rotY += rotSpeed * dt;
		auto& tree2Trans = testScene.Get<TTN_Transform>(tree2);
		tree2Trans.SetRot(glm::vec3(tree2Trans.GetRotation().x, rotY, tree2Trans.GetRotation().z));
		if (rotY >= 90.f)
			rotSpeed = -5.0f;
		else if (rotY <= -90.f)
			rotSpeed = 5.0f;

<<<<<<< HEAD
	while (!TTN_Application::GetIsClosing()) {

		testScene.Get<TTN_Camera>(CamEntity).keyInput(TTN_Application::m_window);
=======
		//render the screen
>>>>>>> Ame
		TTN_Application::Update();
	}
		



	}
	return 0;
}