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
	sphereMesh = TTN_ObjLoader::LoadFromFile("IcoSphere.obj");
	sphereMesh->SetUpVao();
	flamethrowerMesh = TTN_ObjLoader::LoadFromFile("Flamethrower/Flamethrower.obj");
	flamethrowerMesh->SetUpVao();
	swordMesh = TTN_ObjLoader::LoadFromFile("Sword.obj"); //sword, texture test
	swordMesh->SetUpVao();

	birdMesh = TTN_ObjLoader::LoadFromFile("Bird/Birb.obj"); //sword, texture test
	birdMesh->SetUpVao();

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
	birdText = TTN_Texture2D::LoadFromFile("Bird/BirdTexture.png");
	flamethrowerText = TTN_Texture2D::LoadFromFile("Flamethrower/FlamethrowerTexture.png");
	heightmap = TTN_Texture2D::LoadFromFile("Review3/heightmap.bmp");

	//create material pointers and set them up
	waterMat = TTN_Material::Create();
	waterMat->SetAlbedo(waterText);
	waterMat->SetShininess(128.0f);
	birdMat = TTN_Material::Create();
	birdMat->SetAlbedo(birdText);
	birdMat->SetShininess(128.0f);
	flamethrowerMat = TTN_Material::Create();
	flamethrowerMat->SetAlbedo(flamethrowerText);
	flamethrowerMat->SetShininess(128.0f);
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
	testParticle.SetMesh(sphereMesh);
	testParticle.SetOneEndColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
	testParticle.SetOneEndSize(0.01f);
	testParticle.SetOneEndSpeed(2.0f);
	testParticle.SetOneLifetime(1.0f);
	testParticle.SetOneStartColor(glm::vec4(1.0f));
	testParticle.SetOneStartSize(0.01f);
	testParticle.SetOneStartSpeed(1.0f);

	fireMat = TTN_Material::Create();
	fireMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle

	fireParticle = TTN_ParticleTemplate();
	fireParticle.SetMat(fireMat);
	fireParticle.SetMesh(sphereMesh);
	fireParticle.SetOneEndColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	fireParticle.SetOneEndSize(0.80f);
	fireParticle.SetOneEndSpeed(4.0f);
	fireParticle.SetOneLifetime(1.5f);
	fireParticle.SetOneStartColor(glm::vec4(1.0f, 0.60f, 0.0f, 1.0f));
	fireParticle.SetOneStartSize(1.0f);
	fireParticle.SetOneStartSpeed(8.0f);

	TTN_Application::TTN_Input::SetCursorLocked(true);

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
		TTN_Transform testpsTrans = TTN_Transform(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy(testParticleSystem, testpsTrans);

		//setup a particle system for the particle system
		TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(1000, 500, testParticle, 5.0f, true);
		ps->MakeConeEmitter(15.0f, glm::vec3(90.0f, 0.0f, 90.0f));
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

	//set the cannon to be a child of the camera
	Get<TTN_Transform>(cannon).SetParent(&Get<TTN_Transform>(camera), &camera);

	/////// other /////////
	rotAmmount = glm::vec2(0.0f, 0.0f);
	mousePos = TTN_Application::TTN_Input::GetMousePosition();
}

void Review3Scene::Update(float deltaTime)
{
	//get the mouse position
	glm::vec2 tempMousePos = TTN_Application::TTN_Input::GetMousePosition();

	//figure out how much the cannon and camera should be rotated
	rotAmmount += (tempMousePos - mousePos) * 5.0f * deltaTime;

	//clamp the rotation to within 85 degrees of the base rotation in all the directions
	if (rotAmmount.x > 85.0f) rotAmmount.x = 85.0f;
	else if (rotAmmount.x < -85.0f) rotAmmount.x = -85.0f;
	if (rotAmmount.y > 85.0f) rotAmmount.y = 85.0f;
	else if (rotAmmount.y < -85.0f) rotAmmount.y = -85.0f;

	//reset the rotation
	Get<TTN_Transform>(camera).LookAlong(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//and rotate it by the ammount it should be rotated
	Get<TTN_Transform>(camera).RotateFixed(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f));

	//save the next position to rotate properly next frame
	mousePos = tempMousePos;

	//parameters: number of waves, rest time between waves, length of waves, deltatime
	Waves(3, 10.f, 30.0f, deltaTime); //first wave is shorter because delta time starts incrementing before scene loads in

	SpawnerLS(deltaTime, 5.0f);//sets the spawner and gives the interval of time the spawner should spawn boats
	SpawnerRS(deltaTime, 3.0f);//sets the spawner and gives the interval of time the spawner should spawn boats
	//BoatPathing(boat2, 4);

	//goes through the boats vector
	for (int i = 0; i < boats.size(); i++) {
		//std::cout << "Path: " << Get<TTN_Tag>(boats[i]).getPath() << std::endl;
		int p = Get<TTN_Tag>(boats[i]).getPath(); //gets the boats randomized path num
		BoatPathing(boats[i], p); //updates the pathing for the boat
	}

	//printf("fps: %f\n", 1.0f/deltaTime);

	TTN_Scene::Update(deltaTime);
}

#pragma region INPUTS
void Review3Scene::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Q)) {
		BirdBomb();
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Two)) {
		Flamethrower();
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

	//auto& cannonTrans = Get<TTN_Transform>(cannon);
	auto& transCamera = Get<TTN_Transform>(camera);

	TTN_Transform ballTrans = TTN_Transform(glm::vec3(10.f, -5.0f, 20.0f), glm::vec3(0.0f), glm::vec3(0.75f));
	glm::vec3 forward = glm::mat3(glm::inverse(transCamera.GetMatrix())) * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 origin = forward + transCamera.GetPos();
	//origin = glm::normalize(origin);

	ballTrans.SetPos(glm::vec3((origin.x, origin.y + 5.0f, origin.z + 5.0f)));

	ballTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
	ballTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));

	AttachCopy<TTN_Transform>(ball, ballTrans);
	float power = 10.f;
	//glm::vec3 vel = forward * power;
	glm::vec3 vel = glm::normalize(transCamera.GetRotation()) * power;

	TTN_Physics pbody = TTN_Physics(ballTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), ball, TTN_PhysicsBodyType::DYNAMIC, 1.5f);
	pbody.SetHasGravity(true); //doesn't do anything for now
	pbody.SetLinearVelocity(glm::vec3(vel.x, -2.f, vel.z));
	//pbody.SetLinearVelocity(glm::vec3(0.f, -2.0f, 12.0f));
	//pbody.SetLinearVelocity(glm::vec3(origin.x * 6.0f, -1.0f, origin.z * 6.0f));

	//std::cout << glm::to_string(ball) << std::endl;
	//std::cout << "Ball: " << cannonballs.size() << std::endl;

	AttachCopy<TTN_Physics>(ball, pbody);
}

//sets the pathing the boat entity should take based on the path integer (1-3 is left side, 4-6 is right side)
void Review3Scene::BoatPathing(entt::entity boatt, int path)
{
	auto& pBoat = Get<TTN_Physics>(boatt);
	auto& tBoat = Get<TTN_Transform>(boatt);

	//left side middle path
	if (path == 1) {
		if (tBoat.GetPos().x <= 30.f) {
			if (tBoat.GetRotation().y <= 55.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.55f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(0.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(5.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
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
			//pBoat.SetLinearVelocity(Seek(glm::vec3(20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//center left
	if (path == 3) {
		if (tBoat.GetPos().x <= 20.f && !(tBoat.GetPos().x <= 3.f)) {
			if (tBoat.GetRotation().y <= 60.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.75f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
		if (tBoat.GetPos().x <= 3.f) {
			if (tBoat.GetRotation().y <= 89.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.95f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(2.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(3.0f, -5.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//right middle path
	if (path == 4) {
		if (tBoat.GetPos().x >= -30.F) {
			if (tBoat.GetRotation().y <= 55.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.75f, 0.0f));
			}
			//tBoat.LookAlong(glm::vec3(5.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			//pBoat.SetLinearVelocity(Seek(glm::vec3(0.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(0.0f, -5.0f, 5.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//far right path
	if (path == 5) {
		if (tBoat.GetPos().x >= -35.f) {
			if (tBoat.GetRotation().y <= 85.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.95f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(-20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(-20.0f, -5.0f, 3.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//right center path
	if (path == 6) {
		if (tBoat.GetPos().x >= -20.f && !(tBoat.GetPos().x >= -3.F)) {
			if (tBoat.GetRotation().y <= 60.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.75f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(-2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(-2.0f, -5.0f, 40.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}

		if (tBoat.GetPos().x >= -3.f) {
			if (tBoat.GetRotation().y <= 89.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.95f, 0.0f));
			}
			//pBoat.SetLinearVelocity(Seek(glm::vec3(-2.0f, -5.0f, 4.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			pBoat.AddForce(Seek(glm::vec3(-3.0f, -5.0f, 4.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}
}

void Review3Scene::SpawnerLS(float deltatime, float SpawnTime) {
	//increment timer
	Timer += deltatime;

	//if the timer is >= spawn time then it will spawn the boat
	if (Timer >= SpawnTime && Spawning) {
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
		pbody.SetLinearVelocity(glm::vec3(-10.0f, 0.f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boat, pbody);

		//add a tag to the boat
		//TTN_Tag boatTag = TTN_Tag(" Boat "+ boats.size() );

		int r = rand() % 3 + 1; // generates path number between 1-3 (left side paths, right side path nums are 4-6)
		//std::cout << "Num: " << r << std::endl; //random boat path
		TTN_Tag boatTag = TTN_Tag(r); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(boat, boatTag);
	}
}

void Review3Scene::SpawnerRS(float deltatime, float SpawnTime)
{
	Timer2 += deltatime;

	if (Timer2 >= SpawnTime && Spawning) {
		// timer = 0, boat spawn code
		Timer2 = 0.F;//reset timer

		boat2 = CreateEntity();
		boats.push_back(boat2);

		TTN_Renderer boatRenderer = TTN_Renderer(boatMesh, shaderProgam);
		AttachCopy<TTN_Renderer>(boat2, boatRenderer);

		TTN_Transform boatTrans = TTN_Transform();
		boatTrans.SetPos(glm::vec3(-70.0f, -5.0f, 70.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		AttachCopy<TTN_Transform>(boat2, boatTrans);

		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), boat, TTN_PhysicsBodyType::DYNAMIC, 1.0f);
		pbody.SetHasGravity(false);
		pbody.SetLinearVelocity(glm::vec3(10.0f, 0.f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boat2, pbody);

		int r = rand() % 3 + 4; // generates path number between 4-6 (left side paths 1-3, right side path nums are 4-6)
		//std::cout << "Num: " << r << std::endl; //random boat path

		TTN_Tag boatTag = TTN_Tag(r); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(boat2, boatTag);
	}
}

void Review3Scene::Waves(int num, float restTime, float waveTime, float deltaTime)
{
	if (num > wave) {
		//printf("Wave now!\n");
		waveTimer += deltaTime;

		if (waveTimer >= waveTime) {
			Spawning = false;
			printf("wavetimer over!\n");
			waveTimer = 0;
		}

		if (!Spawning) {
			printf("resting!\n");
			restTimer += deltaTime;
			if (restTimer >= restTime) {
				Spawning = true;
				restTimer = 0;
				printf("Spawn now!\n");
				wave++;
			}
		}
	}

	else {
		printf("GAME over!\n");
	}
}

void Review3Scene::Flamethrower() {
	flamethrower = CreateEntity();

	//setup a mesh renderer for the cannon
	TTN_Renderer ftRenderer = TTN_Renderer(flamethrowerMesh, shaderProgamTextured);
	ftRenderer.SetMat(flamethrowerMat);
	//attach that renderer to the entity
	AttachCopy<TTN_Renderer>(flamethrower, ftRenderer);

	//setup a transform for the cannon
	TTN_Transform ftTrans = TTN_Transform(glm::vec3(5.0f, -3.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.40f));

	//attach that transform to the entity
	AttachCopy<TTN_Transform>(flamethrower, ftTrans);

	{
		ftParticle = CreateEntity();

		//setup a transfrom for the particle system
		TTN_Transform firePSTrans = TTN_Transform(glm::vec3(3.5f, -3.0f, 4.5f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy(ftParticle, firePSTrans);

		//setup a particle system for the particle system
		TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(1200, 400, fireParticle, 4.0f, true);
		ps->MakeConeEmitter(20.0f, glm::vec3(90.0f, 0.0f, 0.0f));

		//setup a particle system component
		TTN_ParticeSystemComponent psComponent = TTN_ParticeSystemComponent(ps);
		//attach the particle system component to the entity
		AttachCopy(ftParticle, psComponent);
	}
	//Get<TTN_ParticeSystemComponent>(ftParticle).GetParticleSystemPointer()->Burst(450);

}

void Review3Scene::BirdBomb() {
	playerDir = glm::vec3(0.0f, 0.0f, 1.0f);
	playerDir = glm::vec3((glm::toMat4(glm::quat(glm::radians(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f))))) * glm::vec4(playerDir, 1.0f));

	playerDir = glm::normalize(playerDir) * (glm::length(playerDir) * 95.f);

	std::cout << glm::to_string(playerDir) << std::endl;

	bird = CreateEntity();

	//setup a mesh renderer for the cannon
	TTN_Renderer birdRenderer = TTN_Renderer(birdMesh, shaderProgamTextured);
	birdRenderer.SetMat(birdMat);
	//attach that renderer to the entity
	AttachCopy<TTN_Renderer>(bird, birdRenderer);

	//setup a transform for the cannon
	TTN_Transform birdTrans = TTN_Transform(glm::vec3(playerDir), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.40f));
	birdTrans.SetPos(playerDir);
	birdTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
	//attach that transform to the entity
	AttachCopy<TTN_Transform>(bird, birdTrans);

	TTN_Physics pbody = TTN_Physics(birdTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f), bird, TTN_PhysicsBodyType::DYNAMIC, 1.0f);
	pbody.SetHasGravity(true); //doesn't do anything for now

	glm::vec3 vel = 20.f * playerDir;
	//std::cout << glm::to_string(vel) << std::endl;
	AttachCopy<TTN_Physics>(bird, pbody);

	//glm::vec3 bpos = Get<TTN_Transform>(*it).GetPos();
	//glm::vec3 cross = glm::cross(bpos, playerDir);
	//float l = glm::length(cross);
	//pbody.SetLinearVelocity(glm::normalize(cross) * 10.f * l);
	entt::entity b;
	float shortest(1.0f);
	std::vector<entt::entity>::iterator it = boats.begin();
	while (it != boats.end()) {
		if (glm::distance(Get<TTN_Transform>(*it).GetGlobalPos(), playerDir) < shortest) {
			shortest = glm::distance(Get<TTN_Transform>(*it).GetGlobalPos(), playerDir);
			b = *it;
		}

		if (Get<TTN_Transform>(*it).GetGlobalPos() != playerDir) {
			it++;
			//pbody.SetLinearVelocity(vel);
		}
		else {
			it++;
		}
	}

	//BirdSeek();
}

glm::vec3 Review3Scene::Seek(glm::vec3 target, glm::vec3 velo, glm::vec3 pos)
{
	//std::cout << glm::to_string(target) << std::endl;
	//std::cout << glm::to_string(velo) << std::endl;
	//std::cout << glm::to_string(pos) << std::endl;
	glm::vec3 maxVelo(-10.0f, 0.f, -10.0f);
	glm::vec3 desired = (pos - target); //gets the desired vector

	desired = glm::normalize(desired) * maxVelo;
	glm::vec3 steering = desired - velo;

	glm::vec3 moveVelo = steering;  //clamp(moveVelo,velo+steering, maxVelo);

	return moveVelo;
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