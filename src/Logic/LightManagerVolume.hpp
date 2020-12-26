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

#ifndef LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_
#define LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_

#include "LightManagerInterface.hpp"

class LightManagerVolume : public LightManagerInterface {
public:
    LightManagerVolume(sgl::CameraPtr _camera);
    void beginRenderScene();
    void endRenderScene();
    void beginRenderLightmap();
    void renderLightmap(std::function<void()> renderfun);
    void endRenderLightmap();
    void blitMixSceneAndLights();
    void renderGUI();

    VolumeLightPtr addLight(
            const glm::vec2 &pos, float rad = 10.0f, const sgl::Color &col = sgl::Color(255, 255, 255));
    std::vector<VolumeLightPtr> &getLights() { return lights; }

    void onResolutionChanged();
    sgl::ShaderProgramPtr getEdgeShader() { return edgeShader; }

private:
    sgl::CameraPtr camera;
    std::vector<VolumeLightPtr> lights;
    sgl::ShaderProgramPtr plainShader;
    sgl::ShaderProgramPtr edgeShader;
    sgl::ShaderProgramPtr lightCombineShader;

    sgl::RenderTargetPtr sceneTarget;
    sgl::RenderTargetPtr lightTarget;
    sgl::RenderTargetPtr lightTempTarget;
    sgl::FramebufferObjectPtr sceneFBO;
    sgl::TexturePtr sceneRenderTex; // Equal to sceneTex if no MSAA is used
    sgl::TexturePtr sceneTex;
    sgl::FramebufferObjectPtr lightFBO;
    sgl::TexturePtr lightRenderTex; // Equal to lightTex if no MSAA is used
    sgl::TexturePtr lightTex;
    sgl::FramebufferObjectPtr lightTempFBO;
    sgl::TexturePtr lightTempTex;
};



#endif /* LOGIC_VOLUMELIGHT_LIGHTMANAGER_HPP_ */
