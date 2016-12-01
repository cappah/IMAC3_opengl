#pragma once

#include <map>
#include "Texture.h"

namespace GlHelper {

class Renderbuffer
{

private:
	GLuint m_glId;
	std::map<GLenum, const Texture*> m_attachedTextures;

public:

	Renderbuffer(float width, float height, GLenum internalFormat)
	{
		glGenRenderbuffers(1, &m_glId);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
	}

	~Renderbuffer()
	{
		glDeleteRenderbuffers(1, &m_glId);
	}

	GLuint getId() const
	{
		return m_glId;
	}

	void bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_glId);
	}
};


class Framebuffer 
{
public:
	enum AttachmentTypes {
		COLOR = GL_COLOR_ATTACHMENT0,
		DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT,
		DEPTH = GL_DEPTH_ATTACHMENT,
		STENCIL = GL_STENCIL_ATTACHMENT
	};

private:
	GLuint m_glId;
	std::map<GLenum, const Texture*> m_attachedTextures;
	std::map<GLenum, const Renderbuffer*> m_attachedRenderbuffers;

public:

	Framebuffer()
	{
		glGenFramebuffers(1, &m_glId);
	}

	~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_glId);
	}

	GLuint getId() const
	{
		return m_glId;
	}

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_glId);
	}

	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void attachTexture(const Texture* textureToAttach, AttachmentTypes attachment, int mipMapLevel = 0, int colorAttachmentOffset = 0)
	{
		GLenum attachementType = (attachment == AttachmentTypes::COLOR) ? (GLenum)(attachment + colorAttachmentOffset) : (GLenum)attachment;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachementType, GL_TEXTURE_2D, textureToAttach->glId, mipMapLevel);

		m_attachedTextures[attachementType] = textureToAttach;
	}

	void attachRenderBuffer(const Renderbuffer* renderBufferToAttach, AttachmentTypes attachmentType)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderBufferToAttach->getId());

		m_attachedRenderbuffers[attachmentType] = renderBufferToAttach;
	}

	const Texture* getAttachedTexture(GLenum attachmentType) const
	{
		auto& found = m_attachedTextures.find(attachmentType);
		if (found != m_attachedTextures.end())
			return found->second;
		else
			return nullptr;
	}

};

}