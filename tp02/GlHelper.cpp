#include "GlHelper.h"


namespace GlHelper {

void transferDepth(const GlHelper::Framebuffer& from, GlHelper::Framebuffer& to, const glm::vec2& depthTextureSize)
{
	from.bind(GL_READ_FRAMEBUFFER);
	to.bind(GL_DRAW_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, depthTextureSize.x, depthTextureSize.y, 0, 0, depthTextureSize.x, depthTextureSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



}