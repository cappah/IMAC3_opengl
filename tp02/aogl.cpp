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

	//////////////////// INPUT HANDLER ///////////////////////////
	InputHandler inputHandler;

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
	

	// cube and plane ;

	Mesh cube;
	cube.triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cube.uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cube.vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cube.normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
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


	//create and initialize our light manager
	LightManager lightManager;
	//lightManager.init(programObject_lightPass); // done in renderer

	// renderer
	Renderer renderer(&lightManager, "aogl.vert", "aogl_gPass.frag", "aogl_lightPass.vert", "aogl_lightPass.frag"); // call lightManager.init()
	renderer.initPostProcessQuad("blit.vert", "blit.frag");

	//Our scene
	Scene scene(&renderer);

	//populate world

	//add lights
    //lightManager.addSpotLight(SpotLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1.f, 0), glm::vec3(0, -1.f, 0), glm::radians(20.f) ));
	//lightManager.addPointLight(PointLight(10, glm::vec3(1, 0, 0), glm::vec3(6.f, 1.f, 0)));
	//lightManager.addPointLight(PointLight(10, glm::vec3(0, 1, 0), glm::vec3(0.f, 1.f, 6.f)));
	//lightManager.addDirectionalLight(DirectionalLight(10, glm::vec3(0, 0, 1), glm::vec3(0.f, -1.f, -1.f)));

	//materials : 
	MaterialLit brickMaterial(programObject_gPass, diffuseTexture, specularTexture, 50);
	MaterialUnlit wireframeMaterial(programObject_wireframe);

	MeshRenderer cubeWireFrameRenderer;
	cubeWireFrameRenderer.mesh = &cubeWireFrame;
	cubeWireFrameRenderer.material = &wireframeMaterial;

	int r = 5;
	float omega = 0;
	for (int i = 0; i < 100; i++)
	{
		//lightManager.addPointLight(PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3( r*std::cosf(omega), 2.f, r*std::sinf(omega))));
		Entity* newEntity = new Entity(&scene);
		BoxCollider* boxColliderLight = new BoxCollider(&cubeWireFrameRenderer);
		PointLight* pointLight = new PointLight(10, glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f), glm::vec3(0,0,0));
		newEntity->add(boxColliderLight).add(pointLight);
		newEntity->setTranslation(glm::vec3(r*std::cosf(omega), 2.f, r*std::sinf(omega)));
		
		scene.add(newEntity);

		omega += 0.4f;

		if(i % 10 == 0)
			r += 5;
	}


	//renderers : 
	MeshRenderer cubeRenderer01;
	cubeRenderer01.mesh = &cube;
	cubeRenderer01.material = &brickMaterial;

	MeshRenderer cubeRenderer02;
	cubeRenderer02.mesh = &cube;
	cubeRenderer02.material = &brickMaterial;

	MeshRenderer planeRenderer(&plane, &brickMaterial);


	//colliders : 
    BoxCollider boxCollider01(&cubeWireFrameRenderer);
	BoxCollider boxCollider02(&cubeWireFrameRenderer);

	//entities : 
	//cube entity 01
	Entity* entity_cube01 = new Entity(&scene);
	entity_cube01->add(&cubeRenderer01);
	entity_cube01->add(&boxCollider01);
	entity_cube01->setTranslation( glm::vec3(4, 0, 0) );
	//cube entity 02
	Entity* entity_cube02 = new Entity(&scene);
	entity_cube02->add(&cubeRenderer02);
	entity_cube02->add(&boxCollider02);
	entity_cube02->setTranslation( glm::vec3(0, 0, 4) );

	//plane entity
	Entity* entity_plane = new Entity(&scene);
	entity_plane->add(&planeRenderer);
	entity_plane->setScale( glm::vec3(30,1,30) ); //scale plane

	//std::vector<Entity*> entities = {&entity_cube01, &entity_cube02, &entity_plane};

	//editor : 
	Editor editor(&wireframeMaterial);

	//main loop
    do
    {
        t = glfwGetTime();
        ImGui_ImplGlfwGL3_NewFrame();

        // Mouse states
        int leftButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE );

		int altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
		int shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS || (leftButton == GLFW_PRESS && altPressed) )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // Camera movements

        if (!altPressed && (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))
        {
            double x; double y;
            glfwGetCursorPos(window, &x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
        if (altPressed == GLFW_PRESS || shiftPressed == GLFW_PRESS)
        {
            double mousex; double mousey;
            glfwGetCursorPos(window, &mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;

			if (altPressed == GLFW_PRESS && shiftPressed == GLFW_RELEASE)
			{
				if (guiStates.zoomLock)
				{
					float zoomDir = 0.0;
					if (diffLockPositionX > 0)
						zoomDir = -1.f;
					else if (diffLockPositionX < 0)
						zoomDir = 1.f;
					camera_zoom(camera, zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
				}
				else if (guiStates.turnLock)
				{
					camera_turn(camera, diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
						diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

				}
			}
			if (altPressed == GLFW_PRESS && shiftPressed == GLFW_PRESS)
			{
				if (guiStates.panLock)
				{
					camera_pan(camera, diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
						diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
				}
			}

            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }

		//object picking : 
		if (inputHandler.getMouseButtonDown(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			glm::vec3 origin = camera.eye;
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			glm::vec3 direction = screenToWorld(mouseX, mouseY, width, height, camera);
			//direction = direction - origin;
			//direction = glm::normalize(direction);

			Ray ray(origin, direction, 1000.f);

			if (editor.testGizmoIntersection(ray))
			{
				editor.beginMoveGizmo();
			}

			auto entities = scene.getEntities();
			for (int i = 0; i < entities.size(); i++)
			{
				Collider* collider = static_cast<Collider*>( entities[i]->getComponent(Component::ComponentType::COLLIDER) );
				if (entities[i]->getComponent(Component::ComponentType::COLLIDER) != nullptr)
				{
					if (ray.intersect(*collider))
					{
						editor.changeCurrentSelected(entities[i]);
						std::cout << "intersect a cube !!!" << std::endl;

						ray.debugLog();
					}
				}
			}
		}
		else if (inputHandler.getMouseButtonUp(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			if(editor.isMovingGizmo())
				editor.endMoveGizmo();
		}
		if (inputHandler.getMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		{
			if (editor.isMovingGizmo())
			{
				glm::vec3 origin = camera.eye;
				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				glm::vec3 direction = screenToWorld(mouseX, mouseY, width, height, camera);
				Ray ray(origin, direction, 1000.f);

				editor.moveGizmo(ray);
			}
		}


		//synchronize input handler : 
		inputHandler.synchronize(window);
		
		//rendering : 
		//renderer.render(camera, entities);
		scene.render(camera);
		scene.renderColliders(camera);
		scene.renderDebugDeferred();

		glDisable(GL_DEPTH_TEST);
		editor.renderGizmo(camera);
		

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
		editor.renderUI();

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
