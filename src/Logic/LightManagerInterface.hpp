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

#ifndef LOGIC_LIGHTMANAGERINTERFACE_HPP_
#define LOGIC_LIGHTMANAGERINTERFACE_HPP_

#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Scene/RenderTarget.hpp>
#include <Graphics/Scene/Camera.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include "VolumeLight.hpp"

class LightManagerInterface
{
public:
    LightManagerInterface() {}
    virtual ~LightManagerInterface() {}
    virtual void beginRenderScene()=0;
    virtual void endRenderScene()=0;
    virtual void beginRenderLightmap()=0;
    virtual void renderLightmap(std::function<void()> renderfun)=0;
    virtual void endRenderLightmap()=0;
    virtual void blitMixSceneAndLights()=0;
    virtual void renderGUI()=0;

    virtual VolumeLightPtr addLight(
            const glm::vec2 &pos, float rad = 10.0f, const sgl::Color &col = sgl::Color(255, 255, 255))=0;
    virtual std::vector<VolumeLightPtr> &getLights()=0;

    virtual void onResolutionChanged()=0;
    virtual sgl::ShaderProgramPtr getEdgeShader()=0;
};



#endif /* LOGIC_LIGHTMANAGERINTERFACE_HPP_ */
