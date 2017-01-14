#pragma once

#include <glm/common.hpp>

#include "FrameBuffer.h"
class Mesh;
class MaterialBlit;
class Texture;

class RenderTarget
{
protected:
	glm::vec2 m_size;

	GlHelper::Framebuffer m_frameBuffer;
	Texture m_texture;

public:
	RenderTarget(float width = 400, float height = 400);

	const glm::vec2& getSize() const;
	void setSize(const glm::vec2& size);

	virtual void onResize();

	void bindFramebuffer();
	void unbindFramebuffer();
	const GlHelper::Framebuffer& getFrameBuffer() const;

	GLuint getFinalFrame() const;
};

class RenderTargetWithDepth final : public RenderTarget
{
private:
	GlHelper::Renderbuffer m_depthBuffer;

public:
	RenderTargetWithDepth(float width = 400, float height = 400);
	void onResize() override;
};

class RenderTargetWithStencil final : public RenderTarget
{
private:
	GlHelper::Renderbuffer m_stencilBuffer;

public:
	RenderTargetWithStencil(float width = 400, float height = 400);
	void onResize() override;
};