/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2017-2020, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <GL/glew.h>

#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Texture/TextureManager.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Input/Keyboard.hpp>
#include <Math/Geometry/MatrixUtil.hpp>
#include <ImGui/ImGuiWrapper.hpp>

#include "LightManagerVolume.hpp"

LightManagerVolume::LightManagerVolume(sgl::CameraPtr _camera) {
    camera = _camera;
    sceneTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    lightTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    lightTempTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    lightCombineShader = sgl::ShaderManager->getShaderProgram(
            {"LightMix.Vertex", "LightMix.Fragment"});
    lightCombineShader->setUniform("ambientLight", sgl::Color(50, 50, 50));
    edgeShader = sgl::ShaderManager->getShaderProgram(
            {"VolumeLight.Vertex", "VolumeLight.Geometry", "VolumeLight.Fragment"});
    onResolutionChanged();
}

static bool multisampling = false;

void LightManagerVolume::renderGUI() {
    ImGui::Separator();

    if (ImGui::Checkbox("Multisampling", &multisampling)) {
        onResolutionChanged();
    }
}


VolumeLightPtr LightManagerVolume::addLight(const glm::vec2 &pos, float rad, const sgl::Color &col) {
    VolumeLightPtr light(new VolumeLight(pos, rad, col));
    lights.push_back(light);
    return light;
}

void LightManagerVolume::onResolutionChanged() {
    sgl::Window *window = sgl::AppSettings::get()->getMainWindow();

    sceneFBO = sgl::Renderer->createFBO();
    if (multisampling) {
        sceneRenderTex = sgl::TextureManager->createMultisampledTexture(window->getWidth(), window->getHeight(), 8);
    } else {
        sceneRenderTex = sgl::TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
    }
    sceneFBO->bindTexture(sceneRenderTex);
    sceneTarget->bindFramebufferObject(sceneFBO);

    lightFBO = sgl::Renderer->createFBO();
    if (multisampling) {
        lightRenderTex = sgl::TextureManager->createMultisampledTexture(window->getWidth(), window->getHeight(), 8);
    } else {
        lightRenderTex = sgl::TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
    }
    lightFBO->bindTexture(lightRenderTex);
    lightTarget->bindFramebufferObject(lightFBO);

    lightTempFBO = sgl::Renderer->createFBO();
    lightTempTex = sgl::TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
    lightTempFBO->bindTexture(lightTempTex);
    lightTempTarget->bindFramebufferObject(lightTempFBO);
}

void LightManagerVolume::beginRenderScene() {
    camera->setRenderTarget(sceneTarget);
    sceneTarget->bindRenderTarget();
    sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, sgl::Color(242, 242, 242));

    // Now render scene (user)
}

void LightManagerVolume::endRenderScene() {
    sgl::Renderer->unbindFBO();
    sceneTex = sgl::Renderer->resolveMultisampledTexture(sceneRenderTex);
    sgl::Renderer->unbindFBO();
}


void LightManagerVolume::renderLightmap(std::function<void()> renderfun) {
    lightTempTarget->bindRenderTarget();
    sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, sgl::Color(0, 0, 0));

    for (VolumeLightPtr &light : lights) {
        lightTarget->bindRenderTarget();
        sgl::Renderer->setBlendMode(sgl::BLEND_SUBTRACTIVE);
        sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, light->getColor());
        edgeShader->setUniform("lightpos", light->position);
        renderfun();

        lightTempTarget->bindRenderTarget();
        sgl::Renderer->setBlendMode(sgl::BLEND_ADDITIVE);
        sgl::Renderer->setProjectionMatrix(sgl::matrixIdentity());
        sgl::Renderer->setViewMatrix(sgl::matrixIdentity());
        sgl::Renderer->setModelMatrix(sgl::matrixIdentity());
        sgl::Renderer->blitTexture(
                lightRenderTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
    }
    sgl::Renderer->setBlendMode(sgl::BLEND_ALPHA);
}

void LightManagerVolume::beginRenderLightmap() {
    camera->setRenderTarget(lightTarget);
}

void LightManagerVolume::endRenderLightmap() {
    sgl::Renderer->setBlendMode(sgl::BLEND_ALPHA);
    sgl::Renderer->unbindFBO();

    //lightTex = sgl::Renderer->resolveMultisampledTexture(lightRenderTex);
    bool blur = false;
    bool fxaa = false;
    if (blur) {
        sgl::Renderer->blurTexture(lightTempTex);
    }

    if (fxaa) {
        sgl::Window *window = sgl::AppSettings::get()->getMainWindow();
        sgl::TexturePtr texFXAA = sgl::TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
        sgl::FramebufferObjectPtr fboFXAA = sgl::Renderer->createFBO();
        fboFXAA->bindTexture(texFXAA);
        sgl::Renderer->bindFBO(fboFXAA);
        sgl::Renderer->blitTextureFXAAAntialiased(lightTempTex);
        lightTempTex = texFXAA;
        lightTempFBO->bindTexture(lightTempTex);
    }

    sgl::Renderer->unbindFBO();
}

void LightManagerVolume::blitMixSceneAndLights() {
    sgl::Renderer->setProjectionMatrix(sgl::matrixIdentity());
    sgl::Renderer->setViewMatrix(sgl::matrixIdentity());
    sgl::Renderer->setModelMatrix(sgl::matrixIdentity());

    if (sgl::Keyboard->isKeyDown(SDLK_s)) {
        sgl::Renderer->blitTexture(
                sceneTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
    } else if (sgl::Keyboard->isKeyDown(SDLK_d)) {
        sgl::Renderer->blitTexture(
                lightTempTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
    } else {
        lightCombineShader->setUniform("lightTexture", lightTempTex, 1);
        sgl::Renderer->blitTexture(
                sceneTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)), lightCombineShader);
    }
}
