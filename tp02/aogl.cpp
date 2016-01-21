#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#include <cmath>

#include "glew/glew.h"

#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;    

// Shader utils
int check_link_error(GLuint program);
int check_compile_error(GLuint shader, const char ** sourceBuffer);
GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize);
GLuint compile_shader_from_file(GLenum shaderType, const char * fileName);

// OpenGL utils
bool checkError(const char* title);

#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

struct Camera
{
    float radius;
    float theta;
    float phi;
    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;
};
void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);

struct Light
{
    float intensity;
    glm::vec3 color;

	Light(float _intensity, glm::vec3 _color) : intensity(_intensity), color(_color)
	{

	}
};

struct PointLight : public Light
{
	glm::vec3 position;

	PointLight(float _intensity, glm::vec3 _color, glm::vec3 _position) :  Light(_intensity, _color), position(_position)
	{

	}

	void drawUI(int id)
	{
		if (ImGui::CollapsingHeader( ("point light " + patch::to_string(id)).c_str() ))
		{
			ImGui::PushID(("point" + patch::to_string(id)).c_str());
			ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
			ImGui::ColorEdit3("light color", &color[0]);
			ImGui::SliderFloat3("light position", &position[0], -10.f, 10.f);
			ImGui::PopID();
		}
	}
};

struct DirectionalLight : public Light
{
    glm::vec3 direction;

	DirectionalLight(float _intensity, glm::vec3 _color, glm::vec3 _direction) : 
		Light(_intensity, _color) , direction(_direction)
	{

	}

	void drawUI(int id)
	{
		if (ImGui::CollapsingHeader( ("directional light "+ patch::to_string(id)).c_str()  ))
		{
			ImGui::PushID(("directional" + patch::to_string(id)).c_str());
			ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
			ImGui::ColorEdit3("light color", &color[0]);
			ImGui::SliderFloat3("light direction", &direction[0], 0.f, 1.f);
			ImGui::PopID();
		}
	}
};

struct SpotLight : public Light
{
    glm::vec3 position;
    glm::vec3 direction;
    float angle;

    SpotLight(float _intensity, glm::vec3 _color, glm::vec3 _position, glm::vec3 _direction, float _angle) :
        Light(_intensity, _color), position(_position) , direction(_direction), angle(_angle)
    {

    }

	void drawUI(int id)
	{
		if (ImGui::CollapsingHeader( ("spot light "+patch::to_string(id)).c_str() ))
		{
			ImGui::PushID(("spot" + patch::to_string(id)).c_str());
			ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
			ImGui::ColorEdit3("light color", &color[0]);
			ImGui::SliderFloat3("light position", &position[0], -10.f, 10.f);
			ImGui::SliderFloat3("light direction", &direction[0], -1.f, 1.f);
			ImGui::SliderFloat("light angles", &angle, 0.f, glm::pi<float>());
			ImGui::PopID();
		}
	}
};

struct Material 
{
	GLuint glProgram;

	GLuint textureDiffuse;

	float specularPower;
	GLuint textureSpecular;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_specularPower;

	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;
	
	Material(GLuint _glProgram, GLuint _textureDiffuse, GLuint _textureSpecular, float _specularPower = 50) : 
		glProgram(_glProgram), textureDiffuse(_textureDiffuse), specularPower(_specularPower), textureSpecular(_textureSpecular)
	{
		uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
		uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
		uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");

		uniform_MVP = glGetUniformLocation(glProgram, "MVP");
		uniform_normalMatrix = glGetUniformLocation(glProgram, "NormalMatrix");

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);
	}

	void setUniform_MVP(glm::mat4& mvp)
	{
		glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
	}

	void setUniform_normalMatrix(glm::mat4& normalMatrix)
	{
		glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
	}

	void use()
	{
		//bind shaders
		glUseProgram(glProgram);

		//bind textures into texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSpecular);

		//send uniforms
		glUniform1f(uniform_specularPower, specularPower);
		glUniform1i(uniform_textureDiffuse, 0);
		glUniform1i(uniform_textureSpecular, 1);
	}
};

class LightManager
{
private : 
	std::vector<PointLight> pointLights;
	std::vector<DirectionalLight> directionalLights;
    std::vector<SpotLight> spotLights;

	GLuint uniform_pointLight_pos[120];
	GLuint uniform_pointLight_col[120];
	GLuint uniform_pointLight_int[120];

	GLuint uniform_directionalLight_dir[120];
	GLuint uniform_directionalLight_col[120];
	GLuint uniform_directionalLight_int[120];

    GLuint uniform_spotLight_dir[120];
    GLuint uniform_spotLight_col[120];
    GLuint uniform_spotLight_int[120];
    GLuint uniform_spotLight_pos[120];
    GLuint uniform_spotLight_angle[120];

	GLuint uniform_pointLight_count;
	GLuint uniform_directionalLight_count;
    GLuint uniform_spotLight_count;

    float globalIntensity;
    float globalAngle;

public : 
    LightManager() : globalIntensity(0.f)
	{

	}

	void addPointLight(PointLight light)
	{
		pointLights.push_back(light);
	}

	void addDirectionalLight(DirectionalLight light)
	{
		directionalLights.push_back(light);
	}

    void addSpotLight(SpotLight light)
    {
        spotLights.push_back(light);
    }

	void removePointLight(int index)
	{
		pointLights.erase(pointLights.begin() + index);
	}

	void removeDirectionalLight(int index)
	{
		directionalLights.erase(directionalLights.begin() + index);
	}

    void removeSpotLight(int index)
    {
        spotLights.erase(spotLights.begin() + index);
    }

    void changeAllLightIntensities(float _intensity)
    {
        for(int i = 0; i < pointLights.size(); i++)
        {
            pointLights[i].intensity = _intensity;
        }

        for(int i = 0; i < directionalLights.size(); i++)
        {
            directionalLights[i].intensity = _intensity*0.1f;
        }

        for(int i = 0; i < spotLights.size(); i++)
        {
            spotLights[i].intensity = _intensity;
        }
    }

    void changeAllSpotAngle(float _angle)
    {
        for(int i = 0; i < spotLights.size(); i++)
        {
            spotLights[i].angle = glm::radians(_angle);
        }
    }

	void init(GLuint glProgram)
	{
		uniform_pointLight_count = glGetUniformLocation(glProgram, "pointLight_count");
		uniform_directionalLight_count = glGetUniformLocation(glProgram, "directionalLight_count");
        uniform_spotLight_count = glGetUniformLocation(glProgram, "spotLight_count");

		for (int i = 0; i < 120; i++)
		{
            uniform_pointLight_pos[i] = glGetUniformLocation(glProgram, ("pointLights[" + patch::to_string(i) + "].position").c_str() );
            uniform_pointLight_col[i] = glGetUniformLocation(glProgram, ("pointLights[" + patch::to_string(i)  + "].color").c_str() );
            uniform_pointLight_int[i] = glGetUniformLocation(glProgram, ("pointLights[" + patch::to_string(i) + "].intensity").c_str() );

            uniform_directionalLight_dir[i] = glGetUniformLocation(glProgram, ("directionalLights[" + patch::to_string(i) + "].direction").c_str() );
            uniform_directionalLight_col[i] = glGetUniformLocation(glProgram, ("directionalLights[" + patch::to_string(i) + "].color").c_str() );
            uniform_directionalLight_int[i] = glGetUniformLocation(glProgram, ("directionalLights[" + patch::to_string(i) + "].intensity").c_str() );

            uniform_spotLight_dir[i] = glGetUniformLocation(glProgram, ("spotLights[" + patch::to_string(i) + "].direction").c_str() );
            uniform_spotLight_col[i] = glGetUniformLocation(glProgram, ("spotLights[" + patch::to_string(i) + "].color").c_str() );
            uniform_spotLight_int[i] = glGetUniformLocation(glProgram, ("spotLights[" + patch::to_string(i) + "].intensity").c_str() );
            uniform_spotLight_pos[i] = glGetUniformLocation(glProgram, ("spotLights[" + patch::to_string(i) + "].position").c_str() );
            uniform_spotLight_angle[i] = glGetUniformLocation(glProgram, ("spotLights[" + patch::to_string(i) + "].angle").c_str() );
		}
	}

	void renderLights()
	{
		glUniform1i(uniform_pointLight_count, pointLights.size());
		glUniform1i(uniform_directionalLight_count, directionalLights.size());
        glUniform1i(uniform_spotLight_count, spotLights.size());

		for (int i = 0; i < pointLights.size(); i++)
		{
			glUniform3fv(uniform_pointLight_pos[i], 1, glm::value_ptr( pointLights[i].position ) );
			glUniform3fv(uniform_pointLight_col[i], 1, glm::value_ptr( pointLights[i].color ) );
			glUniform1f(uniform_pointLight_int[i], pointLights[i].intensity);
		}

		for (int i = 0; i < directionalLights.size(); i++)
		{
			glUniform3fv(uniform_directionalLight_dir[i], 1, glm::value_ptr(directionalLights[i].direction));
			glUniform3fv(uniform_directionalLight_col[i], 1, glm::value_ptr(directionalLights[i].color));
			glUniform1f(uniform_directionalLight_int[i], directionalLights[i].intensity);
		}

        for (int i = 0; i < spotLights.size(); i++)
        {
            glUniform3fv(uniform_spotLight_dir[i], 1, glm::value_ptr(spotLights[i].direction));
            glUniform3fv(uniform_spotLight_col[i], 1, glm::value_ptr(spotLights[i].color));
            glUniform1f(uniform_spotLight_int[i], spotLights[i].intensity);
            glUniform3fv(uniform_spotLight_pos[i], 1, glm::value_ptr(spotLights[i].position));
            glUniform1f(uniform_spotLight_angle[i], spotLights[i].angle);
        }
	}

    void drawUI()
    {
		if (ImGui::Button("add pointLight"))
			addPointLight(PointLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1, 0)));

		ImGui::SameLine();
		if (ImGui::Button("add DirectionalLight"))
			addDirectionalLight(DirectionalLight(10, glm::vec3(1, 1, 1), glm::vec3(0, -1, 0)));

		ImGui::SameLine();
		if (ImGui::Button("add spotLight"))
			addSpotLight(SpotLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec3(0,-1,0), glm::radians(30.f)));


		int lightCount = 0;
		for (auto& light : spotLights)
		{
			light.drawUI(lightCount);
			lightCount++;
		}
		lightCount = 0;
		for (auto& light : directionalLights)
		{
			light.drawUI(lightCount);
			lightCount++;
		}
		lightCount = 0;
		for (auto& light : pointLights)
		{
			light.drawUI(lightCount);
			lightCount++;
		}
    }
};

struct Mesh 
{
	enum Vbo_usage {USE_INDEX = 0x01, USE_VERTICES = 0x02, USE_UVS = 0x04, USE_NORMALS = 0x08};
	enum Vbo_types{VERTICES = 0, NORMALS, UVS};

	int triangleCount;
	
	std::vector<int> triangleIndex;
	std::vector<float> uvs;
	std::vector<float> vertices;
	std::vector<float> normals;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	GLuint vao;

	unsigned int vbo_usage;

	int coordCountByVertex;

	GLenum primitiveType;

	Mesh(GLenum _primitiveType = GL_TRIANGLES, unsigned int _vbo_usage = (USE_INDEX | USE_VERTICES | USE_UVS | USE_NORMALS), int _coordCountByVertex = 3 ) : primitiveType(_primitiveType), coordCountByVertex(_coordCountByVertex), vbo_usage(_vbo_usage), triangleCount(0), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0)
	{

	}

	~Mesh()
	{
		if (vbo_index != 0)
			glDeleteBuffers(1, &vbo_index);

		if (vbo_vertices != 0)
			glDeleteBuffers(1, &vbo_vertices);

		if (vbo_uvs != 0)
			glDeleteBuffers(1, &vbo_uvs);

		if (vbo_normals != 0)
			glDeleteBuffers(1, &vbo_normals);

		glDeleteVertexArrays(1, &vao);
	}

	//initialize vbos and vao, based on the informations of the mesh.
	inline void initGl()
	{
		triangleCount = triangleIndex.size() / 3;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		if (USE_INDEX & vbo_usage)
		{
			glGenBuffers(1, &vbo_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndex.size()*sizeof(int), &triangleIndex[0], GL_STATIC_DRAW);
		}

		if (USE_VERTICES & vbo_usage)
		{
			glGenBuffers(1, &vbo_vertices);
			glEnableVertexAttribArray(VERTICES);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(VERTICES, coordCountByVertex, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * coordCountByVertex, (void*)0);
		}

		if (USE_NORMALS & vbo_usage)
		{
			glGenBuffers(1, &vbo_normals);
			glEnableVertexAttribArray(NORMALS);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
			glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
		}

		if (USE_UVS & vbo_usage)
		{
			glGenBuffers(1, &vbo_uvs);
			glEnableVertexAttribArray(UVS);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
			glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), &uvs[0], GL_STATIC_DRAW);
			glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// simply draw the vertices, using vao.
	inline void draw()
	{
		glBindVertexArray(vao);
		if (USE_INDEX & vbo_usage)
			glDrawElements(primitiveType, triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0);
		else
			glDrawArrays(primitiveType, 0, vertices.size());
		glBindVertexArray(0);
	}
};

class Application {

private:
	int windowWidth;
	int windowHeight;

public : 
	inline void setWindowHeight(int height)
	{
		windowHeight = height;
	}

	inline void setWindowWidth(int width)
	{
		windowWidth = width;
	}

	inline int getWindowHeight()
	{
		return windowHeight;
	}

	inline int getWindowWidth()
	{
		return windowWidth;
	}


// singleton implementation :
private:
	Application() {

	}

public:
	inline static Application& get()
	{
		static Application instance;

		return instance;
	}

	
	Application(const Application& other) = delete;
	void operator=(const Application& other) = delete;

};

struct MeshRenderer
{
	Mesh* mesh;
	Material* material;

	MeshRenderer(Mesh* _mesh = nullptr, Material* _material = nullptr) : mesh(_mesh), material(_material)
	{

	}
};

struct Collider
{
	//TODO
};

struct Entity
{
	glm::mat4 modelMatrix;
	MeshRenderer* meshRenderer;
	Collider* collider;

	Entity() : modelMatrix(glm::mat4(1)), meshRenderer(nullptr), collider(nullptr)
	{

	}
};


class Renderer
{
private :
	GLuint glProgram_gPass;
	GLuint glProgram_lightPass;

	GLuint uniformTexturePosition;
	GLuint uniformTextureNormal;
	GLuint uniformTextureDepth;
	GLuint unformScreenToWorld;
	GLuint uniformCameraPosition;

	Mesh quadMesh;

	//light system
	LightManager* lightManager;
	
	//frame buffer for deferred lighting
	GLuint gbufferFbo;
	GLuint gbufferTextures[3];

	//for blit pass 
	GLuint glProgram_blit;
	GLuint uniformTextureBlit;

public : 
	Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path) : quadMesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES), 2)
	{

		int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();

		////////////////////// INIT QUAD MESH ////////////////////////
		quadMesh.triangleIndex = { 0, 1, 2, 2, 1, 3 };
		quadMesh.vertices = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
		quadMesh.initGl();


		//////////////////// 3D lightPass shaders ////////////////////////
		// Try to load and compile shaders
		GLuint vertShaderId_lightPass = compile_shader_from_file(GL_VERTEX_SHADER, programLightPass_vert_path.c_str());
		GLuint fragShaderId_lightPass = compile_shader_from_file(GL_FRAGMENT_SHADER, programLightPass_frag_path.c_str());

		glProgram_lightPass = glCreateProgram();
		glAttachShader(glProgram_lightPass, vertShaderId_lightPass);
		glAttachShader(glProgram_lightPass, fragShaderId_lightPass);

		glLinkProgram(glProgram_lightPass);
		if (check_link_error(glProgram_lightPass) < 0)
			exit(1);

		uniformTexturePosition = glGetUniformLocation(glProgram_lightPass, "ColorBuffer");
		uniformTextureNormal = glGetUniformLocation(glProgram_lightPass, "NormalBuffer");
		uniformTextureDepth = glGetUniformLocation(glProgram_lightPass, "DepthBuffer");
		unformScreenToWorld = glGetUniformLocation(glProgram_lightPass, "ScreenToWorld");
		uniformCameraPosition = glGetUniformLocation(glProgram_lightPass, "CameraPosition");

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);

		//////////////////// 3D Gpass shaders ////////////////////////
		// Try to load and compile shaders
		GLuint vertShaderId_gpass = compile_shader_from_file(GL_VERTEX_SHADER, programGPass_vert_path.c_str());
		GLuint fragShaderId_gpass = compile_shader_from_file(GL_FRAGMENT_SHADER, programGPass_frag_path.c_str());

		glProgram_gPass = glCreateProgram();
		glAttachShader(glProgram_gPass, vertShaderId_gpass);
		glAttachShader(glProgram_gPass, fragShaderId_gpass);

		glLinkProgram(glProgram_gPass);
		if (check_link_error(glProgram_gPass) < 0)
			exit(1);

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);

		//////////////////// INITIALIZE FRAME BUFFER ///////////////////

		glGenTextures(3, gbufferTextures);
		// 2 draw buffers for color and normal
		GLuint gbufferDrawBuffers[2];

		// Create color texture
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create normal texture
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,width, height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create depth texture
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create Framebuffer Object
		glGenFramebuffers(1, &gbufferFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
		// Initialize DrawBuffers
		gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
		gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
		glDrawBuffers(2, gbufferDrawBuffers);

		// Attach textures to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbufferTextures[0], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbufferTextures[1], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			fprintf(stderr, "Error on building framebuffer\n");
			exit(EXIT_FAILURE);
		}

		// Back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		////////////////////// LIGHT MANAGER /////////////////////////
		lightManager = _lightManager;
		lightManager->init(glProgram_lightPass);

	}

	void initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path)
	{
		//////////////////// BLIT shaders ////////////////////////
		GLuint vertShaderId_blit = compile_shader_from_file(GL_VERTEX_SHADER, programBlit_vert_path.c_str());
		GLuint fragShaderId_blit = compile_shader_from_file(GL_FRAGMENT_SHADER, programBlit_frag_path.c_str());

		glProgram_blit = glCreateProgram();
		glAttachShader(glProgram_blit, vertShaderId_blit);
		glAttachShader(glProgram_blit, fragShaderId_blit);

		glLinkProgram(glProgram_blit);
		if (check_link_error(glProgram_blit) < 0)
			exit(1);

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);

		// Upload uniforms
		uniformTextureBlit = glGetUniformLocation(glProgram_blit, "Texture");
	}

	void render(Camera& camera, std::vector<Entity*> entities)
	{

		int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();


		////////////////////////// begin scene rendering 

		// Viewport 
		glViewport(0, 0, width, height);

		// Clear default buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////////// begin deferred

		///////////// begin draw world

		////// begin matrix updates

		// update values
		glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
		glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
		glm::mat4 vp = projection * worldToView;
		glm::mat4 screenToWorld = glm::transpose(glm::inverse(vp));
		
		///// end matrix updates

		////// begin G pass 

		glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);

		// Default states
		glEnable(GL_DEPTH_TEST);

		// Clear the front buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < entities.size(); i++)
		{
			glm::mat4 modelMatrix = entities[i]->modelMatrix; //get modelMatrix
			glm::mat4 mv = worldToView * modelMatrix;
			glm::mat4 normalMatrix = glm::transpose(glm::inverse(mv));
			glm::mat4 mvp = projection * worldToView * modelMatrix;

			entities[i]->meshRenderer->material->use();
			entities[i]->meshRenderer->material->setUniform_MVP(mvp);
			entities[i]->meshRenderer->material->setUniform_normalMatrix(normalMatrix);

			entities[i]->meshRenderer->mesh->draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		////// end G pass

		///// begin light pass

		glUseProgram(glProgram_lightPass);

		// send screen to world matrix : 
		glUniformMatrix4fv(unformScreenToWorld, 1, false, glm::value_ptr(screenToWorld));
		glUniform3fv(uniformCameraPosition, 1, glm::value_ptr(camera.eye));

		//for lighting : 
		lightManager->renderLights();

		//geometry informations :
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);

		glUniform1i(uniformTexturePosition, 0);
		glUniform1i(uniformTextureNormal, 1);
		glUniform1i(uniformTextureDepth, 2);

		// Render quad
		quadMesh.draw();

		///// end light pass

		///////////// end draw world

		///////////// begin draw blit quad
		glDisable(GL_DEPTH_TEST);

		glUseProgram(glProgram_blit);

		for (int i = 0; i < 3; i++)
		{
			glViewport((width * i) / 3, 0, width / 3, height / 4);

			glActiveTexture(GL_TEXTURE0);
			// Bind gbuffer color texture
			glBindTexture(GL_TEXTURE_2D, gbufferTextures[i]);
			glUniform1i(uniformTextureBlit, 0);

			quadMesh.draw();
		}

		///////////// end draw blit quad

		////////////////// end deferred

		//////////////////////// end scene rendering


	}
};




struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;
void init_gui_states(GUIStates & guiStates);


int main( int argc, char **argv )
{
    int width = 1024, height= 768;
    float widthf = (float) width, heightf = (float) height;
    double t;
    float fps = 0.f;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    int const DPI = 1;
# endif

    // Open a window and create its OpenGL context
    GLFWwindow * window = glfwCreateWindow(width/DPI, height/DPI, "aogl", 0, 0);
    if( ! window )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    }
    glfwMakeContextCurrent(window);

    // Init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
          /* Problem: glewInit failed, something is seriously wrong. */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
          exit( EXIT_FAILURE );
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    ImGui_ImplGlfwGL3_Init(window, true);

    // Init viewer structures
    Camera camera;
    camera_defaults(camera);
    GUIStates guiStates;
    init_gui_states(guiStates);


	///////////////////// SET APPLICATION GLOBAL PARAMETERS /////////////////////
	Application::get().setWindowWidth(width);
	Application::get().setWindowHeight(height);

	//////////////////// 3D Gpass shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_gpass = compile_shader_from_file(GL_VERTEX_SHADER, "aogl.vert");
	GLuint fragShaderId_gpass = compile_shader_from_file(GL_FRAGMENT_SHADER, "aogl_gPass.frag");

	GLuint programObject_gPass = glCreateProgram();
	glAttachShader(programObject_gPass, vertShaderId_gpass);
	glAttachShader(programObject_gPass, fragShaderId_gpass);

	glLinkProgram(programObject_gPass);
	if (check_link_error(programObject_gPass) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	/*
	//////////////////// BLIT shaders ////////////////////////
	GLuint vertShaderId_blit = compile_shader_from_file(GL_VERTEX_SHADER, "blit.vert");
	GLuint fragShaderId_blit = compile_shader_from_file(GL_FRAGMENT_SHADER, "blit.frag");

	GLuint programObject_blit = glCreateProgram();
	glAttachShader(programObject_blit, vertShaderId_blit);
	glAttachShader(programObject_blit, fragShaderId_blit);

	glLinkProgram(programObject_blit);
	if (check_link_error(programObject_blit) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	// Upload uniforms
	GLuint textureLocation_blit = glGetUniformLocation(programObject_blit, "Texture");


	////////////////////////// LOAD GEOMETRY : 

	//blit quad : 
	int   quad_triangleCount = 2;
	int   quad_triangleList[] = { 0, 1, 2, 2, 1, 3 };
	float quad_vertices[] = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };

	// Bind vertices and upload data
	GLuint blitVertices;
	glGenBuffers(1, &blitVertices);
	glBindBuffer(GL_ARRAY_BUFFER, blitVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bind indices and upload data
	GLuint blitIndices;
	glGenBuffers(1, &blitIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blitIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//vao definition
	GLuint blitVAO;
	glGenVertexArrays(1, &blitVAO);
	glBindVertexArray(blitVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blitIndices);

		glBindBuffer(GL_ARRAY_BUFFER, blitVertices);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	*/

	// cube and plane ;

	Mesh cube;
	cube.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cube.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cube.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cube.normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cube.initGl();

	Mesh plane;
	plane.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	plane.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f };
	plane.vertices = { -5.0, -0.5, 5.0, 5.0, -0.5, 5.0, -5.0, -0.5, -5.0, 5.0, -0.5, -5.0 };
	plane.normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane.initGl();

    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    GLuint diffuseTexture;
    glGenTextures(1, &diffuseTexture);

    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned char * specular = stbi_load("textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 3);
	GLuint specularTexture;
	glGenTextures(1, &specularTexture);

    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, specular);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);


	//Create our brick material 
	Material brickMaterial(programObject_gPass, diffuseTexture, specularTexture, 50);

	//create and initialize our light manager
	LightManager lightManager;
	//lightManager.init(programObject_lightPass);

	//add lights
    //lightManager.addSpotLight(SpotLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1.f, 0), glm::vec3(0, -1.f, 0), glm::radians(20.f) ));
	//lightManager.addPointLight(PointLight(10, glm::vec3(1, 0, 0), glm::vec3(6.f, 1.f, 0)));
	//lightManager.addPointLight(PointLight(10, glm::vec3(0, 1, 0), glm::vec3(0.f, 1.f, 6.f)));
	//lightManager.addDirectionalLight(DirectionalLight(10, glm::vec3(0, 0, 1), glm::vec3(0.f, -1.f, -1.f)));

	int r = 5;
	float omega = 0;
	for (int i = 0; i < 100; i++)
	{
		lightManager.addPointLight(PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3( r*std::cosf(omega), 2.f, r*std::sinf(omega))));
		omega += 0.4f;

		if(i % 10 == 0)
			r += 5;
	}

	// renderer
	Renderer renderer(&lightManager, "aogl.vert", "aogl_gPass.frag", "aogl_lightPass.vert", "aogl_lightPass.frag"); // call lightManager.init()
	renderer.initPostProcessQuad("blit.vert", "blit.frag");

	//populate world
	MeshRenderer cubeRenderer;
	cubeRenderer.mesh = &cube;
	cubeRenderer.material = &brickMaterial;

	MeshRenderer planeRenderer(&plane, &brickMaterial);

	Entity entity_cube;
	entity_cube.meshRenderer = &cubeRenderer;
	Entity entity_plane;
	entity_plane.meshRenderer = &planeRenderer;

	entity_plane.modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30,1,30));

	std::vector<Entity*> entities = {&entity_cube, &entity_plane};

	//main loop
    do
    {
        t = glfwGetTime();
        ImGui_ImplGlfwGL3_NewFrame();

        // Mouse states
        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements
        int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            double x; double y;
            glfwGetCursorPos(window, &x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS)
        {
            double mousex; double mousey;
            glfwGetCursorPos(window, &mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera_pan(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                            diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }

		/*
		////////////////////////// begin scene rendering 

		// Viewport 
		glViewport(0, 0, width, height);

		// Clear default buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////////////////// begin deferred

		///////////// begin draw world

		////// begin matrix updates

		// update values
		glm::mat4 projection = glm::perspective(45.0f, widthf / heightf, 0.1f, 1000.f);
		glm::mat4 worldToView = glm::lookAt(camera.eye, camera.o, camera.up);
		glm::mat4 objectToWorld;
		glm::mat4 mvp = projection * worldToView * objectToWorld;
		glm::mat4 screenToWorld = glm::transpose(glm::inverse(mvp));

		///// end matrix updates

		////// begin G pass 

		glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);

		// Default states
		glEnable(GL_DEPTH_TEST);

		// Clear the front buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Select shader
		brickMaterial.use();

		// Upload uniforms
		brickMaterial.setUniform_MVP(mvp);
		brickMaterial.setUniform_cameraPosition(camera.eye);

		// Render vaos
		glBindVertexArray(vao);
		glDrawElementsInstanced(GL_TRIANGLES, cube_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 1);

		//update values
		objectToWorld = glm::scale(glm::mat4(1), glm::vec3(5, 1, 5));
		mvp = projection * worldToView * objectToWorld;

		// Upload uniforms
		brickMaterial.setUniform_MVP(mvp);

		// Render vaos
		glBindVertexArray(vao2);
		glDrawElementsInstanced(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0, 4);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		////// end G pass

		///// begin light pass

		glUseProgram(programObject_lightPass);

		// send screen to world matrix : 
		glUniformMatrix4fv(unformScreenToWorld, 1, false, glm::value_ptr(screenToWorld));
		glUniform3fv(uniformCameraPosition, 1, glm::value_ptr(camera.eye));

		//for lighting : 
		lightManager.renderLights();
		
		//geometry informations :
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);

		glUniform1i(uniformTexturePosition, 0);
		glUniform1i(uniformTextureNormal, 1);
		glUniform1i(uniformTextureDepth, 2);
        
		// Render quad
		glBindVertexArray(blitVAO);
		glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);

		///// end light pass

		///////////// end draw world


		///////////// begin draw blit quad
		glDisable(GL_DEPTH_TEST);

		glUseProgram(programObject_blit);

		for (int i = 0; i < 3; i++)
		{
			glViewport((width * i) / 3, 0, width / 3, height / 4);

			glActiveTexture(GL_TEXTURE0);
			// Bind gbuffer color texture
			glBindTexture(GL_TEXTURE_2D, gbufferTextures[i]);
			glUniform1i(textureLocation_blit, 0);

			// Bind quad VAO
			glBindVertexArray(blitVAO);
			
			// Draw quad
			glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
		}

		///////////// end draw blit quad

		////////////////// end deferred

		//////////////////////// end scene rendering
		*/

		renderer.render(camera, entities);

#if 1

        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("aogl");
        ImGui::SliderFloat("Material Specular Power", &(brickMaterial.specularPower), 0.0f, 100.f);
        lightManager.drawUI();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();



        glDisable(GL_BLEND);
#endif


        // Check for errors
        checkError("End loop");

        glfwSwapBuffers(window);
        glfwPollEvents();

        double newTime = glfwGetTime();
        fps = 1.f/ (newTime - t);
    } // Check if the ESC key was pressed
    while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS );

    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    exit( EXIT_SUCCESS );
}

// No windows implementation of strsep
char * strsep_custom(char **stringp, const char *delim)
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s; ; ) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    return 0;
}

int check_compile_error(GLuint shader, const char ** sourceBuffer)
{
    // Get error log size and print it eventually
    int logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        char *token, *string;
        string = strdup(sourceBuffer[0]);
        int lc = 0;
        while ((token = strsep_custom(&string, "\n")) != NULL) {
           printf("%3d : %s\n", lc, token);
           ++lc;
        }
        fprintf(stderr, "Compile : %s", log);
        delete[] log;
    }
    // If an error happend quit
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        return -1;     
    return 0;
}

int check_link_error(GLuint program)
{
    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1)
    {
        char * log = new char[logLength];
        glGetProgramInfoLog(program, logLength, &logLength, log);
        fprintf(stderr, "Link : %s \n", log);
        delete[] log;
    }
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);        
    if (status == GL_FALSE)
        return -1;
    return 0;
}


GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize)
{
    GLuint shaderObject = glCreateShader(shaderType);
    const char * sc[1] = { sourceBuffer };
    glShaderSource(shaderObject, 
                   1, 
                   sc,
                   NULL);
    glCompileShader(shaderObject);
    check_compile_error(shaderObject, sc);
    return shaderObject;
}

GLuint compile_shader_from_file(GLenum shaderType, const char * path)
{
    FILE * shaderFileDesc = fopen( path, "rb" );
    if (!shaderFileDesc)
        return 0;
    fseek ( shaderFileDesc , 0 , SEEK_END );
    long fileSize = ftell ( shaderFileDesc );
    rewind ( shaderFileDesc );
    char * buffer = new char[fileSize + 1];
    fread( buffer, 1, fileSize, shaderFileDesc );
    buffer[fileSize] = '\0';
    GLuint shaderObject = compile_shader(shaderType, buffer, fileSize );
    delete[] buffer;
    return shaderObject;
}


bool checkError(const char* title)
{
    int error;
    if((error = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch(error)
        {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        default:
            errorString = "UNKNOWN";
            break;
        }
        fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
    }
    return error == GL_NO_ERROR;
}

void camera_compute(Camera & c)
{
    c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;   
    c.eye.y = cos(c.phi) * c.radius + c.o.y ;
    c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;   
    c.up = glm::vec3(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
}

void camera_defaults(Camera & c)
{
    c.phi = 3.14/2.f;
    c.theta = 3.14/2.f;
    c.radius = 10.f;
    camera_compute(c);
}

void camera_zoom(Camera & c, float factor)
{
    c.radius += factor * c.radius ;
    if (c.radius < 0.1)
    {
        c.radius = 10.f;
        c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
    }
    camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
    c.theta += 1.f * theta;
    c.phi   -= 1.f * phi;
    if (c.phi >= (2 * M_PI) - 0.1 )
        c.phi = 0.00001;
    else if (c.phi <= 0 )
        c.phi = 2 * M_PI - 0.1;
    camera_compute(c);
}

void camera_pan(Camera & c, float x, float y)
{
    glm::vec3 up(0.f, c.phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(c.o - c.eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    c.up = glm::normalize(glm::cross(side, fwd));
    c.o[0] += up[0] * y * c.radius * 2;
    c.o[1] += up[1] * y * c.radius * 2;
    c.o[2] += up[2] * y * c.radius * 2;
    c.o[0] -= side[0] * x * c.radius * 2;
    c.o[1] -= side[1] * x * c.radius * 2;
    c.o[2] -= side[2] * x * c.radius * 2;       
    camera_compute(c);
}

void init_gui_states(GUIStates & guiStates)
{
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}
