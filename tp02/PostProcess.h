#pragma once

#include "glew/glew.h"
#include "glm/glm.hpp"

#include "Mesh.h"

class PostProcess
{
private:
	Mesh quadMesh;
	int textureCount;
	GLuint fxFbo;
	GLuint* fxTextures;
	GLuint glProgram_gamma;
	GLuint glProgram_sobel;
	GLuint glProgram_sobelColor;
	GLuint glProgram_blur;

	GLuint uniformGammaValue;
	GLuint uniformGammaTexture;

	GLuint uniformSobelColorMix;
	GLuint uniformSobelColorTexture;

	GLuint uniformSobelTexture;

	GLuint uniformBlurTexture;
	GLuint uniformBlurSampleCount;
	GLuint uniformBlurDirection;

	float gammaValue;
public:
	PostProcess(int _textureCount = 4);
	~PostProcess();
	void render(const glm::mat4& projection, const glm::mat4& view, GLuint beautyTextureId, GLuint depthTextureId);
};
