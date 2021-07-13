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
#include <ImGui/ImGuiWrapper.hpp>

#include "LightManagerMap.hpp"

const float LIGHT_FAR_PLANE_DIST = 10.0f;
static int depthFormat = GL_DEPTH_COMPONENT16;

GLuint createShadowmapTex(int res) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, depthFormat, res, 1, 3);
    return texture;
}

sgl::ShaderAttributesPtr createFullscreenQuadRenderData(sgl::ShaderProgramPtr shader, sgl::AABB2 sceneRect) {
    // Set up the vertex data of the rectangle
    std::vector<glm::vec2> fullscreenQuad{
        glm::vec2(sceneRect.max.x, sceneRect.max.y),
        glm::vec2(sceneRect.min.x, sceneRect.min.y),
        glm::vec2(sceneRect.max.x, sceneRect.min.y),
        glm::vec2(sceneRect.min.x, sceneRect.min.y),
        glm::vec2(sceneRect.max.x, sceneRect.max.y),
        glm::vec2(sceneRect.min.x, sceneRect.max.y)};

    // Feed the shader with the data
    sgl::GeometryBufferPtr geomBuffer = sgl::Renderer->createGeometryBuffer(
            sizeof(glm::vec2)*fullscreenQuad.size(), &fullscreenQuad.front());
    sgl::ShaderAttributesPtr shaderAttributes = sgl::ShaderManager->createShaderAttributes(shader);
    shaderAttributes->addGeometryBuffer(geomBuffer, "position", sgl::ATTRIB_FLOAT, 2);
    return shaderAttributes;
}

LightManagerMap::LightManagerMap(sgl::CameraPtr _camera) {
    camera = _camera;
    sceneTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    lightTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    shadowmapTarget = sgl::RenderTargetPtr(new sgl::RenderTarget());
    lightCombineShader = sgl::ShaderManager->getShaderProgram({"LightMix.Vertex", "LightMix.Fragment"});
    lightCombineShader->setUniform("ambientLight", sgl::Color(50, 50, 50));
    shadowmapShader = sgl::ShaderManager->getShaderProgram({"ShadowMapVolume.Vertex",
            "ShadowMapVolume.Geometry", "ShadowMapVolume.Fragment"});
    shadowMapRenderShader = sgl::ShaderManager->getShaderProgram({"ShadowMapRender.Vertex",
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


static int shadowMapWidth = 2048;
static bool multisampling = false;
static int depthFormatIndex = 0;

void LightManagerMap::renderGUI() {
    ImGui::Separator();

    ImGui::Text("Shadow Map Resolution:");
    if (ImGui::SliderInt("pixels", &shadowMapWidth, 16, 4096)) {
        onResolutionChanged();
    }

    if (ImGui::Checkbox("Multisampling", &multisampling)) {
        onResolutionChanged();
    }

    const char *depthFormatNames[] = {"UNORM 16-bit", "UNORM 24-bit", "UNORM 32-bit", "Float 32-bit"};
    if (ImGui::Combo("Precision", (int*)&depthFormatIndex, depthFormatNames, IM_ARRAYSIZE(depthFormatNames))) {
        if (depthFormatIndex == 0) {
            depthFormat = GL_DEPTH_COMPONENT16;
        } else if (depthFormatIndex == 1) {
            depthFormat = GL_DEPTH_COMPONENT24;
        } else if (depthFormatIndex == 2) {
            depthFormat = GL_DEPTH_COMPONENT32;
        } else if (depthFormatIndex == 3) {
            depthFormat = GL_DEPTH_COMPONENT32F;
        }
        onResolutionChanged();
    }
}



VolumeLightPtr LightManagerMap::addLight(const glm::vec2 &pos, float rad, const sgl::Color &col) {
    VolumeLightPtr light(new VolumeLight(pos, rad, col));
    lights.push_back(light);
    onResolutionChanged();
    return light;
}

void LightManagerMap::onResolutionChanged() {
    sgl::Window *window = sgl::AppSettings::get()->getMainWindow();

    sceneFBO = sgl::Renderer->createFBO();
    if (multisampling) {
        sceneRenderTex = sgl::TextureManager->createMultisampledTexture(
                window->getWidth(), window->getHeight(), 8);
    } else {
        sceneRenderTex = sgl::TextureManager->createEmptyTexture(
                window->getWidth(), window->getHeight());
    }
    sceneFBO->bindTexture(sceneRenderTex);
    sceneTarget->bindFramebufferObject(sceneFBO);

    lightFBO = sgl::Renderer->createFBO();
    lightTex = sgl::TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
    lightFBO->bindTexture(lightTex);
    lightTarget->bindFramebufferObject(lightFBO);

    // Create shadow map
    sgl::TextureSettings settings(
            sgl::TEXTURE_2D_ARRAY, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    settings.internalFormat = depthFormat;
    sgl::TextureGL *texGL = new sgl::TextureGL(
            createShadowmapTex(shadowMapWidth), shadowMapWidth, 1, 16, settings);
    shadowmap = sgl::TexturePtr(texGL);
    shadowmapFBO = sgl::Renderer->createFBO();
    shadowmapFBO->bindTexture(shadowmap, sgl::DEPTH_ATTACHMENT);
    shadowmapTarget->bindFramebufferObject(shadowmapFBO);

    sgl::AABB2 camRect = camera->getAABB2(0.0f);
    shadowmapRenderAttributes = createFullscreenQuadRenderData(shadowMapRenderShader, camRect);
}

void LightManagerMap::beginRenderScene() {
    camera->setRenderTarget(sceneTarget);
    sceneTarget->bindRenderTarget();
    sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, sgl::Color(242, 242, 242));

    // Now render scene (user)
}

void LightManagerMap::endRenderScene() {
    sgl::Renderer->unbindFBO();

    sceneTex = sgl::Renderer->resolveMultisampledTexture(sceneRenderTex);

    /*TexturePtr texFXAA = TextureManager->createEmptyTexture(window->getWidth(), window->getHeight());
    FramebufferObjectPtr fboFXAA = Renderer->createFBO();
    fboFXAA->bindTexture(texFXAA);
    Renderer->bindFBO(fboFXAA);
    Renderer->blitTextureFXAAAntialiased(tex);
    tex = texFXAA;*/

    sgl::Renderer->unbindFBO();
}


void LightManagerMap::renderLightmap(std::function<void()> renderfun) {
    for (VolumeLightPtr &light : lights) {
        sgl::Renderer->setBlendMode(sgl::BLEND_ALPHA);
        sgl::Renderer->setViewMatrix(camera->getViewMatrix());
        sgl::Renderer->setProjectionMatrix(camera->getProjectionMatrix());
        shadowmapTarget->bindRenderTarget();
        sgl::Renderer->clearFramebuffer(GL_DEPTH_BUFFER_BIT, light->getColor(), 1.0f);
        shadowmapShader->setUniform("lightpos", light->getPosition());
        int matUniformLoc = shadowmapShader->getUniformLoc("camViewProjMatrices");
        for (int i = 0; i < 3; ++i) {
            shadowmapShader->setUniform(
                    matUniformLoc+i,
                    lightcamProj[i]*lightcamView[i]*sgl::matrixTranslation(-light->getPosition()));
        }
        /*shadowmapShader->bind();
        glm::mat4 matrices[3];
        for (int i = 0; i < 3; ++i) {
            matrices[i] = lightcamProj[i]*lightcamView[i]*matrixTranslation(-light->getPosition());
            //shadowmapShader->setUniform(matUniformLoc+i, lightcamProj[i]*lightcamView[i]*matrixTranslation(-light->getPosition()));
        }
        glUniformMatrix4fv(matUniformLoc, 3, false, (float*)&matrices);*/

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glViewport(0,0,shadowMapWidth,1);
        renderfun();
        glDisable(GL_DEPTH_TEST);
        sgl::Window *window = sgl::AppSettings::get()->getMainWindow();
        glViewport(0,0,window->getWidth(),window->getHeight());

        lightTarget->bindRenderTarget();
        sgl::Renderer->setBlendMode(sgl::BLEND_ADDITIVE);
        sgl::Renderer->setViewMatrix(camera->getViewMatrix());
        sgl::Renderer->setProjectionMatrix(camera->getProjectionMatrix());
        sgl::Renderer->setModelMatrix(sgl::matrixIdentity());
        shadowMapRenderShader->setUniform("lightpos", light->getPosition());
        shadowMapRenderShader->setUniform("lightColor", light->getColor());
        sgl::Renderer->render(shadowmapRenderAttributes);
    }
    sgl::Renderer->setBlendMode(sgl::BLEND_ALPHA);
}

void LightManagerMap::beginRenderLightmap() {
    camera->setRenderTarget(lightTarget);
    lightTarget->bindRenderTarget();
    sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, sgl::Color(0, 0, 0));
}

void LightManagerMap::endRenderLightmap() {
    sgl::Renderer->setBlendMode(sgl::BLEND_ALPHA);
    sgl::Renderer->unbindFBO();
}

void LightManagerMap::blitMixSceneAndLights() {
    sgl::Renderer->setProjectionMatrix(sgl::matrixIdentity());
    sgl::Renderer->setViewMatrix(sgl::matrixIdentity());
    sgl::Renderer->setModelMatrix(sgl::matrixIdentity());

    if (sgl::Keyboard->isKeyDown(SDLK_s)) {
        sgl::Renderer->blitTexture(
                sceneTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
    } else if (sgl::Keyboard->isKeyDown(SDLK_d)) {
        sgl::Renderer->blitTexture(
                lightTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)));
    } else {
        lightCombineShader->setUniform("lightTexture", lightTex, 1);
        sgl::Renderer->blitTexture(
                sceneTex, sgl::AABB2(glm::vec2(-1, -1), glm::vec2(1, 1)), lightCombineShader);
    }
}
