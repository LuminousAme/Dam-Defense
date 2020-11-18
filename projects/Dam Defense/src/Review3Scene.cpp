#include "Review3Scene.h"

Review3Scene::Review3Scene()
	: TTN_Scene()
{
}

void Review3Scene::InitScene()
{
	////////// resources /////////////////

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
	cannonMesh = TTN_ObjLoader::LoadFromFile("Review3/Cannon.obj");
	cannonMesh->SetUpVao();
	plane = TTN_ObjLoader::LoadFromFile("Review3/water.obj");
	plane->SetUpVao();
	tree1Mesh = TTN_ObjLoader::LoadFromFile("Review3/Tree 1.obj");
	tree1Mesh->SetUpVao();

	//create textrure pointers and load the textures in 
	waterText = TTN_Texture2D::LoadFromFile("Review3/water_text.png");
	cannonText = TTN_Texture2D::LoadFromFile("Review3/Metal_Texture_2.jpg");

	//create material pointers and set them up
	waterMat = TTN_Material::Create();
	waterMat->SetAlbedo(waterText);
	waterMat->SetShininess(128.0f);
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);


	////////// entities /////////////////

	//entity for the camera
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
		Get<TTN_Camera>(camera).CalcPerspective(60.0f, 1.78f, 0.01f, 100.f);
		//Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.78f, 0.01f, 100.f);
		Get<TTN_Camera>(camera).View();
	}

	//entity for the light
	{
		//create an entity in the scene for a light
		light = CreateEntity();
		SetLightEntity(light);

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
	}

	//entity for a tree
	{
		tree1 = CreateEntity();

		//setup a mesh renderer for the tree
		TTN_Renderer treeRenderer = TTN_Renderer(tree1Mesh, shaderProgam);
		//and attach that renderer to the entity
		AttachCopy<TTN_Renderer>(tree1, treeRenderer);

		//setup a transform for the tree
		TTN_Transform treeTrans = TTN_Transform(glm::vec3(10.f, -5.0f, 20.0f), glm::vec3(0.0f), glm::vec3(0.5f));
		//and attach that transform to the entity
		AttachCopy<TTN_Transform>(tree1, treeTrans);
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
	

	//Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, 5.0f * deltaTime, 0.0f));

	auto& transCannon = Get<TTN_Transform>(cannon);
	auto& transCamera = Get<TTN_Transform>(camera);
	
	//check if the mouse has moved on the x-axis
	/*if (tempMousePos.x != mousePos.x) {
		//if it, get the difference
		float diff = tempMousePos.x - mousePos.x;
		//if it's moving in the positive direction, and the angle in that direction is less than 85 degrees
		if (diff > 0) {
			//rotate by 5 degrees a second in that direction
			rotAmmount.x += 1.0f * deltaTime;
			Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, 25.0f * deltaTime, 0.0f));
		}
		//if it's moving in the negative direction, and the angle in that direction is less than 85 degrees
		else if (diff < 0) {
			//rotate by 5 degrees a second in that direction
			rotAmmount.x += -1.0f * deltaTime;
			Get<TTN_Transform>(camera).RotateFixed(glm::vec3(0.0f, -25.0f * deltaTime, 0.0f));
		}
	}*/

	mousePos = tempMousePos;
	TTN_Scene::Update(deltaTime);
}

void Review3Scene::KeyDownChecks()
{
}

void Review3Scene::KeyChecks()
{
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
		Get<TTN_Transform>(camera).SetPos(Get<TTN_Transform>(camera).GetPos());
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
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

void Review3Scene::CreateCannonball()
{
}
