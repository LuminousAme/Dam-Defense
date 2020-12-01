#include "Review3Scene.h"
#include "Titan/Physics.h"
#include "glm/ext.hpp"

Review3Scene::Review3Scene()
	: TTN_Scene()
{
}

void Review3Scene::InitScene()
{
	////////// resources /////////////////
#pragma region SHADERS, MESHES AND MATERIALS
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

	//create a shader program object for the skybox
	shaderProgramSkybox = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramSkybox->LoadDefaultShader(TTN_DefaultShaders::VERT_SKYBOX);
	shaderProgramSkybox->LoadDefaultShader(TTN_DefaultShaders::FRAG_SKYBOX);
	shaderProgramSkybox->Link();

	//create a shader program object for textured objects
	shaderHeight = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderHeight->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR_HEIGHTMAP);
	shaderHeight->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderHeight->Link();

	//create mesh pointers and set up their vaos
	boatMesh = TTN_ObjLoader::LoadFromFile("boat.obj"); // boat
	boatMesh->SetUpVao();
	cannonMesh = TTN_ObjLoader::LoadAnimatedMeshFromFiles("Morphtest/cannon", 7);
	cannonMesh->SetUpVao();
	plane = TTN_ObjLoader::LoadFromFile("Review3/water.obj");
	plane->SetUpVao();
	tree1Mesh = TTN_ObjLoader::LoadFromFile("Review3/Tree 1.obj");
	tree1Mesh->SetUpVao();
	skyboxMesh = TTN_ObjLoader::LoadFromFile("SkyboxMesh.obj");
	skyboxMesh->SetUpVao();

	swordMesh = TTN_ObjLoader::LoadFromFile("Sword.obj"); //sword, texture test
	swordMesh->SetUpVao();

	//load the texture for the sword from a file
	swordText = TTN_Texture2D::LoadFromFile("Sword_Texture.png");

	//material for the sword
	swordMat = TTN_Material::Create();
	//add the texture to material and set the shininess
	swordMat->SetAlbedo(swordText);
	swordMat->SetShininess(128.0f);

	//create texture pointers and load the textures in
	waterText = TTN_Texture2D::LoadFromFile("Review3/water_text.png");
	cannonText = TTN_Texture2D::LoadFromFile("Review3/Metal_Texture_2.jpg");

	skyboxText = TTN_TextureCubeMap::LoadFromImages("cubemaps/skybox/ocean.jpg");
	heightmap = TTN_Texture2D::LoadFromFile("Review3/heightmap.bmp");

	//create material pointers and set them up
	waterMat = TTN_Material::Create();
	waterMat->SetAlbedo(waterText);
	waterMat->SetShininess(128.0f);
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);

	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);
	heightMat = TTN_Material::Create();
	heightMat->SetAlbedo(heightmap);
	heightMat->SetHeightMap(heightmap);
	heightMat->SetHeightInfluence(1.0f);
	heightMat->SetShininess(128.0f);

#pragma endregion
	testParticle = TTN_ParticleTemplate();
	testParticle.SetMat(waterMat);
	testParticle.SetMesh(tree1Mesh);
	testParticle.SetOneEndColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
	testParticle.SetOneEndSize(0.0f);
	testParticle.SetOneEndSpeed(2.0f);
	testParticle.SetOneLifetime(1.0f);
	testParticle.SetOneStartColor(glm::vec4(1.0f));
	testParticle.SetOneStartSize(0.1f);
	testParticle.SetOneStartSpeed(1.0f);

	////////// entities /////////////////
	//entity for the camera
	{
		//create an entity in the scene for the camera
		camera = CreateEntity();
		SetCamEntity(camera);
		Attach<TTN_Transform>(camera);
		Attach<TTN_Camera>(camera);
		auto& camTrans = Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 5.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Get<TTN_Camera>(camera).CalcPerspective(60.0f, 1.78f, 0.01f, 100.f);
		Get<TTN_Camera>(camera).View();
	}

	//entity for the light
	{
		//create an entity in the scene for a light
		light = CreateEntity();

		m_Lights.push_back(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 3.0f, 5.0f));
		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.6f, 2.0f, 0.3f, 0.3f, 0.3f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(light, lightLight);
	}

	//entity for the skybox
	{
		skybox = CreateEntity();

		//setup a mesh renderer for the skybox
		TTN_Renderer skyboxRenderer = TTN_Renderer(skyboxMesh, shaderProgramSkybox);
		skyboxRenderer.SetMat(skyboxMat);
		skyboxRenderer.SetRenderLayer(100);
		//attach that renderer to the entity
		AttachCopy<TTN_Renderer>(skybox, skyboxRenderer);

		//setup a transform for the skybox
		TTN_Transform skyboxTrans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy<TTN_Transform>(skybox, skyboxTrans);
	}
	
	//entity for the particle system
	{
		testParticleSystem = CreateEntity();

		//setup a transfrom for the particle system
		TTN_Transform testpsTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy(testParticleSystem, testpsTrans);

		//setup a particle system for the particle system
		TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(1000000, 7000, testParticle, 5.0f, true);
		ps->MakeSphereEmitter();
		//setup a component particle system
		TTN_ParticeSystemComponent psComponent = TTN_ParticeSystemComponent(ps);
		//attach that particle system to the entity
		AttachCopy(testParticleSystem, psComponent);
	}
	

	//entity for the cannon
	{
		cannon = CreateEntity();

		//setup a mesh renderer for the cannon
		TTN_Renderer cannonRenderer = TTN_Renderer(cannonMesh, shaderProgamTextured);
		cannonRenderer.SetMat(cannonMat);
		//attach that renderer to the entity
		AttachCopy<TTN_Renderer>(cannon, cannonRenderer);

		//setup a transform for the cannon
		TTN_Transform cannonTrans = TTN_Transform(glm::vec3(0.0f, -0.4f, -0.25f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.40f));
		cannonTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
		//attach that transform to the entity
		AttachCopy<TTN_Transform>(cannon, cannonTrans);
	}

	/*
	//entity for the water
	{
		water = CreateEntity();

		//setup a mesh renderer for the water
		TTN_Renderer waterRenderer = TTN_Renderer(plane, shaderProgamTextured);
		waterRenderer.SetMat(waterMat);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(water, waterRenderer);

		//setup a transform for the water
		TTN_Transform waterTrans = TTN_Transform(glm::vec3(0.f, -7.0f, 0.0f), glm::vec3(0.0f), glm::vec3(200.0f, 1.0f, 200.0f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(water, waterTrans);
	}*/

#pragma region TREES (PLACEHOLDER FOR ISLANDS)
	//positive x is to the left, negative is to the right
	//positive z is farther, negative is closer (lower than 0 is behind)

	//close islands
	//entity for a tree
	{
		tree1 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree1, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(18.f, -5.0f, 35.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree1, treeTrans);
	}
	{
		tree2 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree2, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(-18.f, -5.0f, 35.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree2, treeTrans);
	}
	//normal scoring zone islands
	{
		tree3 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree3, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(15.f, -5.0f, 55.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree3, treeTrans);
	}
	{
		tree4 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree4, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(-15.f, -5.0f, 55.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree4, treeTrans);
	}
	//center far island
	{
		tree5 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree5, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(0.f, -5.0f, 75.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree5, treeTrans);
	}

#pragma endregion

	//debugging entity for positions
	{
		debug = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(swordMesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(debug, treeRenderer);

		//setup a transform for the
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(-28.0f, -5.0f, 40.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(debug, treeTrans);
	}

	//left boat entity
	//{
	//	//create an entity in the scene for the boat
	//	boat = CreateEntity();

	//	//attach a mesh renderer to the boat
	//	Attach<TTN_Renderer>(boat);
	//	//grab a referencce to that mesh renderer and set it up
	//	auto& boatRenderer = Get<TTN_Renderer>(boat);
	//	boatRenderer = TTN_Renderer(boatMesh, shaderProgam);

	//	//attach a transform to the boat
	//	Attach<TTN_Transform>(boat);
	//	//grab a reference to that transform and set it up
	//	auto& boatTrans = Get<TTN_Transform>(boat);
	//	boatTrans.SetPos(glm::vec3(70.0f, -5.0f, 70.0f));
	//	boatTrans.RotateFixed(glm::vec3(0.0f, 180.0f, 0.0f));
	//	boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

	//	TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), boat);
	//	pbody.SetLinearVelocity(glm::vec3(-8.0f, 0.f, 0.0f));//-2.0f
	//	//pbody.SetLinearVelocity(Seek(glm::vec3(30.f, -5.f, 50.f), pbody.GetLinearVelocity(), boatTrans.GetPos(), 0));
	//	AttachCopy<TTN_Physics>(boat, pbody);

	//	//add a tag to the boat
	//	TTN_Tag boatTag = TTN_Tag("Boat");
	//	AttachCopy<TTN_Tag>(boat, boatTag);
	//}

	//right boat
	{
		//create an entity in the scene for the boat
		boat2 = CreateEntity();

		//attach a mesh renderer to the boat
		Attach<TTN_Renderer>(boat2);
		//grab a reference to that mesh renderer and set it up
		auto& boatRenderer = Get<TTN_Renderer>(boat2);
		boatRenderer = TTN_Renderer(boatMesh, shaderProgam);

		//attach a transform to the boat
		Attach<TTN_Transform>(boat2);
		//grab a reference to that transform and set it up
		auto& boatTrans = Get<TTN_Transform>(boat2);
		boatTrans.SetPos(glm::vec3(-70.0f, -5.0f, 70.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), boat2);
		pbody.SetLinearVelocity(glm::vec3(8.0f, 0.f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boat2, pbody);

		//add a tag to the boat
		TTN_Tag boatTag = TTN_Tag("Boat");
		AttachCopy<TTN_Tag>(boat2, boatTag);
	}

	//set the camera to be a child of the cannon
	Get<TTN_Transform>(cannon).SetParent(&Get<TTN_Transform>(camera), &camera);

	/////// other /////////
	rotAmmount = glm::vec2(0.0f, 0.0f);
	mousePos = TTN_Application::TTN_Input::GetMousePosition();
}

void Review3Scene::Update(float deltaTime)
{
	//get the mouse position
	glm::vec2 tempMousePos = TTN_Application::TTN_Input::GetMousePosition();

	auto& transCannon = Get<TTN_Transform>(cannon);
	auto& transCamera = Get<TTN_Transform>(camera);

	//transCamera.RotateFixed(glm::vec3(0.0f, 0.0f, 5.0f * deltaTime));
	//std::cout << glm::to_string (Get<TTN_Transform>(camera).GetRotQuat()) << std::endl;

	glm::vec3 tempRot = transCamera.GetRotation();

	////check if the mouse has moved on the x-axis
	if (tempMousePos.x != mousePos.x) {
		//if it is, get the difference
		float diffx = tempMousePos.x - mousePos.x;
		//if it's moving in the positive direction
		if (diffx < 0) {
			//rotate by 5 degrees a second in that direction
			rotAmmount.x += diffx * deltaTime;
			//glm::vec3 rot = glm::rotate(Get<TTN_Transform>(camera).GetRotation(), glm::radians(-1.9f * rotAmmount.x / 18), glm::vec3(0.0f, 1.0f, 0.0f));
			//Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, rot.x, 0.0f));

			Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, glm::normalize(1.5f * rotAmmount.x / 20), 0.0f));
		}
		//if it's moving in the negative direction
		else if (diffx > 0) {
			//rotate by 5 degrees a second in that direction
			rotAmmount.x += diffx * deltaTime;

			//glm::vec3 rot = glm::rotate(Get<TTN_Transform>(camera).GetRotation(), -1.9f * rotAmmount.x / 18, glm::vec3(0.0f, 1.0f, 0.0f));
			//Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, -rot.x, 0.0f)); //+

			Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, glm::normalize(-1.5f * rotAmmount.x / 20), 0.0f)); //+
		}
		else {
			rotAmmount.x = 0.f;
		}
		tempRot = transCamera.GetRotation();
		//Get<TTN_Transform>(camera).SetRotationQuat(glm::quat(glm::radians(glm::vec3(tempRot.x, tempRot.y, -0.0f))));
	}

	//check if mouse moved on y axis
	//if (tempMousePos.y != mousePos.y) {
	//	float diffy = tempMousePos.y - mousePos.y;
	//	if (diffy < 0) {
	//		rotAmmount.y += diffy * deltaTime;

	//		//glm::vec3 rot = glm::rotate(Get<TTN_Transform>(camera).GetRotation(), -1.9f * rotAmmount.y / 18, glm::vec3(1.0f, 0.0f, 0.0f));
	//		//Get<TTN_Transform>(camera).RotateFixed(glm::vec3(-rot.y, 0.0f, 0.0f));  //-

	//		Get<TTN_Transform>(camera).RotateFixed(glm::vec3(-1.8f * rotAmmount.y / 18, 0.0f, 0.0f));  //-
	//	}
	//	else if (diffy > 0) {
	//		rotAmmount.y += diffy * deltaTime;

	//		//glm::vec3 rot = glm::rotate(Get<TTN_Transform>(camera).GetRotation(), -1.9f * rotAmmount.y / 18, glm::vec3(1.0f, 0.0f, 0.0f));
	//		//Get<TTN_Transform>(camera).RotateFixed(glm::vec3(rot.y, 0.0f, 0.0f));  //-

	//		Get<TTN_Transform>(camera).RotateFixed(glm::vec3(1.8f * rotAmmount.y / 18, 0.0f, 0.0f)); //positive x is downwards camera
	//	}
	//	else {
	//		rotAmmount.y = 0;
	//	}
	//	tempRot = transCamera.GetRotation();
	//	//Get<TTN_Transform>(camera).RotateRelative
	//}

	mousePos = tempMousePos;

	
	Spawner(deltaTime, 5.0f);//sets the spawner and gives the interval of time the spawner should spawn boats

	//goes through the boats vector 
	for (int i = 0; i < boats.size(); i++) {
		std::cout << "Path: " << Get<TTN_Tag>(boats[i]).getPath() << std::endl; 
		int p = Get<TTN_Tag>(boats[i]).getPath(); //gets the boats randomized path num
		BoatPathing(boats[i], p); //updates the pathing for the boat 
	}

	BoatPathing(boat2, 6);//right side boat

	glm::vec3 movement = glm::vec3(0.0f);
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
		movement.x += -15.0f * deltaTime;
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
		movement.x += 15.0f * deltaTime;
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A)) {
		movement.y += 15.0f * deltaTime;
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
		movement.y += -15.0f * deltaTime;
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftControl)) {
		movement.z += -8.0f * deltaTime;
	}
	/*if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Space)) {
		movement.z += 8.0f * deltaTime;
	}*/
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftShift)) {
		movement *= 6.0f;
	}

	printf("fps: %f\n", 1.0f/deltaTime);

	TTN_Scene::Update(deltaTime);
}

#pragma region INPUTS
void Review3Scene::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Q)) {
		CreateCannonball();
	}
}

void Review3Scene::KeyChecks()
{
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::T)) {
		Get<TTN_Transform>(camera).SetPos(Get<TTN_Transform>(camera).GetPos());
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::G)) {
		Get<TTN_Transform>(camera).SetPos(Get<TTN_Transform>(camera).GetPos());
	}
}

void Review3Scene::KeyUpChecks()
{
}

void Review3Scene::MouseButtonDownChecks()
{
}

void Review3Scene::MouseButtonChecks()
{
}

void Review3Scene::MouseButtonUpChecks()
{
}
#pragma endregion

void Review3Scene::CreateCannonball() {
	ball = CreateEntity();
	cannonballs.push_back(ball);

	TTN_Renderer ballRenderer = TTN_Renderer(boatMesh, shaderProgam);
	AttachCopy<TTN_Renderer>(ball, ballRenderer);

	auto& cannonTrans = Get<TTN_Transform>(cannon);
	auto& transCamera = Get<TTN_Transform>(camera);

	//Attach<TTN_Transform>(ball);
	//auto& ballTrans = Get<TTN_Transform>(ball);

	TTN_Transform ballTrans = TTN_Transform(glm::vec3(10.f, -5.0f, 20.0f), glm::vec3(0.0f), glm::vec3(0.75f));

	glm::vec3 forward = glm::mat3(glm::inverse(transCamera.GetMatrix())) * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 origin = forward + transCamera.GetPos();
	//origin = glm::normalize(origin);

	ballTrans.SetPos(glm::vec3((origin.x + 1.0f, origin.y + 5.0f, origin.z)));

	//ballTrans.SetPos(glm::vec3(10.f, -5.0f, 20.0f));
	ballTrans.RotateFixed(glm::vec3(0.0f, 270.0f, 0.0f));
	ballTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

	AttachCopy<TTN_Transform>(ball, ballTrans);
	float power = 8.f;
	glm::vec3 vel = forward * power;

	TTN_Physics pbody = TTN_Physics(ballTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), ball, TTN_PhysicsBodyType::DYNAMIC, 1.0f);
	pbody.SetHasGravity(true); //doesn't do anything for now
	pbody.SetLinearVelocity(glm::vec3(vel.x, vel.y, vel.z));
	//pbody.SetLinearVelocity(glm::vec3(0.f, -2.0f, 12.0f));
	//pbody.SetLinearVelocity(glm::vec3(origin.x * 6.0f, -1.0f, origin.z * 6.0f));

	//std::cout << glm::to_string(ball) << std::endl;
	std::cout << "Ball: " << cannonballs.size() << std::endl;

	AttachCopy<TTN_Physics>(ball, pbody);
}

//sets the pathing the boat entity should take based on the path integer (1-3 is left side, 4-6 is right side)
void Review3Scene::BoatPathing(entt::entity boatt, int path)
{
	auto& pBoat = Get<TTN_Physics>(boatt);
	auto& tBoat = Get<TTN_Transform>(boatt);

	//left side middle path
	if (path == 1) {
		//glm::all(glm::lessThanEqual(tBoat.GetPos(), glm::vec3(30.f, -5.0f, 70.f)))
		/*if (tBoat.GetPos().x <= 30.f) {
			pBoat.SetLinearVelocity(glm::vec3(-4.0f, 0.f, -3.0f));
			if (tBoat.GetRotation().y <= 45.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.5f, 0.0f));
			}
		}*/

		if (tBoat.GetPos().x <= 30.f) {
			if (tBoat.GetRotation().y <= 55.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.55f, 0.0f));
			}

			pBoat.SetLinearVelocity(Seek(glm::vec3(0.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
		//	//pBoat.SetLinearVelocity(Seek(glm::vec3(30.f, -5.f, 50.f), pBoat.GetLinearVelocity(), tBoat.GetPos(), deltaTime));
		//	std::cout << glm::to_string(pBoat.GetLinearVelocity()) << std::endl;}
	}

	//far left path
	if (path == 2) {
		if (tBoat.GetPos().x <= 35.f) {
			if (tBoat.GetRotation().y <= 80.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.75f, 0.0f));
			}
			pBoat.SetLinearVelocity(Seek(glm::vec3(20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//center left
	if (path == 3) {
		if (tBoat.GetPos().x <= 20.f) {
			if (tBoat.GetRotation().y <= 60.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.75f, 0.0f));
			}
			pBoat.SetLinearVelocity(Seek(glm::vec3(2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));

			if (tBoat.GetPos().x <= 2.f) {
				if (tBoat.GetRotation().y <= 89.0f) {
					tBoat.RotateFixed(glm::vec3(0.0f, -0.95f, 0.0f));
				}
				pBoat.SetLinearVelocity(Seek(glm::vec3(2.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			}
		}
	}

	//right middle path
	if (path == 4) {
		if (tBoat.GetPos().x >= -30.f) {
			if (tBoat.GetRotation().y <= 55.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.75f, 0.0f));
			}

			pBoat.SetLinearVelocity(Seek(glm::vec3(0.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//far right path
	if (path == 5) {
		if (tBoat.GetPos().x >= -35.f) {
			if (tBoat.GetRotation().y <= 85.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.95f, 0.0f));
			}

			pBoat.SetLinearVelocity(Seek(glm::vec3(-20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//right center path
	if (path == 6) {
		if (tBoat.GetPos().x >= -20.f) {
			if (tBoat.GetRotation().y <= 60.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.75f, 0.0f));
			}
			pBoat.SetLinearVelocity(Seek(glm::vec3(-2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));

			if (tBoat.GetPos().x >= -2.f) {
				if (tBoat.GetRotation().y <= 89.0f) {
					tBoat.RotateFixed(glm::vec3(0.0f, 0.95f, 0.0f));
				}
				pBoat.SetLinearVelocity(Seek(glm::vec3(-2.0f, -5.0f, 4.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			}
		}
	}
}

//note this spawner code only spawns code on the left side of the map (because of the boat pos)
//to spawn on the right side can make a new function with a few changed values
void Review3Scene::Spawner(float deltatime, float SpawnTime) {
	//increment timer
	Timer += deltatime;
	//if the timer is >= spawn time then it will spawn the boat
	if (Timer >= SpawnTime) {
		// timer = 0, boat spawn code
		Timer = 0.F;//reset timer

		boat = CreateEntity();
		boats.push_back(boat);

		TTN_Renderer boatRenderer = TTN_Renderer(boatMesh, shaderProgam);
		AttachCopy<TTN_Renderer>(boat, boatRenderer);

		TTN_Transform boatTrans = TTN_Transform();
		boatTrans.SetPos(glm::vec3(70.0f, -5.0f, 70.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 180.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		AttachCopy<TTN_Transform>(boat, boatTrans);

		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), boat, TTN_PhysicsBodyType::DYNAMIC, 1.0f);
		pbody.SetHasGravity(false);
		pbody.SetLinearVelocity(glm::vec3(-11.0f, 0.f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boat, pbody);

		//add a tag to the boat
		//TTN_Tag boatTag = TTN_Tag(" Boat "+ boats.size() );

		int r = rand() % 3 + 1; // generates path number between 1-3 (left side paths, right side path nums are 4-6)
		std::cout << "Num: " << r << std::endl; //random boat path
		TTN_Tag boatTag = TTN_Tag(r); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(boat, boatTag);
	}


}

glm::vec3 Review3Scene::Seek(glm::vec3 target, glm::vec3 velo, glm::vec3 pos)
{
	//std::cout << glm::to_string(target) << std::endl;
	//std::cout << glm::to_string(velo) << std::endl;
	//std::cout << glm::to_string(pos) << std::endl;
	glm::vec3 maxVelo(-10.0f, 0.f, -10.0f);
	glm::vec3 desired = pos - target; //gets the desired vector

	desired = glm::normalize(desired) * maxVelo;
	//glm::vec3 steering = desired - velo;
	//steering = glm::normalize(steering);
	glm::vec3 moveVelo = desired;  //clamp(moveVelo,velo+steering, maxVelo);

	//std::cout << glm::to_string(moveVelo) << std::endl;

	return moveVelo;
}