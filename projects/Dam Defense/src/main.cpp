#include "Titan/Scene.h"
#include "Titan/Application.h"

using namespace Titan;

int main() {
	TTN_Application::Init("Dam Defense", 800, 800);

	//create a shader program object
	TTN_Shader shaderProgam = TTN_Shader(); 
	//load the shaders into the shader program 
	shaderProgam.LoadShaderStageFromFile("vertex_shader.glsl", GL_VERTEX_SHADER);
	shaderProgam.LoadShaderStageFromFile("frag_shader.glsl", GL_FRAGMENT_SHADER);

	//create a mesh for the triangle
	TTN_Mesh triMesh = TTN_Mesh();
	//create the vertex position
	std::vector<glm::vec3> triVerts;
	triVerts[0] = glm::vec3(1.0f, 1.0f, 1.0f);
	triVerts[1] = glm::vec3(0.0f, 0.0f, 1.0f);
	triVerts[2] = glm::vec3(1.0f, 0.0f, 1.0f);
	triMesh.setVertices(triVerts);
	//create the vertex normals
	std::vector<glm::vec3> triNorms;
	for (int i = 0; i < triNorms.size(); i++) triNorms[i] = glm::vec3(0.0f, 0.0f, 1.0f);
	triMesh.setNormals(triNorms);
	//create the vertex uvs
	std::vector<glm::vec2> triUVs;
	for (int i = 0; i < triUVs.size(); i++) triUVs[i] = glm::vec2(0.0f, 0.0f);
	triMesh.setUVs(triUVs);

	//setup a transform for the trinagle
	TTN_Transform triTrans = TTN_Transform();
	triTrans.SetPos(glm::vec3(1.0f, 1.0f, 0.0f));
	triTrans.SetRot(glm::vec3(0.0f, 0.0f, 0.0f));
	triTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

	//setup a mesh renderer for the trinagle
	TTN_Renderer triRenderer = TTN_Renderer(triMesh);

	//create a new scene
	TTN_Scene testScene = TTN_Scene();

	//create an entity in the scene for the triangle
	auto triEntity = testScene.CreateEntity();

	testScene.AttachCopy<TTN_Transform>(triEntity, triTrans);
	testScene.AttachCopy<TTN_Renderer>(triEntity, triRenderer);

	while (!TTN_Application::GetIsClosing())
		TTN_Application::Update();

	return 0;
}