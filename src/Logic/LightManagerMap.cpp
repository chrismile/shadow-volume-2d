/*
 * LightManager2.cpp
 *
 *  Created on: 23.04.2017
 *      Author: christoph
 */

#include <vector>
#include <GL/glew.h>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Texture/TextureManager.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Input/Keyboard.hpp>
#include <Math/Math.hpp>
#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/OpenGL/Texture.hpp>
#include <Graphics/Mesh/Vertex.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include "LightManagerMap.hpp"

const float LIGHT_FAR_PLANE_DIST = 10.0f;

GLuint createShadowmapTex(int res) {
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT16, res, 1, 3);
	return texture;
}

ShaderAttributesPtr createFullscreenQuadRenderData(ShaderProgramPtr shader, AABB2 sceneRect) {
	// Set up the vertex data of the rectangle
	std::vector<glm::vec2> fullscreenQuad{
		glm::vec2(sceneRect.max.x, sceneRect.max.y),
		glm::vec2(sceneRect.min.x, sceneRect.min.y),
		glm::vec2(sceneRect.max.x, sceneRect.min.y),
		glm::vec2(sceneRect.min.x, sceneRect.min.y),
		glm::vec2(sceneRect.max.x, sceneRect.max.y),
		glm::vec2(sceneRect.min.x, sceneRect.max.y)};

	// Feed the shader with the data
	GeometryBufferPtr geomBuffer = Renderer->createGeometryBuffer(sizeof(glm::vec2)*fullscreenQuad.size(), &fullscreenQuad.front());
	ShaderAttributesPtr shaderAttributes = ShaderManager->createShaderAttributes(shader);
	shaderAttributes->addGeometryBuffer(geomBuffer, "position", ATTRIB_FLOAT, 2);
	return shaderAttributes;
}

LightManagerMap::LightManagerMap(CameraPtr _camera)
{
	camera = _camera;
	sceneTarget = RenderTargetPtr(new RenderTarget());
	lightTarget = RenderTargetPtr(new RenderTarget());
	shadowmapTarget = RenderTargetPtr(new RenderTarget());
	multisampling = false;
	lightCombineShader = ShaderManager->getShaderProgram({"LightMix.Vertex", "LightMix.Fragment"});
	lightCombineShader->setUniform("ambientLight", Color(50, 50, 50));
	shadowmapShader = ShaderManager->getShaderProgram({"ShadowMapVolume.Vertex",
			"ShadowMapVolume.Geometry", "ShadowMapVolume.Fragment"});
	shadowMapRenderShader = ShaderManager->getShaderProgram({"ShadowMapRender.Vertex",
		"ShadowMapRender.Fragment"});
	onResolutionChanged();

	// The three light cams look in three directions with 120° angles inbetween
	glm::vec3 lightcamLookDir[3];
	lightcamLookDir[0] = glm::vec3(0.0f, 1.0f, 0.0f);
	lightcamLookDir[1] = glm::vec3(-sqrtf(3.0f)/2.0f, -0.5f, 0.0f);
	lightcamLookDir[2] = glm::vec3(sqrtf(3.0f)/2.0f, -0.5f, 0.0f);
	for (int i = 0; i < 3; ++i) {
		lightcamProj[i] = glm::perspective(2.0f*sgl::PI/3.0f, 1.0f, 0.01f, LIGHT_FAR_PLANE_DIST);
		glm::vec3 eyepos(0.0f, 0.0f, 0.0f);
		lightcamView[i] = glm::lookAt(eyepos, eyepos+lightcamLookDir[i], glm::vec3(0.0f, 0.0f, 1.0f)); // eye, center, up
	}
	shadowmapShader->setUniform("farPlaneDist", LIGHT_FAR_PLANE_DIST);
	shadowMapRenderShader->setUniform("farPlaneDist", LIGHT_FAR_PLANE_DIST);
}




/*glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);
glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, lightTex, 0);*/

VolumeLightPtr LightManagerMap::addLight(const glm::vec2 &pos, float rad, const Color &col)
{
	VolumeLightPtr light(new VolumeLight(pos, rad, col));
	lights.push_back(light);
	onResolutionChanged();
	return light;
}

void LightManagerMap::onResolutionChanged()
{
	Window *window = AppSettings::get()->getMainWindow();

	sceneFBO = Renderer->createFBO();
	if (multisampling) {
		sceneRenderTex = TextureManager->createMultisampledTexture(window->getWidth(), window->getHeight(), 8);
	} else {
		sceneRenderTex = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
	}
	sceneFBO->bindTexture(sceneRenderTex);
	sceneTarget->bindFramebufferObject(sceneFBO);

	lightFBO = Renderer->createFBO();
	lightTex = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
	lightFBO->bindTexture(lightTex);
	lightTarget->bindFramebufferObject(lightFBO);

	// Create shadow map
	shadowMapWidth = 2048;//window->getWidth(); // TODO
	TextureGL *texGL = new TextureGL(createShadowmapTex(shadowMapWidth), shadowMapWidth, 1, 16,
			GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	shadowmap = TexturePtr(texGL);
	texGL->setTextureType(TEXTURE_3D);
	shadowmapFBO = Renderer->createFBO();
	shadowmapFBO->bindTexture(shadowmap, DEPTH_ATTACHMENT);
	shadowmapTarget->bindFramebufferObject(shadowmapFBO);

	AABB2 camRect = camera->getAABB2(0.0f);
	shadowmapRenderAttributes = createFullscreenQuadRenderData(shadowMapRenderShader, camRect);
}

void LightManagerMap::beginRenderScene()
{
	camera->setRenderTarget(sceneTarget);
	sceneTarget->bindRenderTarget();
	Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, Color(242, 242, 242));

	// Now render scene (user)
}

void LightManagerMap::endRenderScene()
{
	Renderer->unbindFBO();

	sceneTex = Renderer->resolveMultisampledTexture(sceneRenderTex);

	/*TexturePtr texFXAA = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
	FramebufferObjectPtr fboFXAA = Renderer->createFBO();
	fboFXAA->bindTexture(texFXAA);
	Renderer->bindFBO(fboFXAA);
	Renderer->blitTextureFXAAAntialiased(tex);
	tex = texFXAA;*/

	Renderer->unbindFBO();
}


void LightManagerMap::renderLightmap(function<void()> renderfun) {
	for (VolumeLightPtr &light : lights) {
		Renderer->setBlendMode(BLEND_ALPHA);
		Renderer->setViewMatrix(camera->getViewMatrix());
		Renderer->setProjectionMatrix(camera->getProjectionMatrix());
		shadowmapTarget->bindRenderTarget();
		Renderer->clearFramebuffer(GL_DEPTH_BUFFER_BIT, light->getColor(), 1.0f);
		shadowmapShader->setUniform("lightpos", light->getPosition());
		int matUniformLoc = shadowmapShader->getUniformLoc("camViewProjMatrices");
		for (int i = 0; i < 3; ++i) {
			shadowmapShader->setUniform(matUniformLoc+i, lightcamProj[i]*lightcamView[i]*matrixTranslation(-light->getPosition()));
		}
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glViewport(0,0,shadowMapWidth,1);
		renderfun();
		glDisable(GL_DEPTH_TEST);
		Window *window = AppSettings::get()->getMainWindow();
		glViewport(0,0,window->getWidth(),window->getHeight());

		lightTarget->bindRenderTarget();
		Renderer->setBlendMode(BLEND_ADDITIVE);
		Renderer->setViewMatrix(camera->getViewMatrix());
		Renderer->setProjectionMatrix(camera->getProjectionMatrix());
		Renderer->setModelMatrix(matrixIdentity());
		shadowMapRenderShader->setUniform("lightpos", light->getPosition());
		shadowMapRenderShader->setUniform("lightColor", light->getColor());
		Renderer->render(shadowmapRenderAttributes);
	}
	Renderer->setBlendMode(BLEND_ALPHA);
}

void LightManagerMap::beginRenderLightmap()
{
	camera->setRenderTarget(lightTarget);
	lightTarget->bindRenderTarget();
	Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, Color(0, 0, 0));
}

void LightManagerMap::endRenderLightmap()
{
	Renderer->setBlendMode(BLEND_ALPHA);
	Renderer->unbindFBO();
}

void LightManagerMap::blitMixSceneAndLights()
{
	Renderer->setProjectionMatrix(matrixIdentity());
	Renderer->setViewMatrix(matrixIdentity());
	Renderer->setModelMatrix(matrixIdentity());

	if (Keyboard->isKeyDown(SDLK_s)) {
		Renderer->blitTexture(sceneTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
	} else if (Keyboard->isKeyDown(SDLK_d)) {
		Renderer->blitTexture(lightTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
	} else {
		lightCombineShader->setUniform("lightTexture", lightTex, 1);
		Renderer->blitTexture(sceneTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)), lightCombineShader);
	}
}
