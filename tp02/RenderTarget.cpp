#include "RenderTarget.h"
#include "Texture.h"
#include "FrameBuffer.h"

RenderTarget::RenderTarget(float width, float height)
	: m_size(width, height)
{
	// Setup texture
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();
	// Setup framebuffer
	m_frameBuffer.bind();
		m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

const glm::vec2 & RenderTarget::getSize() const
{
	return m_size;
}

void RenderTarget::setSize(const glm::vec2 & size)
{
	m_size = size;
	onResize();
}

void RenderTarget::onResize()
{
	m_frameBuffer.bind();
	m_frameBuffer.detachTexture(GL_COLOR_ATTACHMENT0);

	m_texture.freeGL();
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();

	m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

void RenderTarget::bindFramebuffer()
{
	m_frameBuffer.bind();
}

void RenderTarget::unbindFramebuffer()
{
	m_frameBuffer.unbind();
}

const GlHelper::Framebuffer & RenderTarget::getFrameBuffer() const
{
	return m_frameBuffer;
}

GLuint RenderTarget::getFinalFrame() const
{
	return m_texture.glId;
}

///////////////////////////////////////////////

RenderTargetWithDepth::RenderTargetWithDepth(float width, float height)
	: RenderTarget(width, height)
	, m_depthBuffer(width, height, GL_DEPTH_COMPONENT24)
{
	// Setup texture
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();
	// Setup framebuffer
	m_frameBuffer.bind();
	m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.attachRenderBuffer(&m_depthBuffer, GL_DEPTH_ATTACHMENT);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

void RenderTargetWithDepth::onResize()
{
	m_frameBuffer.bind();
	m_frameBuffer.detachTexture(GL_COLOR_ATTACHMENT0);

	m_texture.freeGL();
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();

	m_frameBuffer.detachRenderBuffer(GL_DEPTH_ATTACHMENT);
	m_depthBuffer.resize(m_size.x, m_size.y);
	m_frameBuffer.attachRenderBuffer(&m_depthBuffer, GL_DEPTH_ATTACHMENT);

	m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

RenderTargetWithStencil::RenderTargetWithStencil(float width, float height)
	: RenderTarget(width, height)
	, m_stencilBuffer(width, height, GL_STENCIL_INDEX8)
{
	// Setup texture
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();
	// Setup framebuffer
	m_frameBuffer.bind();
		m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
		m_frameBuffer.attachRenderBuffer(&m_stencilBuffer, GL_STENCIL_ATTACHMENT);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

void RenderTargetWithStencil::onResize()
{
	m_frameBuffer.bind();
	m_frameBuffer.detachTexture(GL_COLOR_ATTACHMENT0);

	m_texture.freeGL();
	GlHelper::makeFloatColorTexture(m_texture, m_size.x, m_size.y);
	m_texture.initGL();

	m_frameBuffer.detachRenderBuffer(GL_STENCIL_ATTACHMENT);
	m_stencilBuffer.resize(m_size.x, m_size.y);
	m_frameBuffer.attachRenderBuffer(&m_stencilBuffer, GL_STENCIL_ATTACHMENT);

	m_frameBuffer.attachTexture(&m_texture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}
