/*
 * LightManager2.cpp
 *
 *  Created on: 23.04.2017
 *      Author: Christoph Neuhauser
 */

#include <GL/glew.h>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Texture/TextureManager.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Input/Keyboard.hpp>
#include <Math/Geometry/MatrixUtil.hpp>
#include "LightManagerVolume.hpp"

LightManagerVolume::LightManagerVolume(CameraPtr _camera)
{
	camera = _camera;
	sceneTarget = RenderTargetPtr(new RenderTarget());
	lightTarget = RenderTargetPtr(new RenderTarget());
	lightTempTarget = RenderTargetPtr(new RenderTarget());
	multisampling = false;
	lightCombineShader = ShaderManager->getShaderProgram({"LightMix.Vertex", "LightMix.Fragment"});
	lightCombineShader->setUniform("ambientLight", Color(50, 50, 50));
	edgeShader = ShaderManager->getShaderProgram({"VolumeLight.Vertex", "VolumeLight.Geometry", "VolumeLight.Fragment"});
	onResolutionChanged();
}

VolumeLightPtr LightManagerVolume::addLight(const glm::vec2 &pos, float rad, const Color &col)
{
	VolumeLightPtr light(new VolumeLight(pos, rad, col));
	lights.push_back(light);
	return light;
}

void LightManagerVolume::onResolutionChanged()
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
	if (multisampling) {
		lightRenderTex = TextureManager->createMultisampledTexture(window->getWidth(), window->getHeight(), 8);
	} else {
		lightRenderTex = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
	}
	lightFBO->bindTexture(lightRenderTex);
	lightTarget->bindFramebufferObject(lightFBO);

	lightTempFBO = Renderer->createFBO();
	lightTempTex = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
	lightTempFBO->bindTexture(lightTempTex);
	lightTempTarget->bindFramebufferObject(lightTempFBO);
}

void LightManagerVolume::beginRenderScene()
{
	camera->setRenderTarget(sceneTarget);
	sceneTarget->bindRenderTarget();
	Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, Color(242, 242, 242));

	// Now render scene (user)
}

void LightManagerVolume::endRenderScene()
{
	Renderer->unbindFBO();
	sceneTex = Renderer->resolveMultisampledTexture(sceneRenderTex);
	Renderer->unbindFBO();
}


void LightManagerVolume::renderLightmap(function<void()> renderfun) {
	lightTempTarget->bindRenderTarget();
	Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, Color(0, 0, 0));

	for (VolumeLightPtr &light : lights) {
		lightTarget->bindRenderTarget();
		Renderer->setBlendMode(BLEND_SUBTRACTIVE);
		Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, light->getColor());
		edgeShader->setUniform("lightpos", light->position);
		renderfun();

		lightTempTarget->bindRenderTarget();
		Renderer->setBlendMode(BLEND_ADDITIVE);
		Renderer->setProjectionMatrix(matrixIdentity());
		Renderer->setViewMatrix(matrixIdentity());
		Renderer->setModelMatrix(matrixIdentity());
		Renderer->blitTexture(lightRenderTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
	}
	Renderer->setBlendMode(BLEND_ALPHA);
}

void LightManagerVolume::beginRenderLightmap()
{
	camera->setRenderTarget(lightTarget);
}

void LightManagerVolume::endRenderLightmap()
{
	Renderer->setBlendMode(BLEND_ALPHA);
	Renderer->unbindFBO();

	//lightTex = Renderer->resolveMultisampledTexture(lightRenderTex);
	bool blur = false;
	bool fxaa = false;
	if (blur) {
		Renderer->blurTexture(lightTempTex);
	}

	if (fxaa) {
		Window *window = AppSettings::get()->getMainWindow();
		TexturePtr texFXAA = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
		FramebufferObjectPtr fboFXAA = Renderer->createFBO();
		fboFXAA->bindTexture(texFXAA);
		Renderer->bindFBO(fboFXAA);
		Renderer->blitTextureFXAAAntialiased(lightTempTex);
		lightTempTex = texFXAA;
		lightTempFBO->bindTexture(lightTempTex);
	}

	Renderer->unbindFBO();
}

void LightManagerVolume::blitMixSceneAndLights()
{
	Renderer->setProjectionMatrix(matrixIdentity());
	Renderer->setViewMatrix(matrixIdentity());
	Renderer->setModelMatrix(matrixIdentity());

	if (Keyboard->isKeyDown(SDLK_s)) {
		Renderer->blitTexture(sceneTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
	} else if (Keyboard->isKeyDown(SDLK_d)) {
		Renderer->blitTexture(lightTempTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
	} else {
		lightCombineShader->setUniform("lightTexture", lightTempTex, 1);
		Renderer->blitTexture(sceneTex, AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)), lightCombineShader);
	}
}
