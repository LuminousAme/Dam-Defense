//Dam Defense by Atlas X Games
//WaterManager.cpp, the source file for the class that manages the water effect
#include "WaterManager.h"

void WaterManager::Init()
{
	m_waveFrequency.clear();
	m_waveDirection.clear();
	m_waveAcutalAmplitude.clear();
	m_waveTargetAmplitude.clear();
	m_waveTimeAlive.clear();

	for (int i = 0; i < m_numberOfWaves; i++) {
		//generate a wavelenght
		float lenght = TTN_Random::RandomFloat(m_medianWaveLenght * 0.75f, m_medianWaveLenght * 1.25f);
		//calculate the frequency based on the wavelenght
		float freq = glm::sqrt(m_gravationalConstant * ((2.0f * glm::pi<float>()) / lenght));

		//get the ratio between the median amplitude and wave lenght
		float ratio = m_medianAmplitude / m_medianWaveLenght;
		//use that to calculate the target amplitude
		float targetAmp = ratio * lenght;

		//use the current iteration, number of total waves, and the total life time to calculate how far into
		//it's life each wave currently is, this allows us to displace waves fading in and out from each other
		float currentTimeAlive = i * (m_lifeTime / float(m_numberOfWaves));

		//take the base direction of the wave
		glm::vec3 dir = m_basedirection;

		//and generate a random rotation within a given angle to it 
		glm::vec3 Rot = glm::vec3(TTN_Random::RandomFloat(-m_angle, m_angle), 0.0f, TTN_Random::RandomFloat(-m_angle, m_angle));

		//and rotate it by that ammount
		glm::quat RotQuat = glm::quat(glm::radians(Rot));
		glm::mat4 RotMat = glm::toMat4(RotQuat);
		dir = glm::vec3(RotMat * glm::vec4(dir, 1.0f));

		//get the acutal amplitude
		float t = glm::clamp(currentTimeAlive / m_lifeTime, 0.0f, 1.0f);
		float acutalAmp = TTN_Interpolation::Lerp(0.0f, targetAmp, waterLifeInterpolationParameter(t));

		//push the values into the vectors
		m_waveFrequency.push_back(freq);
		m_waveDirection.push_back(glm::normalize(dir));
		m_waveAcutalAmplitude.push_back(acutalAmp);
		m_waveTargetAmplitude.push_back(targetAmp);
		m_waveTimeAlive.push_back(currentTimeAlive);
	}
	
	//set up voronoi stuff

	//make the framebuffer
	m_voronoiBuffer = TTN_Framebuffer::Create();
	m_voronoiBuffer->AddColorTarget(GL_RGBA8);
	m_voronoiBuffer->AddDepthTarget();
	m_voronoiBuffer->Init(1024, 1024);

	//make the shader
	m_voronoiShader = TTN_Shader::Create();
	m_voronoiShader->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
	m_voronoiShader->LoadShaderStageFromFile("shaders/voronoi_frag.glsl", GL_FRAGMENT_SHADER);
	m_voronoiShader->Link();

	//set up reflection and refraction stuff

	//make the framebuffers
	m_reflectionBuffer = TTN_Framebuffer::Create();
	m_reflectionBuffer->AddColorTarget(GL_RGBA8);
	m_reflectionBuffer->AddDepthTarget();
	glm::ivec2 size = TTN_Backend::GetWindowSize();
	m_reflectionBuffer->Init(int(0.167f * float(size.x)), int(0.167f * float(size.y)));

	m_refractionBuffer = TTN_Framebuffer::Create();
	m_refractionBuffer->AddColorTarget(GL_RGBA8);
	m_refractionBuffer->AddDepthTarget();
	m_refractionBuffer->Init(int(0.67f * float(size.x)), int(0.67f * float(size.y)));

	//make the shaders
	m_clippingRegShader = TTN_Shader::Create();
	m_clippingRegShader->LoadShaderStageFromFile("shaders/clipping_regular_vert.glsl", GL_VERTEX_SHADER);
	m_clippingRegShader->LoadShaderStageFromFile("shaders/clipping_regular_frag.glsl", GL_FRAGMENT_SHADER);
	m_clippingRegShader->Link();

	m_clippingTerrainShader = TTN_Shader::Create();
	m_clippingTerrainShader->LoadShaderStageFromFile("shaders/clipping_terrain_vert.glsl", GL_VERTEX_SHADER);
	m_clippingTerrainShader->LoadShaderStageFromFile("shaders/clipping_terrain_frag.glsl", GL_FRAGMENT_SHADER);
	m_clippingTerrainShader->Link();
}

void WaterManager::MakeAWave(int index)
{
	//generate a wavelenght
	float lenght = TTN_Random::RandomFloat(m_medianWaveLenght * 0.5f, m_medianWaveLenght * 2.0f);
	//calculate the frequency based on the wavelenght
	float freq = glm::sqrt(m_gravationalConstant * ((2.0f * glm::pi<float>()) / lenght));

	//get the ratio between the median amplitude and wave lenght
	float ratio = m_medianAmplitude / m_medianWaveLenght;
	//use that to calculate the target amplitude
	float targetAmp = ratio * lenght;

	//use the current iteration, number of total waves, and the total life time to calculate how far into
	//it's life each wave currently is, this allows us to displace waves fading in and out from each other
	float currentTimeAlive = 0.0f;

	//take the base direction of the wave
	glm::vec3 dir = m_basedirection;

	//and generate a random rotation within a given angle to it 
	glm::vec3 Rot = glm::vec3(TTN_Random::RandomFloat(-m_angle, m_angle), 0.0f, TTN_Random::RandomFloat(-m_angle, m_angle));

	//and rotate it by that ammount
	glm::quat RotQuat = glm::quat(glm::radians(Rot));
	glm::mat4 RotMat = glm::toMat4(RotQuat);
	dir = glm::vec3(RotMat * glm::vec4(dir, 1.0f));

	//get the acutal amplitude
	float acutalAmp = 0.0f;

	//save the values into the vectors
	m_waveFrequency[index] = freq;
	m_waveDirection[index] = glm::normalize(dir);
	m_waveAcutalAmplitude[index] = acutalAmp;
	m_waveTargetAmplitude[index] = targetAmp;
	m_waveTimeAlive[index] = currentTimeAlive;
}

void WaterManager::Update(float deltaTime)
{
	//update the current time variable 
	m_time += deltaTime;

	//loop through each wave and update them 
	for (int i = 0; i < m_waveFrequency.size(); i++) {
		//update how long the wave has been alive
		m_waveTimeAlive[i] += deltaTime;

		//update it's acutal amplitude
		float t = glm::clamp(m_waveTimeAlive[i] / m_lifeTime, 0.0f, 1.0f);
		m_waveAcutalAmplitude[i] = TTN_Interpolation::Lerp(0.0f, m_waveTargetAmplitude[i], waterLifeInterpolationParameter(t));

		//if the wave's life time has ended, make a new wave to replace it
		if (m_waveTimeAlive[i] >= m_lifeTime)
			MakeAWave(i);
	}

	//if any waves have been added, reinit the system
	if (m_numberOfWaves != m_waveFrequency.size())
		Init();

	//render the voronoi effect
	m_voronoiBuffer->Clear();

	//bind the shader
	m_voronoiShader->Bind();
	
	//set the uniforms
	m_voronoiShader->SetUniform("u_gridFidelty", m_voronoiGridFidelty);
	m_voronoiShader->SetUniform("time", m_time);
	m_voronoiShader->SetUniform("u_speed", m_voronoiSpeed);

	//draw to the framebuffer
	m_voronoiBuffer->RenderToFSQ();

	//unbind the shader
	m_voronoiShader->UnBind();

	//and finally make sure the viewport is set correclty
	glm::ivec2 size = TTN_Backend::GetWindowSize();
	glViewport(0, 0, size.x, size.y);
}

//renders the scene to the reflection and refraction buffers so they can be used in the water shader later
void WaterManager::RenderSceneForWater(entt::registry* reg, entt::entity camera, entt::entity terrain, std::vector<entt::entity> exclude, glm::vec3 waterPos)
{
	m_refractionBuffer->Clear();
	m_reflectionBuffer->Clear();

	//create a list of every entity that should be rendered
	std::vector<entt::entity> renderList;

	//make a view with all of the entities with a transform and renderer
	auto view = reg->view<TTN_Transform, TTN_Renderer>();
	for (auto entity : view) {
		//check if the entity isn't in the exclusion list
		bool shouldExclude = false;
		for (int i = 0; i < exclude.size(); i++) {
			if (entity == exclude[i]) {
				shouldExclude = true;
				break;
			}
		}

		//if shouldn't be excluded add to the list of entities to render
		if (!shouldExclude) renderList.push_back(entity);
	}

	///// REFRACTION PASS ////////

	m_refractionBuffer->Bind();

	//set up the plane
	glm::vec4 plane = glm::vec4(0.0f, -1.0f, 0.0f, waterPos.y);

	//save the view and projection matrix
	glm::mat4 vp;
	vp = reg->get<TTN_Camera>(camera).GetProj();
	glm::mat4 viewMat = glm::inverse(reg->get<TTN_Transform>(camera).GetGlobal());
	vp *= viewMat;

	//now loop through and render them 
	for (auto entity : renderList) {
		//get references to the renderer and transform
		TTN_Renderer& renderer = reg->get<TTN_Renderer>(entity);
		TTN_Transform& trans = reg->get<TTN_Transform>(entity);

		//figure out which shader to use and bind the apporiate uniforms and textures
		if (renderer.GetShader()->GetVertexShaderDefaultStatus() == TTN_DefaultShaders::VERT_SKYBOX) {
			//if it's the skybox 
			//bind the skybox shader
			renderer.GetShader()->Bind();
			//bind the skybox texture
			renderer.GetMat()->GetSkybox()->Bind(0);
			//set the rotation matrix uniform
			renderer.GetShader()->SetUniformMatrix("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0))));
			//set the skybox matrix uniform
			renderer.GetShader()->SetUniformMatrix("u_SkyboxMatrix", reg->get<TTN_Camera>(camera).GetProj() * glm::mat4(glm::mat3(viewMat)));
		}
		else {
			//otherwise it must be the regular shader
			m_clippingRegShader->Bind();

			glm::mat4 model = reg->get<TTN_Transform>(entity).GetGlobal();
			m_clippingRegShader->SetUniformMatrix("MVP", vp * model);
			m_clippingRegShader->SetUniformMatrix("Model", model);
			m_clippingRegShader->SetUniformMatrix("NormalMat", glm::mat3(glm::transpose(glm::inverse(model))));

			//try to get an animator component
			if (reg->has<TTN_MorphAnimator>(entity)) {
				m_clippingRegShader->SetUniform("t", reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getInterpolationParameter());
			}
			else
				m_clippingRegShader->SetUniform("t", 0.0f);

			//if the entity has an animator
			if (reg->has<TTN_MorphAnimator>(entity)) {
				//set up the vao on the mesh properly
				renderer.GetMesh()->SetUpVao(reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getCurrentMeshIndex(),
					reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getNextMeshIndex());
			}
			//if it doesn't
			else {
				//set up the vao with both mesh indices on zero
				renderer.GetMesh()->SetUpVao();
			}

			//set up wheter it should use an albedo
			m_clippingRegShader->SetUniform("u_UseDiffuse", 1);
			//and find that albedo
			renderer.GetMat()->GetAlbedo()->Bind(0);

			//send the plane
			m_clippingRegShader->SetUniform("plane", plane);
		}

		glm::mat4 arr[] = { glm::mat4(1.0f) };
		renderer.Render(glm::mat4(1.0f), glm::mat4(1.0f), arr, glm::mat4(1.0f));
	}

	//now render the terrain
	{
		//bind the shader
		m_clippingTerrainShader->Bind();

		//vert shader
		//bind the height map texture
		TTN_AssetSystem::GetTexture2D("Terrain height map")->Bind(0);
		TTN_AssetSystem::GetTexture2D("Normal Map")->Bind(1);

		//pass the scale uniform
		m_clippingTerrainShader->SetUniform("u_scale", 0.15f);

		glm::mat4 model = reg->get<TTN_Transform>(terrain).GetGlobal();
		m_clippingTerrainShader->SetUniformMatrix("MVP", vp* model);
		m_clippingTerrainShader->SetUniformMatrix("Model", model);
		m_clippingTerrainShader->SetUniformMatrix("NormalMat", glm::mat3(glm::transpose(glm::inverse(model))));
		
		//frag shader
		//bind the textures
		TTN_AssetSystem::GetTexture2D("Sand texture")->Bind(2);
		TTN_AssetSystem::GetTexture2D("Rock texture")->Bind(3);
		TTN_AssetSystem::GetTexture2D("Grass texture")->Bind(4);

		//set if the albedo textures should be used
		m_clippingTerrainShader->SetUniform("u_UseDiffuse", 1);

		//set the plane
		m_clippingTerrainShader->SetUniform("plane", plane);

		//render the terrain
		TTN_AssetSystem::GetMesh("Terrain plane")->GetVAOPointer()->Render();

		m_clippingTerrainShader->UnBind();
	}

	m_refractionBuffer->Unbind();

	///// REFLECTION PASS ////////

	m_reflectionBuffer->Bind();

	//change the plane
	plane = glm::vec4(0.0f, 1.0f, 0.0f, -waterPos.y);

	//change the view matrix

	//first invert the pitch
	glm::vec3 forward = glm::normalize(glm::inverse(viewMat)[2]);
	forward.y *= -1;
	TTN_Transform reflectionTrans = reg->get<TTN_Transform>(camera);
	reflectionTrans.LookAlong(forward, glm::vec3(0.0f, 1.0f, 0.0f));
	
	//now that the pitch has been inverted let's move the position 
	float distance = glm::abs(reflectionTrans.GetPos().y - waterPos.y);
	reflectionTrans.SetPos(reflectionTrans.GetPos() - glm::vec3(0.0f, 2.0f * distance, 0.0f));

	//now we have a temporary camera with the correct settings we can set that up as the view matrix and render the whole scene again
	viewMat = glm::inverse(reflectionTrans.GetGlobal());
	vp = reg->get<TTN_Camera>(camera).GetProj();
	vp *= viewMat;

	//now loop through and render them
	for (auto entity : renderList) {
		//get references to the renderer and transform
		TTN_Renderer& renderer = reg->get<TTN_Renderer>(entity);
		TTN_Transform& trans = reg->get<TTN_Transform>(entity);

		//figure out which shader to use and bind the apporiate uniforms and textures
		if (renderer.GetShader()->GetVertexShaderDefaultStatus() == TTN_DefaultShaders::VERT_SKYBOX) {
			//if it's the skybox 
			//bind the skybox shader
			renderer.GetShader()->Bind();
			//bind the skybox texture
			renderer.GetMat()->GetSkybox()->Bind(0);
			//set the rotation matrix uniform
			renderer.GetShader()->SetUniformMatrix("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0))));
			//set the skybox matrix uniform
			renderer.GetShader()->SetUniformMatrix("u_SkyboxMatrix", reg->get<TTN_Camera>(camera).GetProj() * glm::mat4(glm::mat3(viewMat)));
		}
		else {
			//otherwise it must be the regular shader
			m_clippingRegShader->Bind();

			glm::mat4 model = reg->get<TTN_Transform>(entity).GetGlobal();
			glm::mat4 proj = reg->get<TTN_Camera>(camera).GetProj();
			m_clippingRegShader->SetUniformMatrix("MVP", vp * model);
			m_clippingRegShader->SetUniformMatrix("Model", model);
			m_clippingRegShader->SetUniformMatrix("NormalMat", glm::mat3(glm::transpose(glm::inverse(model))));

			//try to get an animator component
			if (reg->has<TTN_MorphAnimator>(entity)) {
				m_clippingRegShader->SetUniform("t", reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getInterpolationParameter());
			}
			else
				m_clippingRegShader->SetUniform("t", 0.0f);

			//if the entity has an animator
			if (reg->has<TTN_MorphAnimator>(entity)) {
				//set up the vao on the mesh properly
				renderer.GetMesh()->SetUpVao(reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getCurrentMeshIndex(),
					reg->get<TTN_MorphAnimator>(entity).getActiveAnimRef().getNextMeshIndex());
			}
			//if it doesn't
			else {
				//set up the vao with both mesh indices on zero
				renderer.GetMesh()->SetUpVao();
			}

			//set up wheter it should use an albedo
			m_clippingRegShader->SetUniform("u_UseDiffuse", 1);
			//and find that albedo
			renderer.GetMat()->GetAlbedo()->Bind(0);

			//send the plane
			m_clippingRegShader->SetUniform("plane", plane);
		}

		glm::mat4 arr[] = { glm::mat4(1.0f) };
		renderer.Render(glm::mat4(1.0f), glm::mat4(1.0f), arr, glm::mat4(1.0f));
	}

	//now render the terrain
	{
		//bind the shader
		m_clippingTerrainShader->Bind();

		//vert shader
		//bind the height map texture
		TTN_AssetSystem::GetTexture2D("Terrain height map")->Bind(0);
		TTN_AssetSystem::GetTexture2D("Normal Map")->Bind(1);

		//pass the scale uniform
		m_clippingTerrainShader->SetUniform("u_scale", 0.15f);
		//pass the mvp uniform
		m_clippingTerrainShader->SetUniformMatrix("MVP", vp * reg->get<TTN_Transform>(terrain).GetGlobal());
		//pass the model uniform
		m_clippingTerrainShader->SetUniformMatrix("Model", reg->get<TTN_Transform>(terrain).GetGlobal());
		//and pass the normal matrix uniform
		m_clippingTerrainShader->SetUniformMatrix("NormalMat",
			glm::mat3(glm::inverse(glm::transpose(reg->get<TTN_Transform>(terrain).GetGlobal()))));

		//frag shader
		//bind the textures
		TTN_AssetSystem::GetTexture2D("Sand texture")->Bind(2);
		TTN_AssetSystem::GetTexture2D("Rock texture")->Bind(3);
		TTN_AssetSystem::GetTexture2D("Grass texture")->Bind(4);

		//set if the albedo textures should be used
		m_clippingTerrainShader->SetUniform("u_UseDiffuse", 1);

		//set the plane
		m_clippingTerrainShader->SetUniform("plane", plane);

		//render the terrain
		TTN_AssetSystem::GetMesh("Terrain plane")->GetVAOPointer()->Render();

		m_clippingTerrainShader->UnBind();
	}

	m_reflectionBuffer->Unbind();
}