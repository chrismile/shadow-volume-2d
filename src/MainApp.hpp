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

#ifndef LOGIC_MainApp_HPP_
#define LOGIC_MainApp_HPP_

#include <vector>
#include <glm/glm.hpp>

#include <Utils/AppLogic.hpp>
#include <Utils/Random/Xorshift.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include <Math/Geometry/Point2.hpp>
#include <Graphics/Mesh/Mesh.hpp>
#include <Graphics/Scene/Camera.hpp>
#include <Graphics/Video/VideoWriter.hpp>

#include "Logic/Cube.hpp"
#include "Logic/Primitive.hpp"
#include "Logic/LightManagerMap.hpp"
#include "Logic/LightManagerVolume.hpp"

class Shape;
typedef boost::shared_ptr<Shape> ShapePtr;

class VolumeLightApp : public sgl::AppLogic {
public:
    VolumeLightApp();
    ~VolumeLightApp();
    void render();
    void renderGUI();
    void processSDLEvent(const SDL_Event &event);
    void renderScene(); // Renders lighted scene
    void renderEdges(); // Renders edge lines of scene that get extruded by the geometry of "edgeShader"
    void update(float dt);
    void resolutionChanged(sgl::EventPtr event);

private:
    // Lighting & rendering
    sgl::CameraPtr camera;
    boost::shared_ptr<LightManagerInterface> lightManager;
    int lightManagerType;
    vector<PrimitivePtr> primitives;
    sgl::ShaderProgramPtr plainShader;
    sgl::ShaderProgramPtr edgeShader;
    sgl::ShaderProgramPtr whiteSolidShader;

    // User interaction
    sgl::ShaderAttributesPtr grabPointRenderData;
    float grabPointRadius;
    VolumeLightPtr grabbedLight;
    sgl::XorshiftRandomGenerator random;

    // GUI
    bool showSettingsWindow = true;

    // Benchmarking performance
    bool benchmark;
    float benchmarkTimer;
    bool benchmarkFinished;
    int maxLights;
    int numProbes;
    std::vector<vector<float>> benchmarkData;
    std::vector<int> fps;

    // Save video stream to file
    sgl::VideoWriter *videoWriter;
};

#endif /* LOGIC_MainApp_HPP_ */
