//Titan Engine, by Atlas X Games 
// Shader.cpp - source file for the class that wraps around an openGL shader program 
#include "Titan/Shader.h"
#include "Logging.h"
#include <fstream>
#include <sstream>

namespace Titan {
	//default constructor, makes an empty shader program 
	TTN_Shader::TTN_Shader() :
		_vs(0), _fs(0), _handle(0)
	{
		_handle = glCreateProgram();
		setDefault = false;
		vertexShaderTTNIndentity = 0;
		fragShaderTTNIdentity = 0;
	}

	//destructor, deletes program 
	TTN_Shader::~TTN_Shader()
	{
		//if the program exists within opengl
		if (_handle != 0) {
			//then delete it and set the handle to 0 again
			glDeleteProgram(_handle);
			_handle = 0;
		}
	}

	//Load a shader stage into the pipeline 
	bool TTN_Shader::LoadShaderStage(const char* sourceCode, GLenum shaderType)
	{
		//check if it's loading a default shader
		if (!setDefault) {
			//if it's not, then mark the shader as custom
			if (shaderType == GL_VERTEX_SHADER)
				vertexShaderTTNIndentity = 0;
			else
				fragShaderTTNIdentity = 0;
		}

		//Create the new shader steage (vs, fs, etc.)
		GLuint handle = glCreateShader(shaderType);

		//Load and compile the GLSL sourcecode 
		glShaderSource(handle, 1, &sourceCode, nullptr);
		glCompileShader(handle);

		//Get the compilation status of the shader stage (so we can check if it compiled properly)
		GLint status = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

		//check if it compiled correctly
		if (status == GL_FALSE) {
			//if it did not, create an error log
			//get the size of the error for the log 
			GLint logSize = 0;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

			//create a new character array buffer for the log to store in 
			char* log = new char[logSize];

			//get the log
			glGetShaderInfoLog(handle, logSize, &logSize, log);

			//transfer the error log to our own logging files 
			LOG_ERROR("Failed to compile shader stage:\n{}", log);

			//clean up the memory of our log (it's been dumped to an external file so we don't need it here anymore)
			delete[] log; 

			//Delete the broken shader stage so it doesn't waste memory 
			glDeleteShader(handle);
			handle = 0;
		}

		//set the shader variables to the handle so they can be accessed again later 
		switch (shaderType) {
			case GL_VERTEX_SHADER: //if it's a vertex shader, set the vertex shader variable
				_vs = handle; 
				break;
			case GL_FRAGMENT_SHADER: //if it's a fragment shader, set the vertex shader variable
				_fs = handle;
				break; 
			default: //if it is anything else, log a warning that that type of shader has not been implemented with this shader program 
				LOG_WARN("Shader type not implemented"); 
				break;
		}


		return status != GL_FALSE;
	}

	//Load a shader stage from an external file into the pipeline 
	bool TTN_Shader::LoadShaderStageFromFile(const char* filePath, GLenum shaderType)
	{
		//open the file
		std::ifstream file(filePath);
		//check if the file failed to open
		if (!file.is_open()) {
			//if it did fail to open log an error
			LOG_ERROR("Shader file not found: {}", filePath);
			//and throw a runtime error
			throw std::runtime_error("File not found, see logs");
		} 
		//if it did open correctly then make a stream to parse it 
		std::stringstream stream;
		//begin parsing it 
		stream << file.rdbuf();
		//use the load function earlier to load the shader from the stream and save if it was sucessful in a boolean 
		bool result = LoadShaderStage(stream.str().c_str(), shaderType);
		//close the file 
		file.close();

		//return the result of the earlier load
		return result;
	}

	bool TTN_Shader::LoadDefaultShader(TTN_DefaultShaders shader)
	{
		//make a variable to store the filepath
		const char* filePath;
		//make a variable to store the result of the loading
		bool result;

		//mark the shader being loaded as default
		setDefault = true;

		//check which shader the user is trying to load, and set the filepath to the approriate shader
		if (shader == TTN_DefaultShaders::VERT_NO_COLOR) {
			filePath = "shaders/ttn_vert_no_color.glsl";
			result = LoadShaderStageFromFile(filePath, GL_VERTEX_SHADER);
			vertexShaderTTNIndentity = (int)shader;
		}
		else if (shader == TTN_DefaultShaders::VERT_COLOR) {
			filePath = "shaders/ttn_vert_color.glsl";
			result = LoadShaderStageFromFile(filePath, GL_VERTEX_SHADER);
			vertexShaderTTNIndentity = (int)shader;
		}
		else if (shader == TTN_DefaultShaders::FRAG_BLINN_PHONG_NO_TEXTURE) {
			filePath = "shaders/ttn_frag_blinn_phong_no_texture.glsl";
			result = LoadShaderStageFromFile(filePath, GL_FRAGMENT_SHADER);
			fragShaderTTNIdentity = (int)shader;
		}
		else if (shader == TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY) {
			filePath = "shaders/ttn_frag_blinn_phong_textured_no_specular.glsl";
			result = LoadShaderStageFromFile(filePath, GL_FRAGMENT_SHADER);
			fragShaderTTNIdentity = (int)shader;
		}
		else if (shader == TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_AND_SPECULAR) {
			filePath = "shaders/ttn_frag_blinn_phong_textured_with_specular.glsl";
			result = LoadShaderStageFromFile(filePath, GL_FRAGMENT_SHADER);
			fragShaderTTNIdentity = (int)shader;
		}
		else {
			//if the user tried to load a shader that doesn't,
			LOG_ERROR("Default shader {} does not exist", shader);
			//and throw a runtime error
			throw std::runtime_error("Default Shader does not exist, see logs");
			result = false;
		}

		//clear the shader being loaded bool
		setDefault = false;

		return result;
	}

	bool TTN_Shader::Link()
	{
		//if the program doesn't have both a vertex and a fragment shader log an error
		LOG_ASSERT(_vs != 0 && _fs != 0, "Both a vertex and fragment shader need to be attached to the shader program.");

		//Attach our shaders 
		glAttachShader(_handle, _vs);
		glAttachShader(_handle, _fs);

		//Perform linking
		glLinkProgram(_handle);

		//Remove shader stages to save memory (because the shader program has now been compiled we no longer need them seperatedly) 
		glDetachShader(_handle, _vs);
		glDeleteShader(_vs);
		glDetachShader(_handle, _fs);
		glDeleteShader(_fs);

		//Setup a check to make sure the shader program compiled and linked correclty
		GLint status = 0;
		glGetProgramiv(_handle, GL_LINK_STATUS, &status);

		//check if it failed to compile or link
		if (status == GL_FALSE)
		{
			//if it did not, create an error log
			//get the size of the error for the log 
			GLint lenght = 0;
			glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &lenght);

			//if openGL has made an error log 
			if (lenght > 0) {
				//read it's log 
				char* log = new char[lenght];
				glGetProgramInfoLog(_handle, lenght, &lenght, log);
				//save the error in our own logs
				LOG_ERROR("Sgader failed to link:\n{}", log);
				delete[] log;
			}
			else {
				//if opengl did not generate an error log, log an error saying we don't know why it failed in our own logs
				LOG_ERROR("Shader failed to link for an unknown reason");
			}
		}

		//return wheter or not the link was sucessful 
		return status != GL_FALSE;
	}

	//bind the program so we can use it 
	void TTN_Shader::Bind()
	{
		glUseProgram(_handle);
	}

	//unbind the program
	void TTN_Shader::UnBind()
	{
		glUseProgram(0);
	}

#pragma region Uniform_Setters
	//set a uniform for a 3x3 matrix so it can be recieved by the shaders
	void TTN_Shader::SetUniformMatrix(int location, const glm::mat3* value, int count, bool transposed)
	{
		glProgramUniformMatrix3fv(_handle, location, count, transposed, glm::value_ptr(*value));
	}
	
	//set a uniform for a 4x4 matrix so it can be recieved by the shaders 
	void TTN_Shader::SetUniformMatrix(int location, const glm::mat4* value, int count, bool transposed)
	{
		glProgramUniformMatrix4fv(_handle, location, count, transposed, glm::value_ptr(*value));
	}

	//set a uniform for a float salar so it can recieved by the shaders
	void TTN_Shader::SetUniform(int location, const float* value, int count)
	{
		glProgramUniform1fv(_handle, location, count, value);
	}

	//set a uniform for a 2D float vector so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::vec2* value, int count)
	{
		glProgramUniform2fv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for a 3D float vector so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::vec3* value, int count)
	{
		glProgramUniform3fv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for a 4D float vector so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::vec4* value, int count)
	{
		glProgramUniform4fv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for an integer scalar so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const int* value, int count)
	{
		glProgramUniform1iv(_handle, location, count, value);
	}

	//set a uniform for a 2D integer vector so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::ivec2* value, int count)
	{
		glProgramUniform2iv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for a 3D integer vector so it can be recieved by the shaders 
	void TTN_Shader::SetUniform(int location, const glm::ivec3* value, int count)
	{
		glProgramUniform3iv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for a 4D integer vector so it can be recieved by the shaders 
	void TTN_Shader::SetUniform(int location, const glm::ivec4* value, int count)
	{
		glProgramUniform4iv(_handle, location, count, glm::value_ptr(*value));
	}

	//set a uniform for a single boolean so it can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const bool* value, int count)
	{
		LOG_ASSERT(count == 1, "SetUniform for bools only supports setting single values at a time!");
		glProgramUniform1i(location, *value, 1);
	}

	//set a uniform for 2 booleans so they can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::bvec2* value, int count)
	{
		LOG_ASSERT(count == 1, "SetUniform for bools only supports setting single values at a time!");
		glProgramUniform2i(location, value->x, value->y, 1);
	}

	//set a uniform for 3 booleans so they can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::bvec3* value, int count)
	{
		LOG_ASSERT(count == 1, "SetUniform for bools only supports setting single values at a time!");
		glProgramUniform3i(location, value->x, value->y, value->z, 1);
	}

	//set a unifrom for 4 booleans so they can be recieved by the shaders
	void TTN_Shader::SetUniform(int location, const glm::bvec4* value, int count)
	{
		LOG_ASSERT(count == 1, "SetUniform for bools only supports setting single values at a time!");
		glProgramUniform4i(location, value->x, value->y, value->z, value->w, 1);
	}

#pragma endregion Uniform_Setters


	int TTN_Shader::__GetUniformLocation(const std::string& name)
	{
		//search the unordered map for a location marked by the given name
		std::unordered_map<std::string, int>::const_iterator it = _uniformLocations.find(name);
		int result = -1;

		//if we find something at a location marked by the name, call get uniform and store it so we can save for the next use
		if (it == _uniformLocations.end()) {
			result = glGetUniformLocation(_handle, name.c_str());
			_uniformLocations[name] = result;
		}
		//otherwise there was already a value at the location, return that value
		else {
			result = it->second;
		}

		//return the result
		return result;
	}
}