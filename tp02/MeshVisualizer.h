#pragma once

#include <memory>
#include "FrameBuffer.h"
#include "Camera.h"

class Material;
class SubMesh;
class Renderer;

class MeshVisualizer
{

private:

	// For Mesh Vizualizer
	GlHelper::Framebuffer m_frameBuffer;
	Texture m_renderTex;
	SimpleCamera m_camera;
	std::shared_ptr<SubMesh> m_visualizedMesh;
	//Resource_ptr<Mesh> m_visualizedMesh;
	glm::vec3 m_cameraTarget;
	float m_cameraPhi;
	float m_cameraTheta;
	std::shared_ptr<Material> m_materialInstance;
	std::vector<PointLight*> m_pointLights;
	Rendering::PipelineType m_pipelineType;
	bool m_isSelected;

public:

	MeshVisualizer();
	~MeshVisualizer();

	// For Mesh Visualizer
	void setMesh(Mesh * mesh);
	void setMaterial(std::shared_ptr<Material> material);
	void render(Renderer& renderer);
	void rotateCamera(float x, float y);
	void panCamera(float x, float y);
	void drawUI();
};