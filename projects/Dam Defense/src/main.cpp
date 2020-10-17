#include "Titan/Scene.h"
#include "Titan/Application.h"

using namespace Titan;

int main() {
	TTN_Application::Init("Dam Defense", 800, 800);

	//create a shader program object
	TTN_Shader::sshptr shaderProgam = TTN_Shader::Create(); 
	//load the shaders into the shader program 
	shaderProgam->LoadShaderStageFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	shaderProgam->LoadShaderStageFromFile("shaders/frag_shader.glsl", GL_FRAGMENT_SHADER);
	shaderProgam->Link(); 

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

	//create a new scene
	TTN_Scene testScene = TTN_Scene();

	//create an entity in the scene for the triangle
	auto triEntity = testScene.CreateEntity();

	testScene.AttachCopy<TTN_Transform>(triEntity, triTrans);
	testScene.AttachCopy<TTN_Renderer>(triEntity, triRenderer);

	//add the scene to the application
	TTN_Application::scenes.push_back(testScene);

	while (!TTN_Application::GetIsClosing())
		TTN_Application::Update();

	return 0;
}