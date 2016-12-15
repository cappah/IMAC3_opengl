#include "stdafx.h"

#include "Renderer.h"

//forwards :
#include "Factories.h"
#include "EditorTools.h" 
#include "RenderBatch.h"
#include "BatchableWith.h"

Renderer::Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path_pointLight, std::string programLightPass_frag_path_directionalLight, std::string programLightPass_frag_path_spotLight)  
	: quadMesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES), 2)
{

	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();

	////////////////////// INIT QUAD MESH ////////////////////////
	quadMesh.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	quadMesh.vertices = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
	quadMesh.initGl();

	//////////////////// MAKE NEW LIGHTING MATERIALS ///////////////////

	m_pointLightMaterial = std::make_shared<MaterialPointLight>(*getProgramFactory().get("pointLight"));
	if (!checkError("uniforms"))
		PRINT_ERROR("");
	m_directionalLightMaterial = std::make_shared<MaterialDirectionalLight>(*getProgramFactory().get("directionalLight"));
	if (!checkError("uniforms"))
		PRINT_ERROR("");
	m_spotLightMaterial = std::make_shared<MaterialSpotLight>(*getProgramFactory().get("spotLight"));
	if (!checkError("uniforms"))
		PRINT_ERROR("");

	//////////////////// INITIALIZE G BUFFER ///////////////////

	GlHelper::makeColorTexture(gPassColorTexture, width, height);
	gPassColorTexture.initGL();
	GlHelper::makeNormalTexture(gPassNormalTexture, width, height);
	gPassNormalTexture.initGL();
	GlHelper::makeDepthTexture(gPassDepthTexture, width, height);
	gPassDepthTexture.initGL();
	GlHelper::makeFloatColorTexture(gPassHightValuesTexture, width, height);
	gPassHightValuesTexture.initGL();

	gBufferFBO.bind(GL_FRAMEBUFFER);
	GLenum drawBufferForGPass[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	gBufferFBO.setDrawBuffers(4, drawBufferForGPass);
	gBufferFBO.attachTexture(&gPassColorTexture, GL_COLOR_ATTACHMENT0, 0);
	gBufferFBO.attachTexture(&gPassNormalTexture, GL_COLOR_ATTACHMENT1, 0);
	gBufferFBO.attachTexture(&gPassHightValuesTexture, GL_COLOR_ATTACHMENT2, 0);
	gBufferFBO.attachTexture(&gPassDepthTexture, GL_DEPTH_ATTACHMENT, 0);

	gBufferFBO.checkIntegrity();
	gBufferFBO.unbind();

	////////////////////// LIGHT MANAGER /////////////////////////
	lightManager = _lightManager;
	lightManager->setLightingMaterials(m_pointLightMaterial, m_directionalLightMaterial, m_spotLightMaterial);
	lightManager->setShadowMapCount(LightManager::SPOT, 10);
	lightManager->setShadowMapCount(LightManager::DIRECTIONAL, 5);
	lightManager->setShadowMapCount(LightManager::POINT, 10);

	////////////////////// SETUP MAIN FRAMEBUFFER /////////////////////////

	m_lightPassHDRColor = GlHelper::makeNewFloatColorTexture(width, height);
	m_lightPassHDRColor->initGL();

	m_lightPassDepth = GlHelper::makeNewDepthTexture(width, height);
	m_lightPassDepth->initGL();

	m_lightPassHighValues = GlHelper::makeNewFloatColorTexture(width, height);
	m_lightPassHighValues->initGL();

	m_lightPassBuffer.bind();
	m_lightPassBuffer.attachTexture(m_lightPassHDRColor, GL_COLOR_ATTACHMENT0);
	m_lightPassBuffer.attachTexture(m_lightPassHighValues, GL_COLOR_ATTACHMENT1);
	m_lightPassBuffer.attachTexture(m_lightPassDepth, GlHelper::Framebuffer::AttachmentTypes::DEPTH);
	GLuint drawBuffers[] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
	m_lightPassBuffer.setDrawBuffers(2, drawBuffers);
	m_lightPassBuffer.checkIntegrity();
	m_lightPassBuffer.unbind();

	////////////////////// FINALLY STORE THE VIEWPORT SIZE /////////////////////////
	m_viewportRenderSize.x = width;
	m_viewportRenderSize.y = height;

	if (!checkError("uniforms"))
		PRINT_ERROR("");

}

Renderer::~Renderer()
{
	delete lightManager;
	delete m_lightPassHDRColor;
	delete m_lightPassDepth;
	delete m_lightPassHighValues;
}

Texture * Renderer::getFinalFrame() const
{
	return m_lightPassHDRColor;
}

const glm::vec2 & Renderer::getViewportRenderSize() const
{
	return m_viewportRenderSize;
}


void Renderer::onResizeViewport(const glm::vec2& newViewportSize)
{
	const int width = newViewportSize.x, height = newViewportSize.y;

	if (width < 1 || height < 1)
		return;

	////////////////////// RESIZE G BUFFER /////////////////////////

	gBufferFBO.bind(GL_FRAMEBUFFER);

	// Detach textures
	gBufferFBO.detachTexture(GL_COLOR_ATTACHMENT0, 0);
	gBufferFBO.detachTexture(GL_COLOR_ATTACHMENT1, 0);
	gBufferFBO.detachTexture(GL_COLOR_ATTACHMENT2, 0);
	gBufferFBO.detachTexture(GL_COLOR_ATTACHMENT3, 0);
	gBufferFBO.detachTexture(GL_DEPTH_ATTACHMENT, 0);

	// Pop, resize and repush textures
	gPassColorTexture.freeGL();
	GlHelper::makeColorTexture(gPassColorTexture, width, height);
	gPassColorTexture.initGL();

	gPassNormalTexture.freeGL();
	GlHelper::makeNormalTexture(gPassNormalTexture, width, height);
	gPassNormalTexture.initGL();

	gPassHightValuesTexture.freeGL();
	GlHelper::makeFloatColorTexture(gPassHightValuesTexture, width, height);
	gPassHightValuesTexture.initGL();

	gPassDepthTexture.freeGL();
	GlHelper::makeDepthTexture(gPassDepthTexture, width, height);
	gPassDepthTexture.initGL();

	// Attach texture again
	gBufferFBO.attachTexture(&gPassColorTexture, GL_COLOR_ATTACHMENT0, 0);
	gBufferFBO.attachTexture(&gPassNormalTexture, GL_COLOR_ATTACHMENT1, 0);
	gBufferFBO.attachTexture(&gPassHightValuesTexture, GL_COLOR_ATTACHMENT2, 0);
	gBufferFBO.attachTexture(&gPassDepthTexture, GL_DEPTH_ATTACHMENT, 0);

	gBufferFBO.checkIntegrity();
	gBufferFBO.unbind();

	////////////////////// RESIZE MAIN TEXTURE /////////////////////////

	delete m_lightPassHDRColor;
	delete m_lightPassDepth;
	delete m_lightPassHighValues;

	m_lightPassHDRColor = GlHelper::makeNewColorTexture(width, height);
	m_lightPassHDRColor->initGL();
	m_lightPassDepth = GlHelper::makeNewDepthTexture(width, height);
	m_lightPassDepth->initGL();
	m_lightPassHighValues = GlHelper::makeNewFloatColorTexture(width, height);
	m_lightPassHighValues->initGL();

	m_lightPassBuffer.bind();
	m_lightPassBuffer.attachTexture(m_lightPassHDRColor, GL_COLOR_ATTACHMENT0);
	m_lightPassBuffer.attachTexture(m_lightPassHighValues, GL_COLOR_ATTACHMENT1);
	m_lightPassBuffer.attachTexture(m_lightPassDepth, GlHelper::Framebuffer::AttachmentTypes::DEPTH);
	m_lightPassBuffer.checkIntegrity();
	m_lightPassBuffer.unbind();

	////////////////////// FINALLY STORE THE VIEWPORT SIZE /////////////////////////
	m_viewportRenderSize.x = width;
	m_viewportRenderSize.y = height;

	////////////////////// SETUP POST PROCESS MANAGER /////////////////////////
	m_postProcessManager.onViewportResized(m_viewportRenderSize.x, m_viewportRenderSize.y);
}


void Renderer::initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path)
{
	//////////////////// BLIT shaders ////////////////////////
	glProgram_blit = std::make_shared<MaterialBlit>(*getProgramFactory().get("blit"));
}

void Renderer::initialyzeShadowMapping(std::string progamShadowPass_vert_path, std::string progamShadowPass_frag_path,
										std::string progamShadowPassOmni_vert_path, std::string progamShadowPassOmni_frag_path, std::string progamShadowPassOmni_geom_path)
{
	//////////////////////// SHADOWS //////////////////////////

	//////////////////// shadow pass shader ////////////////////////
	shadowPassMaterial = std::make_shared<MaterialShadowPass>(*getProgramFactory().get("shadowPass"));

	//////////////////// omnidirectional shadow pass shader ////////////////////////
	shadowPassOmniMaterial = std::make_shared<MaterialShadowPassOmni>(*getProgramFactory().get("shadowPassOmni"));
	


	//glGenFramebuffers(1, &shadowFrameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	////initialyze shadowRenderBuffer : 
	//glGenRenderbuffers(1, &shadowRenderBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, 1024, 1024);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);

	////initialyze shadow texture : 
	//glGenTextures(1, &shadowTexture);
	//glBindTexture(GL_TEXTURE_2D, shadowTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	fprintf(stderr, "Error on building shadow framebuffer\n");
	//	exit(EXIT_FAILURE);
	//}

	//glBindTexture(GL_TEXTURE_2D, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::renderShadows(const glm::mat4& lightProjection, const glm::mat4& lightView, const IDrawable& drawable)
{
	glm::mat4 modelMatrix = drawable.getModelMatrix(); //get modelMatrix

	// From object to light (MV for light)
	glm::mat4 objectToLight = lightView * modelMatrix;
	// From object to shadow map screen space (MVP for light)
	glm::mat4 objectToLightScreen = lightProjection * objectToLight;
	// From world to shadow map screen space 
	glm::mat4 worldToLightScreen = lightProjection * lightView;

	//glUniformMatrix4fv(uniformShadowMVP, 1, false, glm::value_ptr(objectToLightScreen));
	shadowPassMaterial->setUniformMVP(objectToLightScreen);

	//draw mesh : 
	drawable.draw();

}

void Renderer::renderShadows(float farPlane, const glm::vec3 & lightPos, const std::vector<glm::mat4>& lightVPs, const IDrawable& drawable)
{
	//get modelMatrix
	glm::mat4 modelMatrix = drawable.getModelMatrix();

	for (int i = 0; i < 6; i++)
	{
		// the light_projection * light_view to transform vertices in light space in geometry shader
		//glUniformMatrix4fv(uniformShadowOmniVPLight[i], 1, false, glm::value_ptr(lightVPs[i]));  
		shadowPassOmniMaterial->setUniformVPLight(lightVPs[i], i);
	}
	// model matrix of the mesh
	//glUniformMatrix4fv(uniformShadowOmniModelMatrix, 1, false, glm::value_ptr(modelMatrix)); 
	//glUniform3fv(uniformShadowOmniLightPos, 1, glm::value_ptr(lightPos));
	//glUniform1f(uniformShadowOmniFarPlane, farPlane);
	shadowPassOmniMaterial->setUniformModelMatrix(modelMatrix);
	shadowPassOmniMaterial->setUniformLightPos(lightPos);
	shadowPassOmniMaterial->setUniformFarPlane(farPlane);

	//draw mesh : 
	drawable.draw();
}

//
//void Renderer::render(const BaseCamera& camera, std::vector<MeshRenderer*>& meshRenderers, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, Terrain& terrain, Skybox& skybox, std::vector<Physic::Flag*>& flags, std::vector<Billboard*>& billboards, std::vector<Physic::ParticleEmitter*>& particleEmitters, DebugDrawRenderer* debugDrawer)
//{
//	//int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
//	const int width = m_viewportRenderSize.x;
//	const int height = m_viewportRenderSize.y;
//	glm::vec3 cameraPosition = camera.getCameraPosition();
//	glm::vec3 cameraForward = camera.getCameraForward();
//
//	////////////////////////// begin scene rendering 
//
//
//	//////// begin shadow pass
//	glEnable(GL_DEPTH_TEST);
//
//	//culling for lights : 
//	updateCulling(camera, pointLights, spotLights, pointLightCullingInfos, spotLightCullingInfos);
//
//	//TODO : check if shadow map count and light count match
//
//	//for spot lights : 
//	//glUseProgram(glProgram_shadowPass);
//	shadowPassMaterial->use();
//	for (int shadowIdx = 0; shadowIdx < spotLightCount; shadowIdx++)
//	{
//		int lightIdx = spotLightCullingInfos[shadowIdx].idx;
//
//		if (shadowIdx < lightManager->getShadowMapCount(LightManager::SPOT))
//		{
//			lightManager->bindShadowMapFBO(LightManager::SPOT, shadowIdx);
//			glClear(GL_DEPTH_BUFFER_BIT);
//			glm::mat4 lightProjection = glm::perspective(spotLights[lightIdx]->angle*2.f, 1.f, 0.1f, 100.f);
//			glm::mat4 lightView = glm::lookAt(spotLights[lightIdx]->position, spotLights[lightIdx]->position + spotLights[lightIdx]->direction, spotLights[lightIdx]->up);
//
//			for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
//			{
//				renderShadows(lightProjection, lightView, *meshRenderers[meshIdx]);
//			}
//			lightManager->unbindShadowMapFBO(LightManager::SPOT);
//			glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		}
//	}
//
//	
//	//for directional lights : 
//	//glUseProgram(glProgram_shadowPass);
//	shadowPassMaterial->use();
//	for (int lightIdx = 0; lightIdx < directionalLights.size(); lightIdx++)
//	{
//		if (lightIdx < lightManager->getShadowMapCount(LightManager::DIRECTIONAL))
//		{
//			lightManager->bindShadowMapFBO(LightManager::DIRECTIONAL, lightIdx);
//			glClear(GL_DEPTH_BUFFER_BIT);
//			float directionalShadowMapRadius = lightManager->getDirectionalShadowMapViewportSize()*0.5f;
//			float directionalShadowMapNear = lightManager->getDirectionalShadowMapViewportNear();
//			float directionalShadowMapFar = lightManager->getDirectionalShadowMapViewportFar();
//			glm::vec3 orig = glm::vec3(cameraForward.x, 0, cameraForward.z)*directionalShadowMapRadius + glm::vec3(cameraPosition.x, directionalLights[lightIdx]->position.y /*directionalShadowMapFar*0.5f*/, cameraPosition.z);
//			glm::vec3 eye = -directionalLights[lightIdx]->direction + orig;
//			glm::mat4 lightProjection = glm::ortho(-directionalShadowMapRadius, directionalShadowMapRadius, -directionalShadowMapRadius, directionalShadowMapRadius, directionalShadowMapNear, directionalShadowMapFar);
//			glm::mat4 lightView = glm::lookAt(eye, orig, directionalLights[lightIdx]->up);
//
//			for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
//			{
//				renderShadows(lightProjection, lightView, *meshRenderers[meshIdx]);
//			}
//			lightManager->unbindShadowMapFBO(LightManager::DIRECTIONAL);
//			glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		}
//	}
//	
//
//	//for point lights : 
//	//glUseProgram(glProgram_shadowPassOmni);
//	shadowPassOmniMaterial->use();
//	for (int shadowIdx = 0; shadowIdx < pointLightCount; shadowIdx++)
//	{
//		int lightIdx = pointLightCullingInfos[shadowIdx].idx;
//
//		if (shadowIdx < lightManager->getShadowMapCount(LightManager::POINT))
//		{
//			lightManager->bindShadowMapFBO(LightManager::POINT, shadowIdx);
//			glClear(GL_DEPTH_BUFFER_BIT);
//			glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);
//			std::vector<glm::mat4> lightVPs;
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)) );
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
//			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)) );
//
//			for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
//			{
//				renderShadows(100.f, pointLights[lightIdx]->position, lightVPs, *meshRenderers[meshIdx]); // draw shadow for the first spot light
//			}
//			lightManager->unbindShadowMapFBO(LightManager::POINT);
//			glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		}
//	}
//	
//	CHECK_GL_ERROR("error in shadow pass");
//	//////// end shadow pass
//
//
//	// Viewport 
//	glViewport(0, 0, width, height);
//
//	// Clear default buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	///////////// begin draw world
//
//	//////// begin deferred
//
//	////// begin matrix updates
//
//	// update values
//	glm::mat4 projection = camera.getProjectionMatrix();//glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
//	glm::mat4 worldToView = camera.getViewMatrix();//glm::lookAt(camera.eye, camera.o, camera.up);
//	glm::mat4 camera_mvp = projection * worldToView;
//	glm::mat4 screenToWorld = glm::transpose(glm::inverse(camera_mvp));
//	
//	///// end matrix updates
//
//	////// begin G pass 
//
//	gBufferFBO.bind();
//
//	// Default states
//	glEnable(GL_DEPTH_TEST);
//
//	// Clear the front buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//render meshes
//	for (int i = 0; i < meshRenderers.size(); i++)
//	{
//		//glm::mat4 modelMatrix = meshRenderers[i]->entity()->getModelMatrix(); //get modelMatrix
//		//glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
//		//glm::mat4 mvp = projection * worldToView * modelMatrix;
//
//		//meshRenderers[i]->getMaterial()->use();
//		//meshRenderers[i]->getMaterial()->setUniform_MVP(mvp);
//		//meshRenderers[i]->getMaterial()->setUniform_normalMatrix(normalMatrix);
//
//		//meshRenderers[i]->getMesh()->draw();
//		meshRenderers[i]->render(projection, worldToView);
//	}
//	CHECK_GL_ERROR("error when rendering meshes");
//
//	//render physic flags : 
//	for (int i = 0; i < flags.size(); i++)
//	{
//		flags[i]->render(projection, worldToView);
//	}
//	CHECK_GL_ERROR("error when rendering flags");
//
//	//render terrain :
//	terrain.render(projection, worldToView);
//	CHECK_GL_ERROR("error when rendering terrain");
//
//	terrain.renderGrassField(projection, worldToView);
//	CHECK_GL_ERROR("error when rendering grass");
//
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	CHECK_GL_ERROR("error in G pass");
//	////// end G pass
//
//
//	m_lightPassBuffer.bind();
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	///// begin light pass
//	// Disable the depth test
//	glDisable(GL_DEPTH_TEST);
//	// Enable blending
//	glEnable(GL_BLEND);
//	// Setup additive blending
//	glBlendFunc(GL_ONE, GL_ONE);
//
//
//	// Render quad
//	glm::vec4 viewport; 
//	
//	//point light : 
//	m_pointLightMaterial->use();
//
//	// send screen to world matrix : 
//	m_pointLightMaterial->setUniformScreenToWorld(screenToWorld);
//	m_pointLightMaterial->setUniformCameraPosition(cameraPosition);
//
//	//geometry informations :
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);
//
//	m_pointLightMaterial->setUniformColorTexture(0);
//	m_pointLightMaterial->setUniformNormalTexture(1);
//	m_pointLightMaterial->setUniformDepthTexture(2);
//
//	for (int i = 0; i < pointLightCount; i++)
//	{
//		int lightIdx = pointLightCullingInfos[i].idx;
//		viewport = pointLightCullingInfos[i].viewport;
//
//		if (i < lightManager->getShadowMapCount(LightManager::POINT))
//		{
//			//active the shadow map texture
//			glActiveTexture(GL_TEXTURE3);
//			lightManager->bindShadowMapTexture(LightManager::POINT, i);
//			//glUniform1i(uniformTextureShadow[POINT], 3); // send shadow texture
//			m_pointLightMaterial->setUniformShadowTexture(3);
//		}
//
//		//if (passCullingTest(viewport, projection, worldToView, camera.eye, pointLights[i]->boundingBox)) // optimisation test
//		{
//			//resize viewport
//			resizeBlitQuad(viewport);
//
//			//glUniform1f(uniformLightFarPlane, 100.f);
//			m_pointLightMaterial->setUniformFarPlane(100.f);
//
//			lightManager->uniformPointLight(*pointLights[lightIdx]);
//			quadMesh.draw();
//		}
//	}
//
//	//spot lights : 
//	m_spotLightMaterial->use();
//
//	// send screen to world matrix : 
//	m_spotLightMaterial->setUniformScreenToWorld(screenToWorld);
//	m_spotLightMaterial->setUniformCameraPosition(cameraPosition);
//
//
//	//geometry informations :
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);
//
//	m_spotLightMaterial->setUniformColorTexture(0);
//	m_spotLightMaterial->setUniformNormalTexture(1);
//	m_spotLightMaterial->setUniformDepthTexture(2);
//
//	for (int i = 0; i < spotLightCount; i++)
//	{
//
//		int lightIdx = spotLightCullingInfos[i].idx;
//		viewport = spotLightCullingInfos[i].viewport;
//
//		if (i < lightManager->getShadowMapCount(LightManager::SPOT))
//		{
//			//active the shadow map texture
//			glActiveTexture(GL_TEXTURE3);
//			lightManager->bindShadowMapTexture(LightManager::SPOT, i);
//			//glUniform1i(uniformTextureShadow[SPOT], 3); // send shadow texture
//			m_spotLightMaterial->setUniformShadowTexture(3);
//		}
//
//		//if (passCullingTest(viewport , projection, worldToView, camera.eye, spotLights[i]->boundingBox)) // optimisation test
//		{
//			//resize viewport
//			resizeBlitQuad(viewport);
//
//			glm::mat4 projectionSpotLight = glm::perspective(spotLights[lightIdx]->angle*2.f, 1.f, 0.1f, 100.f);
//			glm::mat4 worldToLightSpotLight = glm::lookAt(spotLights[lightIdx]->position, spotLights[lightIdx]->position + spotLights[lightIdx]->direction, spotLights[lightIdx]->up);
//			glm::mat4 WorldToLightScreen = projectionSpotLight * worldToLightSpotLight;
//			//glUniformMatrix4fv(uniformWorldToLightScreen_spot, 1, false, glm::value_ptr(WorldToLightScreen));
//			m_spotLightMaterial->setUniformWorldToLight(WorldToLightScreen);
//
//			lightManager->uniformSpotLight(*spotLights[lightIdx]);
//			quadMesh.draw();
//		}
//	}
//
//	//make sure that the blit quat cover all the screen : 
//	resizeBlitQuad();
//
//
//	//directionals : 
//	m_directionalLightMaterial->use();
//
//	// send screen to world matrix : 
//	m_directionalLightMaterial->setUniformScreenToWorld(screenToWorld);
//	m_directionalLightMaterial->setUniformCameraPosition(cameraPosition);
//
//
//	//geometry informations :
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);
//
//	m_directionalLightMaterial->setUniformColorTexture(0);
//	m_directionalLightMaterial->setUniformNormalTexture(1);
//	m_directionalLightMaterial->setUniformDepthTexture(2);
//
//
//	for (int i = 0; i < directionalLights.size(); i++)
//	{
//		if (i < lightManager->getShadowMapCount(LightManager::DIRECTIONAL))
//		{
//			//active the shadow map texture
//			glActiveTexture(GL_TEXTURE3);
//			lightManager->bindShadowMapTexture(LightManager::DIRECTIONAL, i);
//			//glUniform1i(uniformTextureShadow[DIRECTIONAL], 3); // send shadow texture
//			m_directionalLightMaterial->setUniformShadowTexture(3);
//		}
//
//		float directionalShadowMapRadius = lightManager->getDirectionalShadowMapViewportSize()*0.5f;
//		float directionalShadowMapNear = lightManager->getDirectionalShadowMapViewportNear();
//		float directionalShadowMapFar = lightManager->getDirectionalShadowMapViewportFar();
//		glm::vec3 orig = glm::vec3(cameraForward.x, 0, cameraForward.z)*directionalShadowMapRadius + glm::vec3(cameraPosition.x, directionalLights[i]->position.y/*directionalShadowMapFar*0.5f*/, cameraPosition.z);
//		glm::vec3 eye = -directionalLights[i]->direction + orig;
//		glm::mat4 projectionDirectionalLight = glm::ortho(-directionalShadowMapRadius, directionalShadowMapRadius, -directionalShadowMapRadius, directionalShadowMapRadius, directionalShadowMapNear, directionalShadowMapFar);
//		glm::mat4 worldToLightDirectionalLight = glm::lookAt(eye, orig, directionalLights[i]->up);
//		glm::mat4 WorldToLightScreen = projectionDirectionalLight * worldToLightDirectionalLight;
//		//glUniformMatrix4fv(uniformWorldToLightScreen_directional, 1, false, glm::value_ptr(WorldToLightScreen));
//		m_directionalLightMaterial->setUniformWorldToLight(WorldToLightScreen);
//
//		lightManager->uniformDirectionalLight(*directionalLights[i]);
//		quadMesh.draw();
//	}
//
//	// Disable blending
//	glDisable(GL_BLEND);	
//	glEnable(GL_DEPTH_TEST);
//
//	CHECK_GL_ERROR("error in light pass");
//	///// end light pass
//
//	m_lightPassBuffer.unbind();
//
//	///////// end deferred
//
//
//	///////////// end draw world
//
//
//	//////////////////////// end scene rendering
//
//	///////// turn on forward rendering : 
//
//	// Transfert depth to main buffer : 
//	gBufferFBO.bind(GL_READ_FRAMEBUFFER);
//	m_lightPassBuffer.bind(GL_DRAW_FRAMEBUFFER);
//	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//	// Rebind main buffer as usual : 
//	m_lightPassBuffer.bind();
//
//	//render skybox : 
//	skybox.render(projection, worldToView);
//
//	glEnable(GL_BLEND);
//	glDepthMask(GL_FALSE);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	for (int i = 0; i < billboards.size(); i++)
//		billboards[i]->render(projection, worldToView);
//
//	for (int i = 0; i < particleEmitters.size(); i++)
//		particleEmitters[i]->render(projection, worldToView);
//	glDepthMask(GL_TRUE);
//	glDisable(GL_BLEND);
//
//	CHECK_GL_ERROR("error in forward pass");
//
//	///////////////////////////////////////////////
//	///// Debug draw : 
//	if (debugDrawer != nullptr)
//	{
//		debugDrawer->drawOutputIfNeeded("gBuffer_color", gPassColorTexture.glId);
//		debugDrawer->drawOutputIfNeeded("gBuffer_normal", gPassNormalTexture.glId);
//		debugDrawer->drawOutputIfNeeded("gBuffer_depth", gPassDepthTexture.glId);
//		CHECK_GL_ERROR("error in render debug pass");
//		glViewport(0, 0, m_viewportRenderSize.x, m_viewportRenderSize.y);
//		m_lightPassBuffer.bind();
//	}
//
//}
//


void Renderer::render(BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, DebugDrawRenderer* debugDrawer)
{
	glClearColor(0, 0, 0, 0);

	renderLightedScene(camera, pointLights, directionalLights, spotLights, debugDrawer);

	if (camera.getPostProcessProxy().getOperationCount() > 0)
	{
		m_postProcessManager.render(camera, *m_lightPassHDRColor, *m_lightPassHighValues, *m_lightPassDepth, gPassHightValuesTexture, pointLights, debugDrawer);
		m_postProcessManager.renderResultOnCamera(camera);
	}
	else
	{
		camera.renderFrame(m_lightPassHDRColor);
	}
}

void Renderer::renderLightedScene(const BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, DebugDrawRenderer* debugDrawer)
{
	////////////////////////////////////////////////////////////////////////
	///////// BEGIN : Update matrices
	const int width = m_viewportRenderSize.x;
	const int height = m_viewportRenderSize.y;
	const glm::vec3& cameraPosition = camera.getCameraPosition();
	const glm::vec3& cameraForward = camera.getCameraForward();
	const glm::mat4& projection = camera.getProjectionMatrix();
	const glm::mat4& view = camera.getViewMatrix();
	const glm::mat4 camera_mvp = projection * view;
	const glm::mat4 screenToView = glm::transpose(glm::inverse(projection));
	///////// END : Update matrices
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	///////// BEGIN : Get render batches
	const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches = camera.getRenderBatches(PipelineTypes::OPAQUE_PIPILINE);
	const std::map<GLuint, std::shared_ptr<IRenderBatch>>& transparentRenderBatches= camera.getRenderBatches(PipelineTypes::TRANSPARENT_PIPELINE);
	///////// END : Get render batches
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	///////// BEGIN : ShadowPass
	shadowPass(camera, opaqueRenderBatches, transparentRenderBatches, pointLights, directionalLights, spotLights, debugDrawer);
	///////// END : ShadowPass
	////////////////////////////////////////////////////////////////////////

	// Viewport 
	glViewport(0, 0, width, height);
	// Clear default buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///////////// begin draw world

	////////////////////////////////////////////////////////////////////////
	///////// BEGIN : Deferred
	deferredPipeline(opaqueRenderBatches, camera, projection, view, cameraPosition, cameraForward, screenToView, camera_mvp, pointLights, directionalLights, spotLights);
	///////// END : Deferred
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	///////// BEGIN : Transfert depth to main buffer
	gBufferFBO.bind(GL_READ_FRAMEBUFFER);
	m_lightPassBuffer.bind(GL_DRAW_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	///////// END : Transfert depth to main buffer
	////////////////////////////////////////////////////////////////////////

	m_lightPassBuffer.bind();
	if (camera.getClearMode() == BaseCamera::ClearMode::SKYBOX)
		camera.renderSkybox();
	m_lightPassBuffer.unbind();

	////////////////////////////////////////////////////////////////////////
	///////// BEGIN :  Forward 
	forwardPipeline(transparentRenderBatches, width, height, projection, view);
	///////// END :  Forward
	////////////////////////////////////////////////////////////////////////
	
	///////////// end draw world

	/////////////////////////////////////////////////////////////////////////
	/////// BEGIN : Debug draw
	if (debugDrawer != nullptr)
	{
		debugDrawRenderer(*debugDrawer);
	}
	/////// END : Debug draw
	/////////////////////////////////////////////////////////////////////////

	// Prepare futur calls
	glViewport(0, 0, width, height);
	m_lightPassBuffer.bind();
}

void Renderer::shadowPass(const BaseCamera& camera, const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const std::map<GLuint, std::shared_ptr<IRenderBatch>>& transparentRenderBatches, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, DebugDrawRenderer* debugDrawer)
{
	const glm::vec3& cameraForward = camera.getCameraForward();
	const glm::vec3& cameraPosition = camera.getCameraPosition();

	glEnable(GL_DEPTH_TEST);

	//culling for lights : 
	updateCulling(camera, pointLights, spotLights, pointLightCullingInfos, spotLightCullingInfos);

	//TODO : check if shadow map count and light count match

	//for spot lights : 
	//glUseProgram(glProgram_shadowPass);
	shadowPassMaterial->use();
	for (int shadowIdx = 0; shadowIdx < spotLightCount; shadowIdx++)
	{
		int lightIdx = spotLightCullingInfos[shadowIdx].idx;

		if (shadowIdx < lightManager->getShadowMapCount(LightManager::SPOT))
		{
			lightManager->bindShadowMapFBO(LightManager::SPOT, shadowIdx);
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 lightProjection = glm::perspective(spotLights[lightIdx]->angle*2.f, 1.f, 0.1f, 100.f);
			glm::mat4 lightView = glm::lookAt(spotLights[lightIdx]->position, spotLights[lightIdx]->position + spotLights[lightIdx]->direction, spotLights[lightIdx]->up);

			for (auto& renderBatchPair : opaqueRenderBatches)
			{
				const IRenderBatch& renderBatch = *renderBatchPair.second;
				for (auto& drawable : renderBatch.getDrawables())
				{
					if(drawable->castShadows())
						renderShadows(lightProjection, lightView, *drawable);
				}
			}

			//for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
			//{
			//	renderShadows(lightProjection, lightView, *meshRenderers[meshIdx]);
			//}

			lightManager->unbindShadowMapFBO(LightManager::SPOT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	//for directional lights : 
	//glUseProgram(glProgram_shadowPass);
	shadowPassMaterial->use();
	for (int lightIdx = 0; lightIdx < directionalLights.size(); lightIdx++)
	{
		if (lightIdx < lightManager->getShadowMapCount(LightManager::DIRECTIONAL))
		{
			lightManager->bindShadowMapFBO(LightManager::DIRECTIONAL, lightIdx);
			glClear(GL_DEPTH_BUFFER_BIT);
			float directionalShadowMapRadius = lightManager->getDirectionalShadowMapViewportSize()*0.5f;
			float directionalShadowMapNear = lightManager->getDirectionalShadowMapViewportNear();
			float directionalShadowMapFar = lightManager->getDirectionalShadowMapViewportFar();
			glm::vec3 orig = directionalLights[lightIdx]->position; // glm::vec3(cameraForward.x, 0, cameraForward.z)*directionalShadowMapRadius + glm::vec3(cameraPosition.x, directionalLights[lightIdx]->position.y /*directionalShadowMapFar*0.5f*/, cameraPosition.z);
			glm::vec3 eye = -directionalLights[lightIdx]->direction + orig;
			glm::mat4 lightProjection = glm::ortho(-directionalShadowMapRadius, directionalShadowMapRadius, -directionalShadowMapRadius, directionalShadowMapRadius, directionalShadowMapNear, directionalShadowMapFar);
			glm::mat4 lightView = glm::lookAt(eye, orig, directionalLights[lightIdx]->up);

			for (auto& renderBatchPair : opaqueRenderBatches)
			{
				const IRenderBatch& renderBatch = *renderBatchPair.second;
				for (auto& drawable : renderBatch.getDrawables())
				{
					if (drawable->castShadows())
						renderShadows(lightProjection, lightView, *drawable);
				}
			}

			//for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
			//{
			//	renderShadows(lightProjection, lightView, *meshRenderers[meshIdx]);
			//}
			lightManager->unbindShadowMapFBO(LightManager::DIRECTIONAL);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	//for point lights : 
	//glUseProgram(glProgram_shadowPassOmni);
	shadowPassOmniMaterial->use();
	for (int shadowIdx = 0; shadowIdx < pointLightCount; shadowIdx++)
	{
		int lightIdx = pointLightCullingInfos[shadowIdx].idx;

		if (shadowIdx < lightManager->getShadowMapCount(LightManager::POINT))
		{
			lightManager->bindShadowMapFBO(LightManager::POINT, shadowIdx);
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), 1.f, 1.f, 100.f);
			std::vector<glm::mat4> lightVPs;
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(1.f, 0.f, 0.f) , glm::vec3(0.f, -1.f, 0.f)) );
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)) );
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 1.f, 0.f) , glm::vec3(0.f, 0.f, 1.f) ) );
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)) );
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 0.f, 1.f) , glm::vec3(0.f, -1.f, 0.f)) );
			lightVPs.push_back(lightProjection * glm::lookAt(pointLights[lightIdx]->position, pointLights[lightIdx]->position + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)) );

			for (auto& renderBatchPair : opaqueRenderBatches)
			{
				const IRenderBatch& renderBatch = *renderBatchPair.second;
				for (auto& drawable : renderBatch.getDrawables())
				{
					if (drawable->castShadows())
						renderShadows(100.f, pointLights[lightIdx]->position, lightVPs, *drawable);
				}
			}

			//for (int meshIdx = 0; meshIdx < meshRenderers.size(); meshIdx++)
			//{
			//	renderShadows(100.f, pointLights[lightIdx]->position, lightVPs, *meshRenderers[meshIdx]); // draw shadow for the first spot light
			//}
			lightManager->unbindShadowMapFBO(LightManager::POINT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	CHECK_GL_ERROR("error in shadow pass");

	if (debugDrawer != nullptr)
	{
		if(lightManager->getShadowMapCount(LightManager::DIRECTIONAL) > 0)
			debugDrawer->drawOutputIfNeeded("shadow_directionnal", lightManager->getDirectionalShadowMap(0).getTextureId());
		if (lightManager->getShadowMapCount(LightManager::SPOT) > 0)
			debugDrawer->drawOutputIfNeeded("shadow_spot", lightManager->getSpotShadowMap(0).getTextureId());
	}
}

void Renderer::gPass(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const glm::mat4& projection, const glm::mat4& view)
{
	glEnable(GL_DEPTH_TEST);

	// Render batches (meshes and flags for now)
	for (auto& renderBatch : opaqueRenderBatches)
	{
		renderBatch.second->render(projection, view);
	}
	
	// TODO RENDERING
	////render terrain :
	//terrain.render(projection, worldToView);
	//CHECK_GL_ERROR("error when rendering terrain");

	//terrain.renderGrassField(projection, worldToView);
	//CHECK_GL_ERROR("error when rendering grass");

	CHECK_GL_ERROR("error in G pass");
}

void Renderer::lightPass(const glm::mat4& screenToView, const glm::vec3& cameraPosition, const glm::vec3& cameraForward, const glm::mat4& view, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights)
{

	// The View to world matrix is the same of all the process, we compute it here :
	glm::mat4 viewToWorld = glm::inverse(view);

	///// begin light pass
	// Disable the depth test
	glDisable(GL_DEPTH_TEST);
	// Enable blending
	glEnable(GL_BLEND);
	// Setup additive blending
	glBlendFunc(GL_ONE, GL_ONE);


	// Render quad
	glm::vec4 viewport;

	//point light : 
	m_pointLightMaterial->use();

	// send screen to world matrix : 
	m_pointLightMaterial->setUniformScreenToView(screenToView);
	//m_pointLightMaterial->setUniformCameraPosition(cameraPosition);

	//geometry informations :
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);

	m_pointLightMaterial->setUniformColorTexture(0);
	m_pointLightMaterial->setUniformNormalTexture(1);
	m_pointLightMaterial->setUniformDepthTexture(2);

	for (int i = 0; i < pointLightCount; i++)
	{
		int lightIdx = pointLightCullingInfos[i].idx;
		viewport = pointLightCullingInfos[i].viewport;

		if (i < lightManager->getShadowMapCount(LightManager::POINT))
		{
			//active the shadow map texture
			glActiveTexture(GL_TEXTURE3);
			lightManager->bindShadowMapTexture(LightManager::POINT, i);
			//glUniform1i(uniformTextureShadow[POINT], 3); // send shadow texture
			m_pointLightMaterial->setUniformShadowTexture(3);
		}

		//if (passCullingTest(viewport, projection, worldToView, camera.eye, pointLights[i]->boundingBox)) // optimisation test
		{
			//resize viewport
			resizeBlitQuad(viewport);

			//glUniform1f(uniformLightFarPlane, 100.f);
			m_pointLightMaterial->setUniformViewToWorld(viewToWorld);
			m_pointLightMaterial->setUniformFarPlane(100.f);

			lightManager->uniformPointLight(*pointLights[lightIdx], view);
			quadMesh.draw();
		}
	}

	//spot lights : 
	m_spotLightMaterial->use();

	// send screen to world matrix : 
	m_spotLightMaterial->setUniformScreenToView(screenToView);
	//m_spotLightMaterial->setUniformCameraPosition(cameraPosition);


	//geometry informations :
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);

	m_spotLightMaterial->setUniformColorTexture(0);
	m_spotLightMaterial->setUniformNormalTexture(1);
	m_spotLightMaterial->setUniformDepthTexture(2);

	for (int i = 0; i < spotLightCount; i++)
	{

		int lightIdx = spotLightCullingInfos[i].idx;
		viewport = spotLightCullingInfos[i].viewport;

		if (i < lightManager->getShadowMapCount(LightManager::SPOT))
		{
			//active the shadow map texture
			glActiveTexture(GL_TEXTURE3);
			lightManager->bindShadowMapTexture(LightManager::SPOT, i);
			//glUniform1i(uniformTextureShadow[SPOT], 3); // send shadow texture
			m_spotLightMaterial->setUniformShadowTexture(3);
		}

		//if (passCullingTest(viewport , projection, worldToView, camera.eye, spotLights[i]->boundingBox)) // optimisation test
		{
			//resize viewport
			resizeBlitQuad(viewport);

			const glm::vec3 spotLightViewPosition = glm::vec3(view * glm::vec4(spotLights[lightIdx]->position, 1.0));
			const glm::vec3 spotLightViewDirection = glm::vec3(view * glm::vec4(spotLights[lightIdx]->direction, 0.0));
			const glm::vec3 spotLightViewUp = glm::vec3(view * glm::vec4(spotLights[lightIdx]->up, 0.0));
			glm::mat4 projectionSpotLight = glm::perspective(spotLights[lightIdx]->angle*2.f, 1.f, 0.1f, 100.f);
			glm::mat4 worldToLightSpotLight = glm::lookAt(spotLightViewPosition, spotLightViewPosition + spotLightViewDirection, spotLightViewUp);
			glm::mat4 ViewToLightScreen = projectionSpotLight * worldToLightSpotLight;
			//glUniformMatrix4fv(uniformWorldToLightScreen_spot, 1, false, glm::value_ptr(WorldToLightScreen));
			m_spotLightMaterial->setUniformViewToLight(ViewToLightScreen);

			lightManager->uniformSpotLight(*spotLights[lightIdx], view);
			quadMesh.draw();
		}
	}

	//make sure that the blit quat cover all the screen : 
	resizeBlitQuad();


	//directionals : 
	m_directionalLightMaterial->use();

	// send screen to world matrix : 
	m_directionalLightMaterial->setUniformScreenToView(screenToView);
	//m_directionalLightMaterial->setUniformCameraPosition(cameraPosition);


	//geometry informations :
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPassColorTexture.glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPassNormalTexture.glId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gPassDepthTexture.glId);

	m_directionalLightMaterial->setUniformColorTexture(0);
	m_directionalLightMaterial->setUniformNormalTexture(1);
	m_directionalLightMaterial->setUniformDepthTexture(2);


	for (int i = 0; i < directionalLights.size(); i++)
	{
		if (i < lightManager->getShadowMapCount(LightManager::DIRECTIONAL))
		{
			//active the shadow map texture
			glActiveTexture(GL_TEXTURE3);
			lightManager->bindShadowMapTexture(LightManager::DIRECTIONAL, i);
			//glUniform1i(uniformTextureShadow[DIRECTIONAL], 3); // send shadow texture
			m_directionalLightMaterial->setUniformShadowTexture(3);
		}

		const glm::vec3 directionalLightViewPosition = glm::vec3(view * glm::vec4(directionalLights[i]->position, 1.0));
		const glm::vec3 directionalLightViewDirection = glm::vec3(view * glm::vec4(directionalLights[i]->direction, 0.0));
		const glm::vec3 directionalLightViewUp = glm::vec3(view * glm::vec4(directionalLights[i]->up, 0.0));
		float directionalShadowMapRadius = lightManager->getDirectionalShadowMapViewportSize()*0.5f;
		float directionalShadowMapNear = lightManager->getDirectionalShadowMapViewportNear();
		float directionalShadowMapFar = lightManager->getDirectionalShadowMapViewportFar();
		glm::vec3 orig = directionalLightViewPosition; ///*glm::vec3(0, 0, 1)*directionalShadowMapRadius + */glm::vec3(0, directionalLightViewPosition.y/*directionalShadowMapFar*0.5f*/, 0);
		glm::vec3 eye = -directionalLightViewDirection + orig;
		glm::mat4 projectionDirectionalLight = glm::ortho(-directionalShadowMapRadius, directionalShadowMapRadius, -directionalShadowMapRadius, directionalShadowMapRadius, directionalShadowMapNear, directionalShadowMapFar);
		glm::mat4 viewToLightDirectionalLight = glm::lookAt(eye, orig, directionalLightViewUp);
		glm::mat4 viewToLightScreen = projectionDirectionalLight * viewToLightDirectionalLight;
		//glUniformMatrix4fv(uniformWorldToLightScreen_directional, 1, false, glm::value_ptr(WorldToLightScreen));
		m_directionalLightMaterial->setUniformViewToLight(viewToLightScreen);

		lightManager->uniformDirectionalLight(*directionalLights[i], view);
		quadMesh.draw();
	}

	// Disable blending
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	CHECK_GL_ERROR("error in light pass");
}

void Renderer::deferredPipeline(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const BaseCamera& camera, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& cameraPosition, const glm::vec3& cameraForward, const glm::mat4& screenToView, const glm::mat4& camera_mvp, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights)
{
	////// begin G pass 
	gBufferFBO.bind();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gPass(opaqueRenderBatches, projection, view);
	gBufferFBO.unbind();
	////// end G pass

	////// begin SSAO pass
	m_postProcessManager.renderSSAO(camera);
	////// end SSAO pass

	///// begin light pass
	m_lightPassBuffer.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lightPass(screenToView, cameraPosition, cameraForward, view, pointLights, directionalLights, spotLights);
	m_lightPassBuffer.unbind();
	///// end light pass
}

void Renderer::forwardPipeline(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& transparentRenderBatches, int width, int height, const glm::mat4& projection, const glm::mat4& view)
{
	// Rebind main buffer as usual : 
	m_lightPassBuffer.bind();

	//render skybox : 
	//skybox.render(projection, worldToView); // TODO RENDERING

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render batches (particles and billboards for now)
	for (auto& renderBatch : transparentRenderBatches)
	{
		renderBatch.second->render(projection, view);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	m_lightPassBuffer.unbind();
	CHECK_GL_ERROR("error in forward pass");
}

void Renderer::debugDrawRenderer(DebugDrawRenderer& debugDrawer) const
{
	debugDrawer.addSeparator();
	debugDrawer.drawOutputIfNeeded("gBuffer_color", gPassColorTexture.glId);
	debugDrawer.drawOutputIfNeeded("gBuffer_normal", gPassNormalTexture.glId);
	debugDrawer.drawOutputIfNeeded("gBuffer_depth", gPassDepthTexture.glId);
	debugDrawer.addSeparator();
	debugDrawer.drawOutputIfNeeded("beauty_color", m_lightPassHDRColor->glId);
	debugDrawer.drawOutputIfNeeded("beauty_depth", m_lightPassDepth->glId);
	debugDrawer.drawOutputIfNeeded("beauty_highValues", m_lightPassHighValues->glId);
	CHECK_GL_ERROR("error in render debug pass");
}

void Renderer::transferDepthTo(const GlHelper::Framebuffer & to, const glm::vec2 & depthTextureSize) const
{
	m_lightPassBuffer.bind(GL_READ_FRAMEBUFFER);
	to.bind(GL_DRAW_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, depthTextureSize.x, depthTextureSize.y, 0, 0, depthTextureSize.x, depthTextureSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::debugDrawColliders(const BaseCamera& camera, const std::vector<Entity*>& entities)
{
	const int width = m_viewportRenderSize.x;
	const int height = m_viewportRenderSize.y;

	glm::mat4 projection = camera.getProjectionMatrix();//glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 view = camera.getViewMatrix();// glm::lookAt(camera.eye, camera.o, camera.up);
	glm::mat4 vp = projection * view;
	glm::mat4 screenToWorld = glm::transpose(glm::inverse(vp));

	//draw collider in forward rendering pass (no lightning)
	for (int i = 0; i < entities.size(); i++)
	{
		Collider* collider = static_cast<Collider*>(entities[i]->getComponent(Component::ComponentType::COLLIDER));
		if (collider == nullptr)
			continue;

		glm::vec3 colliderColor(1, 0, 0);
		if (entities[i]->getIsSelected())
			colliderColor = glm::vec3(1, 1, 0);

		collider->render(projection, view, colliderColor);
	}

	CHECK_GL_ERROR("error when rendering : debugDrawColliders");
}
//
//void Renderer::debugDrawDeferred()
//{
//	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();
//
//	///////////// begin draw blit quad
//	glDisable(GL_DEPTH_TEST);
//
//	//glUseProgram(glProgram_blit);
//	glProgram_blit->use();
//
//	for (int i = 0; i < 3; i++)
//	{
//		glViewport((width * i) / 4, 0, width / 4, height / 4);
//
//		glActiveTexture(GL_TEXTURE0);
//		// Bind gbuffer color texture
//		glBindTexture(GL_TEXTURE_2D, gbufferTextures[i]);
//		//glUniform1i(uniformTextureBlit, 0);
//		glProgram_blit->setUniformBlitTexture(0);
//
//		quadMesh.draw();
//	}
//
//	//shadow : 
//	if (lightManager->getShadowMapCount(LightManager::DIRECTIONAL) > 0)
//	{
//		glViewport((width * 3) / 4, 0, width / 4, height / 4);
//		glActiveTexture(GL_TEXTURE0);
//		lightManager->bindShadowMapTexture(LightManager::DIRECTIONAL, 0);
//		//glUniform1i(uniformTextureBlit, 0);
//		glProgram_blit->setUniformBlitTexture(0);
//
//		quadMesh.draw();
//	}
//
//
//	glViewport(0, 0, width, height);
//
//	glEnable(GL_DEPTH_TEST);
//	///////////// end draw blit quad
//}

void Renderer::debugDrawLights(const BaseCamera& camera, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights)
{
	int width = m_viewportRenderSize.x;
	int height = m_viewportRenderSize.y;

	glm::mat4 projection = camera.getProjectionMatrix(); //glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 view = camera.getViewMatrix(); //glm::lookAt(camera.eye, camera.o, camera.up);

	for (auto& light : pointLights)
	{
		light->renderBoundingBox(projection, view, glm::vec3(0, 0, 1));
	}

	for (auto& light : spotLights)
	{
		light->renderBoundingBox(projection, view, glm::vec3(0, 0, 1));
	}

	CHECK_GL_ERROR("error when rendering : debugDrawLights");
}


bool Renderer::passCullingTest(glm::vec4& viewport, const glm::mat4& projection, const glm::mat4& view, const glm::vec3 cameraPosition, BoxCollider& collider)
{
	glm::vec3 topRight = collider.topRight;
	glm::vec3 bottomLeft = collider.bottomLeft;

	bool insideFrustum = false;

	float maxX = 1;
	float maxY = 1;
	float minX = -1;
	float minY = -1;

	//first optimisation : test if the light is inside camera frustum 

	//is camera outside light bounding box ? 
	if (!(cameraPosition.x > bottomLeft.x && cameraPosition.x < topRight.x &&
		cameraPosition.y > bottomLeft.y && cameraPosition.y < topRight.y &&
		cameraPosition.z > bottomLeft.z && cameraPosition.z < topRight.z))
	{

		// ce code semble plus optimisé que celui retenu, mais je n'ai pas réussi à le faire marcher convenablement pour deux raisons : 
		// le carré généré est trop petit, il n'englobe pas tout le champs d'action de la lumière
		// il y a des bugs lorsqu'on place la camera au dessus ou en dessous de la lumière, la rotation s'effectue mal et le collider se deforme.
		
		/*
		// permet au collider d'être toujours face à la camera. 
		//cela permet d'éviter les erreurs du au caractère "AABB" du collider.
		//en effet, on stock le collider avec une seule diagonale (deux points), il faut qu'une fois projeté en repère ecrant cette diagonale soit aussi celle du carré projeté.
		glm::vec3 camToCollider = glm::normalize(collider.translation - cameraPosition);
		glm::mat4 facingCameraRotation = glm::lookAt(camToCollider, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		facingCameraRotation = glm::inverse(glm::mat4(facingCameraRotation));

		collider.applyRotation(glm::quat(facingCameraRotation));

		glm::vec4 tmpTopRight = projection * view  * glm::vec4(topRight, 1);
		glm::vec4 tmpBottomLeft = projection * view  * glm::vec4(bottomLeft, 1);

		topRight = glm::vec3(tmpTopRight.x / tmpTopRight.w, tmpTopRight.y / tmpTopRight.w, tmpTopRight.z / tmpTopRight.w);
		bottomLeft = glm::vec3(tmpBottomLeft.x / tmpBottomLeft.w, tmpBottomLeft.y / tmpBottomLeft.w, tmpBottomLeft.z / tmpBottomLeft.w);
		//now, topRight and bottom left are in screen space, and their coordinates are between -1 and 1 on all axis.

		//is light outside the camera frustum ? 
		if ((topRight.x < -1 && bottomLeft.x < -1) || (topRight.y < -1 && bottomLeft.y < -1)
			|| (topRight.x > 1 && bottomLeft.x > 1) || (topRight.y > 1 && bottomLeft.y > 1))
		{
			//we don't have to draw the light if its bounding box is outside the camera frustum
			insideFrustum = false;
		}
		*/

		//compute 8 points of 3D collider : 

		float colliderWidth = glm::abs(topRight.x - bottomLeft.x);
		float colliderDepth = glm::abs(topRight.z - bottomLeft.z);

		glm::vec3 colliderPoints[8] = { bottomLeft, glm::vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z + colliderDepth), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z + colliderDepth),
										topRight, glm::vec3(topRight.x, topRight.y, topRight.z - colliderDepth), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z - colliderDepth) };

		glm::mat4 vp = projection * view;
		glm::vec4 tmpColliderPoint;
		maxX = -1;
		maxY = -1;
		minX = 1;
		minY = 1;
		for (int i = 0; i < 8; i++)
		{
			tmpColliderPoint = projection * view * glm::vec4(colliderPoints[i], 1);
			colliderPoints[i] = glm::vec3(tmpColliderPoint.x / tmpColliderPoint.w, tmpColliderPoint.y / tmpColliderPoint.w, tmpColliderPoint.z / tmpColliderPoint.w);
		
			if (colliderPoints[i].x > maxX)
				maxX = colliderPoints[i].x;
			if(colliderPoints[i].x < minX)
				minX = colliderPoints[i].x;

			if (colliderPoints[i].y > maxY)
				maxY = colliderPoints[i].y;
			if (colliderPoints[i].y < minY)
				minY = colliderPoints[i].y;
		}

		//is light outside the camera frustum ? 
		if ((maxX < -1 && minX < -1) || (maxY < -1 && minY < -1)
			|| (maxX > 1 && minX > 1) || (maxY > 1 && minY > 1))
		{
			//we don't have to draw the light if its bounding box is outside the camera frustum
			insideFrustum = false;
		}


		//we have to draw the light if its boudning box is inside/intersect the camera frustum
		insideFrustum = true;
	}
	//we draw the light if we are inside its bounding box
	insideFrustum = true;




	//second optimisation : we modify the quad verticies such that it is reduce to the area of the light
	if (insideFrustum)
	{
		float width = maxX - minX;
		float height = maxY - minY;
		viewport = glm::vec4(minX, minY, width, height);

		//quadMesh.vertices = { minX, minY, minX + width, minY, minX, minY + height, minX + width , minY + height };
		//// update in CG : 
		//glBindBuffer(GL_ARRAY_BUFFER, quadMesh.vbo_vertices);
		//glBufferData(GL_ARRAY_BUFFER, quadMesh.vertices.size() * sizeof(float), &(quadMesh.vertices)[0], GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return insideFrustum;
}

void Renderer::resizeBlitQuad(const glm::vec4 & viewport)
{
	quadMesh.vertices = { viewport.x, viewport.y, viewport.x + viewport.z, viewport.y, viewport.x, viewport.y + viewport.w, viewport.x + viewport.z , viewport.y + viewport.w };
	// update in CG : 
	glBindBuffer(GL_ARRAY_BUFFER, quadMesh.vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, quadMesh.vertices.size() * sizeof(float), &(quadMesh.vertices)[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}




void Renderer::updateCulling(const BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<SpotLight*>& spotLights, std::vector<LightCullingInfo>& pointLightCullingInfos, std::vector<LightCullingInfo>& spotLightCullingInfos )
{
	glm::vec3 cameraPosition = camera.getCameraPosition();


	if (!pointLightCullingInfos.empty())
		pointLightCullingInfos.clear();
	if (!spotLightCullingInfos.empty())
		spotLightCullingInfos.clear();

	bool insideFrustum = false;

	float maxX = 1;
	float maxY = 1;
	float minX = -1;
	float minY = -1;

	pointLightCount = 0;
	spotLightCount = 0;

	int width = m_viewportRenderSize.x;
	int height = m_viewportRenderSize.y;

	glm::mat4 projection = camera.getProjectionMatrix(); //glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 view = camera.getViewMatrix(); //glm::lookAt(camera.eye, camera.o, camera.up);

	int lastId = pointLights.size() - 1;;
	for (int i = 0; i < pointLights.size(); i++)
	{

		BoxCollider& collider = pointLights[i]->boundingBox;
		glm::vec3 topRight = collider.topRight;
		glm::vec3 bottomLeft = collider.bottomLeft;


		if (!(cameraPosition.x > bottomLeft.x && cameraPosition.x < topRight.x && cameraPosition.y > bottomLeft.y && cameraPosition.y < topRight.y && cameraPosition.z > bottomLeft.z && cameraPosition.z < topRight.z))
		{

			//compute 8 points of 3D collider : 

			float colliderWidth = glm::abs(topRight.x - bottomLeft.x);
			float colliderDepth = glm::abs(topRight.z - bottomLeft.z);

			glm::vec3 colliderPoints[8] = { bottomLeft, glm::vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z + colliderDepth), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z + colliderDepth),
				topRight, glm::vec3(topRight.x, topRight.y, topRight.z - colliderDepth), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z - colliderDepth) };

			glm::mat4 vp = projection * view;
			glm::vec4 tmpColliderPoint;
			maxX = -1;
			maxY = -1;
			minX = 1;
			minY = 1;
			for (int i = 0; i < 8; i++)
			{
				tmpColliderPoint = projection * view * glm::vec4(colliderPoints[i], 1);
				colliderPoints[i] = glm::vec3(tmpColliderPoint.x / tmpColliderPoint.w, tmpColliderPoint.y / tmpColliderPoint.w, tmpColliderPoint.z / tmpColliderPoint.w);

				if (colliderPoints[i].x > maxX)
					maxX = colliderPoints[i].x;
				if (colliderPoints[i].x < minX)
					minX = colliderPoints[i].x;

				if (colliderPoints[i].y > maxY)
					maxY = colliderPoints[i].y;
				if (colliderPoints[i].y < minY)
					minY = colliderPoints[i].y;
			}


			//is light outside the camera frustum ? 
			if ((maxX < -1 && minX < -1) || (maxY < -1 && minY < -1)
				|| (maxX > 1 && minX > 1) || (maxY > 1 && minY > 1))
			{
				//PointLight* tmpLight = pointLights[i];
				//pointLights[i] = pointLights[lastId];
				//pointLights[lastId] = tmpLight;

				//lastId--;
				//i--;

				//we don't have to draw the light if its bounding box is outside the camera frustum
				insideFrustum = false;
			}

			//we have to draw the light if its boudning box is inside/intersect the camera frustum
			insideFrustum = true;
		}
		//we draw the light if we are inside its bounding box
		insideFrustum = true;

		//second optimisation : we modify the quad verticies such that it is reduce to the area of the light
		if (insideFrustum)
		{
			float width = maxX - minX;
			float height = maxY - minY;
			
			pointLightCullingInfos.push_back(LightCullingInfo(glm::vec4(minX, minY, width, height), i));
		}

	}

	//lastId = spotLights.size() - 1;
	for (int i = 0; i < spotLights.size(); i++)
	{

		BoxCollider& collider = spotLights[i]->boundingBox;
		glm::vec3 topRight = collider.topRight;
		glm::vec3 bottomLeft = collider.bottomLeft;


		if (!(cameraPosition.x > bottomLeft.x && cameraPosition.x < topRight.x && cameraPosition.y > bottomLeft.y && cameraPosition.y < topRight.y && cameraPosition.z > bottomLeft.z && cameraPosition.z < topRight.z))
		{
			//compute 8 points of 3D collider : 

			float colliderWidth = glm::abs(topRight.x - bottomLeft.x);
			float colliderDepth = glm::abs(topRight.z - bottomLeft.z);

			glm::vec3 colliderPoints[8] = { bottomLeft, glm::vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z + colliderDepth), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z), glm::vec3(bottomLeft.x + colliderWidth, bottomLeft.y, bottomLeft.z + colliderDepth),
				topRight, glm::vec3(topRight.x, topRight.y, topRight.z - colliderDepth), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z), glm::vec3(topRight.x - colliderWidth, topRight.y, topRight.z - colliderDepth) };

			glm::mat4 vp = projection * view;
			glm::vec4 tmpColliderPoint;
			maxX = -1;
			maxY = -1;
			minX = 1;
			minY = 1;
			for (int i = 0; i < 8; i++)
			{
				tmpColliderPoint = projection * view * glm::vec4(colliderPoints[i], 1);
				colliderPoints[i] = glm::vec3(tmpColliderPoint.x / tmpColliderPoint.w, tmpColliderPoint.y / tmpColliderPoint.w, tmpColliderPoint.z / tmpColliderPoint.w);

				if (colliderPoints[i].x > maxX)
					maxX = colliderPoints[i].x;
				if (colliderPoints[i].x < minX)
					minX = colliderPoints[i].x;

				if (colliderPoints[i].y > maxY)
					maxY = colliderPoints[i].y;
				if (colliderPoints[i].y < minY)
					minY = colliderPoints[i].y;
			}


			//is light outside the camera frustum ? 
			if ((maxX < -1 && minX < -1) || (maxY < -1 && minY < -1)
				|| (maxX > 1 && minX > 1) || (maxY > 1 && minY > 1))
			{
				//SpotLight* tmpLight = spotLights[i];
				//spotLights[i] = spotLights[lastId];
				//spotLights[lastId] = tmpLight;

				//lastId--;
				//i--;

				//we don't have to draw the light if its bounding box is outside the camera frustum
				insideFrustum = false;
			}

			//we have to draw the light if its boudning box is inside/intersect the camera frustum
			insideFrustum = true;
		}

		//we draw the light if we are inside its bounding box
		insideFrustum = true;

		//second optimisation : we modify the quad verticies such that it is reduce to the area of the light
		if (insideFrustum)
		{
			float width = maxX - minX;
			float height = maxY - minY;

			spotLightCullingInfos.push_back(LightCullingInfo(glm::vec4(minX, minY, width, height), i));
		}
	}

	spotLightCount = spotLightCullingInfos.size();
	pointLightCount = pointLightCullingInfos.size();

}