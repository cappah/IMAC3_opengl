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

#include "InputHandler.h"
#include "Camera.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Materials.h"
#include "LightManager.h"
#include "Application.h"
#include "Collider.h"
#include "Utils.h"
#include "Entity.h"
#include "Editor.h"
#include "Lights.h"
#include "Editor.h"
#include "Ray.h"
#include "Renderer.h"
#include "Scene.h"
#include "Factories.h"
#include "Flag.h"
#include "Scene.h"

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




void window_size_callback(GLFWwindow* window, int width, int height)
{
	Application::get().setWindowResize(true);
	Application::get().setWindowWidth(width);
	Application::get().setWindowHeight(height);
}




int main( int argc, char **argv )
{
    //int width = 1024, height= 768;
	int width = 1024, height = 680;
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
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
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

	//set the resize window callback 
	glfwSetWindowSizeCallback(window, window_size_callback);


    // Init viewer structures
    //Camera camera;
    //camera_defaults(camera);
    //GUIStates guiStates;
    //init_gui_states(guiStates);


	///////////////////// SET APPLICATION GLOBAL PARAMETERS /////////////////////
	Application::get().setWindowWidth(width);
	Application::get().setWindowHeight(height);

	//////////////////// INPUT HANDLER ///////////////////////////
	InputHandler inputHandler;

	//////////////////// SKYBOX shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_skybox = compile_shader_from_file(GL_VERTEX_SHADER, "skybox.vert");
	GLuint fragShaderId_skybox = compile_shader_from_file(GL_FRAGMENT_SHADER, "skybox.frag");

	GLuint programObject_skybox = glCreateProgram();
	glAttachShader(programObject_skybox, vertShaderId_skybox);
	glAttachShader(programObject_skybox, fragShaderId_skybox);

	glLinkProgram(programObject_skybox);
	if (check_link_error(programObject_skybox) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

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

	//////////////////// WIREFRAME shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_wireframe = compile_shader_from_file(GL_VERTEX_SHADER, "wireframe.vert");
	GLuint fragShaderId_wireframe = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframe.frag");

	GLuint programObject_wireframe = glCreateProgram();
	glAttachShader(programObject_wireframe, vertShaderId_wireframe);
	glAttachShader(programObject_wireframe, fragShaderId_wireframe);

	glLinkProgram(programObject_wireframe);
	if (check_link_error(programObject_wireframe) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// TERRAIN shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_terrain = compile_shader_from_file(GL_VERTEX_SHADER, "terrainEdition.vert");
	GLuint fragShaderId_terrain = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrainEdition.frag");

	GLuint programObject_terrain = glCreateProgram();
	glAttachShader(programObject_terrain, vertShaderId_terrain);
	glAttachShader(programObject_terrain, fragShaderId_terrain);

	glLinkProgram(programObject_terrain);
	if (check_link_error(programObject_terrain) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
	

	// cube and plane ;

	//Mesh cube;
	//cube.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	//cube.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	//cube.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
	//				-0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5,
	//				-0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5,
	//				-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 
	//				0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 
	//				0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5,
	//				-0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	//cube.normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
	//				0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
	//				0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
	//				0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,
	//				1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
	//				1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
	//				-1, 0, 0, -1, 0, 0, -1, 0, 0, };
	//cube.initGl();

	Mesh cube(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_TANGENTS));
	cube.vertices = { 0.5,0.5,-0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,
				-0.5,0.5,-0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  -0.5,-0.5,-0.5,
				-0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,
				-0.5,0.5,-0.5,  0.5,0.5,-0.5,  0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,
				0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5,
				-0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5 };

	cube.normals = { 1,0,0,  1,0,0,  1,0,0,  1,0,0,
				-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
				0,0,1,  0,0,1,  0,0,1,  0,0,1,
				0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,
				0,1,0,  0,1,0,  0,1,0,  0,1,0,
				0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0 };

	cube.tangents = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,
				0,0,1,  0,0,1,  0,0,1,  0,0,1,
				-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
				1,0,0,  1,0,0,  1,0,0,  1,0,0,
				1,0,0,  1,0,0,  1,0,0,  1,0,0,
				-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0 };

	cube.uvs = { 0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
				0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
				0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
				0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
				0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
				0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0 };

	cube.triangleIndex = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };

	cube.initGl();


	Mesh cubeWireFrame(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
	cubeWireFrame.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cubeWireFrame.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cubeWireFrame.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cubeWireFrame.normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cubeWireFrame.initGl();

	Mesh plane;
	plane.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	plane.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f };
	plane.vertices = { -5.0, -0.5, 5.0, 5.0, -0.5, 5.0, -5.0, -0.5, -5.0, 5.0, -0.5, -5.0 };
	plane.normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane.initGl();

	/*
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
	*/

	Texture* diffuseTexture = new Texture("textures/spnza_bricks_a_diff.tga");
	Texture* specularTexture = new Texture("textures/spnza_bricks_a_spec.tga");
	Texture* bumpTexture = new Texture("textures/spnza_bricks_a_normal.png");

	std::vector<std::string> skyboxTexturePaths = {"textures/skyboxes/right.png", "textures/skyboxes/left.png", 
												   "textures/skyboxes/top.png", "textures/skyboxes/top.png",
													"textures/skyboxes/front.png","textures/skyboxes/back.png" };

	CubeTexture* defaultSkybox = new CubeTexture(skyboxTexturePaths);

	//force texture initialisation
	diffuseTexture->initGL();
	specularTexture->initGL();
	bumpTexture->initGL();

	//////////////////// BEGIN RESSOURCES : 
	//the order between resource initialization and factories initialisation is important, indeed it's the factory which set set name of the different ressources when they are added to the factories.
	// So initialyzing materials before TextureFectory initialysation will create materials with wrong texture and mesh names. 

	//texture factories : 
	TextureFactory::get().add("brickDiffuse", diffuseTexture);
	TextureFactory::get().add("brickSpecular", specularTexture);
	TextureFactory::get().add("brickBump", bumpTexture);

	// materials : 
	MaterialLit defaultMaterial(programObject_gPass, TextureFactory::get().get("default") , TextureFactory::get().get("default"), TextureFactory::get().get("default"), 50);
	MaterialLit brickMaterial(programObject_gPass, diffuseTexture, specularTexture, bumpTexture, 50);
	MaterialUnlit wireframeMaterial(programObject_wireframe);

	//material factories : 
	MaterialFactory::get().add("default", &defaultMaterial);
	MaterialFactory::get().add("brick", &brickMaterial);
	MaterialFactory::get().add("wireframe", &wireframeMaterial);

	//mesh factories : 
	MeshFactory::get().add("cube", &cube);
	MeshFactory::get().add("cubeWireframe", &cubeWireFrame);
	MeshFactory::get().add("plane", &plane);

	CubeTextureFactory::get().add("plaineSkybox", defaultSkybox);

	////////// INITIALYZE DEFAULT MATERIAL IN FACTORY : 
	ProgramFactory::get().add("defaultLit", programObject_gPass);
	ProgramFactory::get().add("defaultUnlit", programObject_wireframe);
	ProgramFactory::get().add("defaultSkybox", programObject_skybox);
	ProgramFactory::get().add("defaultTerrain", programObject_terrain);

	///////////////////// END RESSOURCES 


	// create and initialize our light manager
	LightManager lightManager;
	//lightManager.init(programObject_lightPass); // done in renderer

	// renderer : 
	Renderer renderer(&lightManager, "aogl.vert", "aogl_gPass.frag", "aogl_lightPass.vert", "aogl_lightPass_pointLight.frag", "aogl_lightPass_directionalLight.frag", "aogl_lightPass_spotLight.frag"); // call lightManager.init()
	renderer.initPostProcessQuad("blit.vert", "blit.frag");
	renderer.initialyzeShadowMapping("shadowPass.vert", "shadowPass.frag", "shadowPassOmni.vert", "shadowPassOmni.frag", "shadowPassOmni.geom");

	// Our scene : 
	Scene scene(&renderer);

	// populate the scene :

	// mesh renderer for colliders : 
	MeshRenderer cubeWireFrameRenderer;
	cubeWireFrameRenderer.setMesh( &cubeWireFrame );
	cubeWireFrameRenderer.setMaterial( &wireframeMaterial );

	//int r = 5;
	//float omega = 0;
	//for (int i = 0; i < 100; i++)
	//{
	//	Entity* newEntity = new Entity(&scene);
	//	BoxCollider* boxColliderLight = new BoxCollider(&cubeWireFrameRenderer);
	//	PointLight* pointLight = new PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0,0,0));
	//	newEntity->add(boxColliderLight).add(pointLight);
	//	newEntity->setTranslation(glm::vec3(r*std::cosf(omega), 2.f, r*std::sinf(omega)));
	//	
	//	scene.add(newEntity);

	//	omega += 0.4f;

	//	if(i % 10 == 0)
	//		r += 5;
	//}

	// an entity with a light : 
	Entity* newEntity = new Entity(&scene);
	BoxCollider* boxColliderLight = new BoxCollider(&cubeWireFrameRenderer);
	//SpotLight* spotLight = new SpotLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
	PointLight* pointLight = new PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0, 0, 0));
	pointLight->setBoundingBoxVisual(new MeshRenderer(MeshFactory::get().get("cubeWireframe"), MaterialFactory::get().get<Material3DObject>("wireframe")));
	newEntity->add(boxColliderLight).add(pointLight);
	newEntity->setTranslation(glm::vec3(0, 1.5, 0));


	//renderers : 
	MeshRenderer* cubeRenderer01 = new MeshRenderer(&cube, &brickMaterial);
	MeshRenderer* cubeRenderer02 = new MeshRenderer(&cube, &brickMaterial);

	//MeshRenderer cubeRenderer02;
	//cubeRenderer02.mesh = &cube;
	//cubeRenderer02.material = &brickMaterial;

	MeshRenderer* planeRenderer = new MeshRenderer(&plane, &brickMaterial);

	//colliders : 
	BoxCollider* boxCollider01 = new BoxCollider(&cubeWireFrameRenderer);
	BoxCollider* boxCollider02 = new BoxCollider(&cubeWireFrameRenderer);

	//entities : 
	
	//cube entity 01
	Entity* entity_cube01 = new Entity(&scene);
	entity_cube01->add(cubeRenderer01);
	entity_cube01->add(boxCollider01);
	entity_cube01->setTranslation( glm::vec3(3, 0, 0) );
	//cube entity 02
	Entity* entity_cube02 = new Entity(&scene);
	entity_cube02->add(cubeRenderer02);
	entity_cube02->add(boxCollider02);
	entity_cube02->setTranslation(glm::vec3(3, -2, 0));
	entity_cube02->setScale(glm::vec3(10, 1, 10));
	/*
	Entity* entity_cube03= new Entity(&scene);
	entity_cube03->add(cubeRenderer03);
	entity_cube03->add(boxCollider03);
	entity_cube03->setScale(glm::vec3(10, 1, 10));
	entity_cube03->setTranslation(glm::vec3(4, 2, 0));
	entity_cube03->setRotation( glm::quat( glm::vec3(0, 0, -glm::pi<float>()*0.5f) ));
	//cube entity 04
	Entity* entity_cube04 = new Entity(&scene);
	entity_cube04->add(cubeRenderer04);
	entity_cube04->add(boxCollider04);
	entity_cube04->setTranslation(glm::vec3(0, -2, 0));
	entity_cube04->setScale(glm::vec3(10, 1, 10));
	entity_cube04->setTranslation(glm::vec3(0, 2, 4));
	entity_cube04->setRotation(glm::quat(glm::vec3(-glm::pi<float>()*0.5f, 0, 0)));
	*/

	//flage entity : 
	Material3DObject* tmpMat = MaterialFactory::get().get<Material3DObject>("default");
	Physic::Flag* flag = new Physic::Flag(tmpMat);

	Entity* entity_flag = new Entity(&scene);
	entity_flag->add(new BoxCollider(&cubeWireFrameRenderer));
	entity_flag->add(flag);
	entity_flag->endCreation();


	//editor : 
	Editor editor(&wireframeMaterial);

	float deltaTime = 0.f;

	//main loop
    do
    {
        t = glfwGetTime();
        ImGui_ImplGlfwGL3_NewFrame();

		//Physics : 
		scene.updatePhysic(deltaTime);

		//check if window has been resized by user
		if (Application::get().getWindowResize())
		{
			renderer.onResizeWindow();
			//TODO : 
			//editor.onResizeWindow();

			Application::get().setWindowResize(false);
		}


		//update editor : 
		editor.update(scene, window, inputHandler);


		//synchronize input handler : 
		inputHandler.synchronize(window);

		//get active camera before render scene : 
		Camera& currentCamera = editor.getCamera();
		
		//scene.culling(currentCamera);

		//rendering : 
		//renderer.render(camera, entities);
		scene.render(currentCamera);
		scene.renderColliders(currentCamera);
		scene.renderDebugDeferred();
		scene.renderDebugLights(currentCamera);

		glDisable(GL_DEPTH_TEST);
		editor.renderGizmo();
		

#if 1
		/*
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("aogl");
        ImGui::SliderFloat("Material Specular Power", &(brickMaterial.specularPower), 0.0f, 100.f);
        lightManager.drawUI();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
		*/

		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		editor.renderUI(scene);

        ImGui::Render();

        glDisable(GL_BLEND);
#endif


        // Check for errors
        checkError("End loop");

        glfwSwapBuffers(window);
        glfwPollEvents();

        double newTime = glfwGetTime();
		deltaTime = newTime - t;
        fps = 1.f/ (newTime - t);
    } // Check if the ESC key was pressed
    while( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS );

    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    exit( EXIT_SUCCESS );
}


