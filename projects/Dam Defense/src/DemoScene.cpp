//Dam Defense by Atlas X Games 
//DemoScene.cpp - source file for the class that inherits from the scene class to make a useable scene
#include "DemoScene.h"


DemoScene::DemoScene() 
	: TTN_Scene()
{
}

void DemoScene::InitScene()
{
	//create a shader program object
	shaderProgam = TTN_Shader::Create();
	//load the shaders into the shader program 
	shaderProgam->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgam->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_NO_TEXTURE);
	shaderProgam->Link();

	//create a shader program object for textured objects
	shaderProgamTextured = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgamTextured->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgamTextured->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgamTextured->Link();

	//create mesh pointers and set up their vaos
	boatMesh = TTN_ObjLoader::LoadFromFile("boat.obj"); // boat
	boatMesh->SetUpVao();
	treeMesh = TTN_ObjLoader::LoadFromFile("tree.obj"); // tree
	treeMesh->SetUpVao();
	swordMesh = TTN_ObjLoader::LoadFromFile("Sword.obj"); //sword, texture test
	swordMesh->SetUpVao();

	//load the texture for the sword from a file
	swordText = TTN_Texture2D::LoadFromFile("Sword_Texture.png");

	//material for the sword 
	swordMat = TTN_Material::Create();
	//add the texture to material and set the shininess
	swordMat->SetAlbedo(swordText);
	swordMat->SetShininess(128.0f);
	
	//ParticleData data;
	//data.m_max = 100;
	//data.m_position = glm::vec3(10.f, -5.0f, 20.0f);
	//data.m_velocity = glm::vec3(1.0f, 5.0f, 0.0f);
	//data.ColorBegin = glm::vec4(1.0f, 1.0f, 1.5f, 1.0f);
	//data.ColorEnd = glm::vec4(0.0f, 0.0f, 1.0f, 0.5f);

	//data.SizeEnd = 10.0f;
	//data.SizeBegin = 20.0f;
	//data.LifeTime = 15.0f;
	//Particles = data;

	//plane stuff
	plane = TTN_ObjLoader::LoadFromFile("Review3/water.obj");
	plane->SetUpVao();
	waterText = TTN_Texture2D::LoadFromFile("Review3/water_text.png");
	waterMat = TTN_Material::Create();
	waterMat->SetAlbedo(waterText);
	waterMat->SetShininess(128.0f);

	//entity for the camera in testScene
	{
		//create an entity in the scene for the camera
		camera = CreateEntity();
		SetCamEntity(camera);
		Attach<TTN_Transform>(camera);
		Attach<TTN_Camera>(camera);
		auto& camTrans = Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
 
		//Get<TTN_Camera>(camera).CalcOrtho(-800.0f / 100.0f, 800.0f / 100.0f, -800.0f / 100.0f, 800.0f / 100.0f, 0.001f, 100.0f);
		Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.78f, 0.01f, 100.f);
		Get<TTN_Camera>(camera).View();
	}

	//entity for the light in testScene
	{
		//create an entity in the scene for a light
		light = CreateEntity();
		m_Lights.push_back(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 3.0f, 0.0f));
		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.3f, 1.0f, 0.3f, 0.3f, 0.3f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(light, lightLight);
	}

	//entity for the sword in testScene
	{
		sword = CreateEntity();

		//setup a mesh renderer for the sword
		TTN_Renderer swordRenderer = TTN_Renderer(swordMesh, shaderProgamTextured);
		//attach the material to the renderer
		swordRenderer.SetMat(swordMat);
		//attach that renderer to the tree entity
		AttachCopy<TTN_Renderer>(sword, swordRenderer);

		//setup a transform for the first tree
		TTN_Transform swordTrans = TTN_Transform();
		swordTrans.SetPos(glm::vec3(0.0f, 0.0f, 2.0f));
		swordTrans.SetScale(glm::vec3(15.f, 15.f, 15.f));
		swordTrans.RotateFixed(glm::vec3(0.0f, 0.f, 90.0f));
		//attach that transform to the tree entity
		AttachCopy<TTN_Transform>(sword, swordTrans);
	}

	//entity for the first tree in testScene
	{
		//create an entity in the scene for the first tree
		tree1 = CreateEntity();

		//setup a mesh renderer for the first tree
		TTN_Renderer treeRenderer = TTN_Renderer(treeMesh, shaderProgam);
		//attach that renderer to the tree entity
		AttachCopy<TTN_Renderer>(tree1, treeRenderer);

		//setup a transform for the first tree
		TTN_Transform treeTrans = TTN_Transform();
		treeTrans.SetPos(glm::vec3(-3.0f, -3.0f, 5.f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
		//attach that transform to the tree entity
		AttachCopy<TTN_Transform>(tree1, treeTrans);

		//TTN_Physics pbody = TTN_Physics(glm::vec3(treeTrans.GetPos().x , treeTrans.GetPos().y , treeTrans.GetPos().z ));

		TTN_Physics pbody = TTN_Physics(treeTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), tree1, TTN_PhysicsBodyType::STATIC, 0.0f);

		AttachCopy<TTN_Physics>(tree1, pbody);

		//TTN_Particle treePart = TTN_Particle();
		//AttachCopy<TTN_Particle>(tree1, treePart);

		//add a tag to the tree
		TTN_Tag treeTag = TTN_Tag("Tree");
		AttachCopy<TTN_Tag>(tree1, treeTag);
	}

	//entity for the second tree in testScene
	{
		//create an entity in the scene for the second tree
		tree2 = CreateEntity();

		//attach a mesh renderer to the second tree 
		Attach<TTN_Renderer>(tree2);
		//grab a referencce to that mesh renderer and set it up
		auto& treeRenderer = Get<TTN_Renderer>(tree2);
		treeRenderer = TTN_Renderer(treeMesh, shaderProgam);

		//attach a transform to the second tree
		Attach<TTN_Transform>(tree2);
		//grab a reference to that transform and set it up
		auto& treeTrans = Get<TTN_Transform>(tree2);
		treeTrans.SetPos(glm::vec3(2.0f, -3.0f, 3.0f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));


		/*TTN_Physics pbody = TTN_Physics(treeTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), tree2);
		AttachCopy<TTN_Physics>(tree2, pbody);*/
	}

	//entity for the boat in testScene
	{
		//create an entity in the scene for the boat
		boat = CreateEntity();

		//attach a mesh renderer to the boat
		Attach<TTN_Renderer>(boat);
		//grab a referencce to that mesh renderer and set it up
		auto& boatRenderer = Get<TTN_Renderer>(boat);
		boatRenderer = TTN_Renderer(boatMesh, shaderProgam);

		//attach a transform to the boat
		Attach<TTN_Transform>(boat);
		//grab a reference to that transform and set it up
		auto& boatTrans = Get<TTN_Transform>(boat);
		boatTrans.SetPos(glm::vec3(0.0f, 0.0f, 5.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 270.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
 
		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), boat);
//		TTN_PhysicsB pbody = TTN_PhysicsB(boatTrans.GetPos(), m_world, m_PhysicsObjects);
		AttachCopy<TTN_Physics>(boat, pbody);

		//add a tag to the boat
		TTN_Tag boatTag = TTN_Tag("Boat");
		AttachCopy<TTN_Tag>(boat, boatTag);
 
	}

	{
		water = CreateEntity();

		//setup a mesh renderer for the water
		TTN_Renderer waterRenderer = TTN_Renderer(plane, shaderProgamTextured);
		waterRenderer.SetMat(waterMat);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(water, waterRenderer);

		//setup a transform for the water
		TTN_Transform waterTrans = TTN_Transform(glm::vec3(0.f, -7.f, 0.0f), glm::vec3(0.0f), glm::vec3(200.0f, 1.0f, 50.0f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(water, waterTrans);
	}

	//sets the boat at the sword's parent
	Get<TTN_Transform>(sword).SetParent(&Get<TTN_Transform>(boat), &boat);
 
	speed = 1.0f;
	velo = glm::vec3(0.0f);

}

void DemoScene::Update(float deltaTime)
{
 
	//transform of the sword (for debugging the scenegraph breaking
	auto& swordTrans = Get<TTN_Transform>(sword);
 
	//rotate the second tree
	auto& tree2Trans = Get<TTN_Transform>(tree2);
	tree2Trans.RotateFixed(glm::vec3(0, 5.0f * deltaTime, 0));

	//get a pointer to the collosion object between the tree and boat
	//TTN_Collision::scolptr col = FindCollisionPointer(&ptree, &pboat);
	//if (col == nullptr)
	//	printf("failed\n");
	//else if (col->GetHasCollided()) {
	//	std::cout << "Touching " << (pboat.GetMin().x <= ptree.GetMax().x && pboat.GetMax().x >= ptree.GetMin().x) <<
	//		(pboat.GetMin().y <= ptree.GetMax().y && pboat.GetMax().y >= ptree.GetMin().y) <<
	//		(pboat.GetMin().z <= ptree.GetMax().z && pboat.GetMax().z >= ptree.GetMin().z) << std::endl;
	//	//speed = 0;
	//}
	//else
	//{
	//	std::cout << "Not Touching " << (pboat.GetMin().x <= ptree.GetMax().x && pboat.GetMax().x >= ptree.GetMin().x) <<
	//		(pboat.GetMin().y <= ptree.GetMax().y && pboat.GetMax().y >= ptree.GetMin().y) <<
	//		(pboat.GetMin().z <= ptree.GetMax().z && pboat.GetMax().z >= ptree.GetMin().z) << std::endl;
	//}

	 
	//get the collisions from the base scene
	std::vector<TTN_Collision::scolptr> collisionsThisFrame = TTN_Scene::GetCollisions();

	//iterate through the collisions
	for (int i = 0; i < collisionsThisFrame.size(); i++) {
		entt::entity entity1Ptr = collisionsThisFrame[i]->GetBody1();
		entt::entity entity2Ptr = collisionsThisFrame[i]->GetBody2();

		//check if entity 1 is a tree
		if (TTN_Scene::Has<TTN_Tag>(entity1Ptr) && TTN_Scene::Get<TTN_Tag>(entity1Ptr).getName() == "Tree") {
			//if it, then delete it
			DeleteEntity(entity1Ptr);
		}
		//check if entity 2 is a tree
		if (TTN_Scene::Has<TTN_Tag>(entity2Ptr) && TTN_Scene::Get<TTN_Tag>(entity2Ptr).getName() == "Tree") {
			//if it is, delete it
			DeleteEntity(entity2Ptr);
		}
	}

	//Please don't forget to call the base scene's update at the end of the child class' update
	TTN_Scene::Update(deltaTime);
}

void DemoScene::KeyDownChecks()
{
}

void DemoScene::KeyChecks()
{
	//physics bodies for the boat and tree
 
	auto& camTrans = Get<TTN_Transform>(camera);
	auto& pboat = Get<TTN_Physics>(boat);
 

	velo = glm::vec3(0.0f);

	//control the boat through keyboard keys
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
		velo += glm::vec3(0.0f, 1.0f, 0.0f);
		if (velo.x != 0 || velo.y != 0 || velo.z != 0) {
			velo = glm::normalize(velo);
		}
		//camTrans.SetPos(glm::vec3(camTrans.GetPos().x+velo, camTrans.GetPos().y, camTrans.GetPos().z));
		//pboat.SetVelocity(velo * speed)
		//pboat.SetLinearVelocity(velo);
 
		pboat.AddImpulse(velo);
 
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
		velo += glm::vec3(0.0f, -1.0f, 0.0f);
		if (velo.x != 0 || velo.y != 0 || velo.z != 0) {
			velo = glm::normalize(velo);
		}
		//pboat.SetVelocity(velo * speed);
		//.SetLinearVelocity(velo);
		
		pboat.AddImpulse(velo);
 
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
		velo += glm::vec3(-1.0f, 0.0f, 0.0f);
		if (velo.x != 0 || velo.y != 0 || velo.z != 0) {
			velo = glm::normalize(velo);
		}
		//pboat.SetVelocity(velo * speed);
		//pboat.SetLinearVelocity(velo);
 
		pboat.AddImpulse(velo);
 
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A)) {
		velo += glm::vec3(1.0f, 0.0f, 0.0f);
		if (velo.x != 0 || velo.y != 0 || velo.z != 0) {
			velo = glm::normalize(velo);
		}
 		//pboat.SetVelocity(velo * speed);
		//pboat.SetLinearVelocity(velo);
		 
		pboat.AddImpulse(velo);

	}
}

void DemoScene::KeyUpChecks()
{
}

void DemoScene::MouseButtonDownChecks()
{
}

void DemoScene::MouseButtonChecks()
{
}

void DemoScene::MouseButtonUpChecks()
{
}