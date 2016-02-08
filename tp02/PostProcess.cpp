#include "PostProcess.h"
//forward 
#include "Renderer.h"

PostProcess::PostProcess(int _textureCount) : quadMesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES), 2), gammaValue(3), textureCount(_textureCount)
{
	fxTextures = new GLuint[textureCount];

	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();

	// Create Fx Framebuffer Object
	GLuint fxDrawBuffers[1];
	glGenFramebuffers(1, &fxFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
	fxDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, fxDrawBuffers);

	// Create Fx textures
	glGenTextures(textureCount, fxTextures);
	for (int i = 0; i < textureCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, fxTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxTextures[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building framebuffern");
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//////////////////// gamma correction shader  ////////////////////////

	GLuint vertShaderId_gamma = compile_shader_from_file(GL_VERTEX_SHADER, "blit.vert");
	GLuint fragShaderId_gamma = compile_shader_from_file(GL_FRAGMENT_SHADER, "gamma.frag");

	glProgram_gamma = glCreateProgram();
	glAttachShader(glProgram_gamma, vertShaderId_gamma);
	glAttachShader(glProgram_gamma, fragShaderId_gamma);

	glLinkProgram(glProgram_gamma);
	if (check_link_error(glProgram_gamma) < 0)
		exit(1);

	uniformGammaValue = glGetUniformLocation(glProgram_gamma, "Gamma");
	uniformGammaTexture = glGetUniformLocation(glProgram_gamma, "Texture");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	
	///////////////////// sobel shader :  ////////////////////////

	GLuint vertShaderId_sobel = compile_shader_from_file(GL_VERTEX_SHADER, "blit.vert");
	GLuint fragShaderId_sobel = compile_shader_from_file(GL_FRAGMENT_SHADER, "sobel.frag");

	glProgram_sobel = glCreateProgram();
	glAttachShader(glProgram_sobel, vertShaderId_sobel);
	glAttachShader(glProgram_sobel, fragShaderId_sobel);

	glLinkProgram(glProgram_sobel);
	if (check_link_error(glProgram_sobel) < 0)
		exit(1);

	uniformSobelTexture = glGetUniformLocation(glProgram_sobel, "Texture");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	///////////////////// sobel color shader :  ////////////////////////

	GLuint vertShaderId_sobelColor = compile_shader_from_file(GL_VERTEX_SHADER, "blit.vert");
	GLuint fragShaderId_sobelColor = compile_shader_from_file(GL_FRAGMENT_SHADER, "sobelColor.frag");

	glProgram_sobelColor = glCreateProgram();
	glAttachShader(glProgram_sobelColor, vertShaderId_sobelColor);
	glAttachShader(glProgram_sobelColor, fragShaderId_sobelColor);

	glLinkProgram(glProgram_sobelColor);
	if (check_link_error(glProgram_sobelColor) < 0)
		exit(1);

	uniformSobelColorTexture = glGetUniformLocation(glProgram_sobelColor, "Texture");
	uniformSobelColorMix= glGetUniformLocation(glProgram_sobelColor, "MixColor");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	///////////////////// blur shader :  ////////////////////////

	GLuint vertShaderId_blur = compile_shader_from_file(GL_VERTEX_SHADER, "blit.vert");
	GLuint fragShaderId_blur = compile_shader_from_file(GL_FRAGMENT_SHADER, "blur.frag");

	glProgram_blur = glCreateProgram();
	glAttachShader(glProgram_blur, vertShaderId_blur);
	glAttachShader(glProgram_blur, fragShaderId_blur);

	glLinkProgram(glProgram_blur);
	if (check_link_error(glProgram_blur) < 0)
		exit(1);

	uniformBlurTexture = glGetUniformLocation(glProgram_blur, "Texture");
	uniformBlurSampleCount = glGetUniformLocation(glProgram_blur, "SampleCount");
	uniformBlurDirection = glGetUniformLocation(glProgram_blur, "Direction");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	////////////////// QUAD MESH ///////////////////////
	////////////////////// INIT QUAD MESH ////////////////////////
	quadMesh.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	quadMesh.vertices = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
	quadMesh.initGl();
}

PostProcess::~PostProcess()
{
	glDeleteTextures(textureCount, fxTextures);
	glDeleteFramebuffers(1, &fxFbo);
}

void PostProcess::render(const glm::mat4 & projection, const glm::mat4 & view, GLuint beautyTextureId, GLuint depthTextureId)
{
	int width = Application::get().getWindowWidth(), height = Application::get().getWindowHeight();

	// Full screen viewport
	glViewport(0, 0, width, height);
	// Disable depth test
	glDisable(GL_DEPTH_TEST);

	////SOBEL : 

	//glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxTextures[0], 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	//glUseProgram(glProgram_sobel);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, beautyTextureId);

	//glUniform1i(uniformSobelTexture, 0);

	//quadMesh.draw();

	//SOBEL COLOR : 

	glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxTextures[0], 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(glProgram_sobelColor);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, beautyTextureId);

	glUniform1i(uniformSobelColorTexture, 0);
	glUniform1f(uniformSobelColorMix, .5f);

	quadMesh.draw();

	//BLUR : 
	// Use blur program shader
	glUseProgram(glProgram_blur);

	// Write into Vertical Blur Texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxTextures[1], 0);
	// Clear the content of texture
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	// Read the texture processed by the Sobel operator
	glBindTexture(GL_TEXTURE_2D, beautyTextureId);

	glUniform1i(uniformBlurTexture, 0);
	glUniform2f(uniformBlurDirection, 3, 3);
	glUniform1i(uniformBlurSampleCount, 3);

	quadMesh.draw();

	// Write into Horizontal Blur Texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxTextures[0], 0);
	// Clear the content of texture
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	// Read the texture processed by the Vertical Blur
	glBindTexture(GL_TEXTURE_2D, fxTextures[1]);

	glUniform1i(uniformBlurTexture, 0);
	glUniform2f(uniformBlurDirection, 3, 3);
	glUniform1i(uniformBlurSampleCount, 3);

	quadMesh.draw();


	//GAMMA : 
	
	// Default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Clear default framebuffer color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Gamma
	glUseProgram(glProgram_gamma);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fxTextures[0]);

	glUniform1i(uniformGammaTexture, 0);
	glUniform1f(uniformGammaValue, gammaValue);

	quadMesh.draw();
	

}
