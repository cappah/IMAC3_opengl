#pragma once

#include "FrameBuffer.h"

namespace GlHelper {

void transferDepth(const GlHelper::Framebuffer& from, GlHelper::Framebuffer& to, const glm::vec2& depthTextureSize);


}