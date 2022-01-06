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

#include <climits>
#include <GL/glew.h>

#include <Input/Keyboard.hpp>
#include <Math/Math.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Utils/Events/EventManager.hpp>
#include <Utils/Random/Xorshift.hpp>
#include <Utils/Timer.hpp>
#include <Input/Mouse.hpp>
#include <Input/Keyboard.hpp>
#include <Input/Gamepad.hpp>
#include <Utils/File/Logfile.hpp>
#include <Utils/File/FileUtils.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <ImGui/ImGuiWrapper.hpp>

#include "Logic/Circle.hpp"
#include "Logic/Arc.hpp"
#include "MainApp.hpp"
#include <glm/gtx/color_space.hpp>

void openglErrorCallback() {
    std::cerr << "Application callback" << std::endl;
}

VolumeLightApp::VolumeLightApp() : camera(new sgl::Camera()), random(10203), videoWriter(NULL) {
    plainShader = sgl::ShaderManager->getShaderProgram({"Mesh.Vertex.Plain", "Mesh.Fragment.Plain"});
    whiteSolidShader = sgl::ShaderManager->getShaderProgram({"WhiteSolid.Vertex", "WhiteSolid.Fragment"});

    sgl::EventManager::get()->addListener(
            sgl::RESOLUTION_CHANGED_EVENT, [this](sgl::EventPtr event){ this->resolutionChanged(event); });

    camera->setNearClipDistance(0.01f);
    camera->setFarClipDistance(100.0f);
    camera->setOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    camera->setYaw(-sgl::PI / 2.0f);
    camera->setPitch(0.0f);
    float fovy = atanf(1.0f / 2.0f) * 2.0f;
    camera->setFOVy(fovy);
    camera->setPosition(glm::vec3(0.5f, 0.5f, 1.0f));

    sgl::Renderer->setErrorCallback(&openglErrorCallback);
    sgl::Renderer->setDebugVerbosity(sgl::DEBUG_OUTPUT_CRITICAL_ONLY);

    lightManager = boost::shared_ptr<LightManagerInterface>(new LightManagerMap(camera)); // LightManagerMap
    lightManagerType = 0;
    edgeShader = lightManager->getEdgeShader();
    //VolumeLightPtr light = lightManager->addLight(glm::vec2(0.5,0.5));
    VolumeLightPtr light = lightManager->addLight(glm::vec2(0.5,0.5));

    // Add objects to scene
    // A
    Cube *cube = new Cube(plainShader, edgeShader, glm::vec2(0.2f, 0.12f));
    cube->setPosition(glm::vec2(0.5f, 0.2f));
    primitives.push_back(PrimitivePtr(cube));

    // B
    glm::mat4 specialTransform = sgl::matrixScaling(glm::vec2(1.0f, 0.7f));
    CirclePrimitive *circle = new CirclePrimitive(plainShader, edgeShader, specialTransform);
    circle->setPosition(glm::vec2(0.74f, 0.7f));
    primitives.push_back(PrimitivePtr(circle));

    // C
    specialTransform = sgl::matrixSkewY(0.3f);
    cube = new Cube(plainShader, edgeShader, glm::vec2(0.1f, 0.2f), specialTransform);
    cube->setPosition(glm::vec2(0.2f, 0.7f));
    primitives.push_back(PrimitivePtr(cube));


    // Create grab point data for user interaction
    vector<glm::vec2> vertices;
    grabPointRadius = 0.01f;
    int numSegments = 32;
    getPointsOnCircle(vertices, glm::vec2(0.0f, 0.0f), grabPointRadius, numSegments);

    grabPointRenderData = sgl::ShaderManager->createShaderAttributes(plainShader);
    sgl::GeometryBufferPtr geometryBuffer = sgl::Renderer->createGeometryBuffer(
            sizeof(glm::vec2)*vertices.size(), &vertices.front());
    grabPointRenderData->addGeometryBuffer(
            geometryBuffer, "vertexPosition", sgl::ATTRIB_FLOAT, 2);
    grabPointRenderData->setVertexMode(sgl::VERTEX_MODE_TRIANGLE_FAN);

    resolutionChanged(sgl::EventPtr());

    // Benchmark mode
    benchmark = false;
    benchmarkFinished = false;
    maxLights = 101;
    numProbes = 10;
    //Timer->disableFixedFPS();
    if (benchmark) {
        benchmarkTimer = sgl::Timer->getTimeInSeconds();

        // Start with no light
        lightManager->getLights().clear();

        benchmarkData.resize(maxLights);
        for (vector<float> &arr : benchmarkData) {
            arr.reserve(numProbes);
        }
        fps.resize(maxLights);
    }
}

VolumeLightApp::~VolumeLightApp() {
    // Save data to csv table if benchmarking was performed
    if (benchmark && benchmarkFinished) {
        string csvData = "";

        // First row: The number of lights
        for (int i = 0; i < maxLights; ++i) {
            csvData.append(sgl::toString(i) + ",");
        }
        csvData.append("\n");

        // Second row: The median FPS
        for (int i = 0; i < maxLights; ++i) {
            std::sort(benchmarkData.at(i).begin(), benchmarkData.at(i).end());
            int fps = roundf(benchmarkData.at(i).at(numProbes/2));
            csvData.append(sgl::toString(fps) + ",");
        }

        ofstream file("benchmark.csv");
        file << csvData;
        file.close();
    }

    lightManager = boost::shared_ptr<LightManagerInterface>();

    if (videoWriter != NULL) {
        delete videoWriter;
    }
}

void VolumeLightApp::renderScene() {
    sgl::Renderer->setProjectionMatrix(camera->getProjectionMatrix());
    sgl::Renderer->setViewMatrix(camera->getViewMatrix());
    sgl::Renderer->setModelMatrix(sgl::matrixIdentity());

    for (PrimitivePtr &p : primitives) {
        p->render();
    }
}

void VolumeLightApp::renderEdges() {
    sgl::Renderer->setProjectionMatrix(camera->getProjectionMatrix());
    sgl::Renderer->setViewMatrix(camera->getViewMatrix());
    sgl::Renderer->setModelMatrix(sgl::matrixIdentity());

    for (PrimitivePtr &p : primitives) {
        p->renderEdges();
    }
}

void VolumeLightApp::render()
{
    bool wireframe = false;

    if (videoWriter == NULL) {
        //videoWriter = new VideoWriter("video.mp4");
    }

    sgl::Renderer->setCamera(camera);

    lightManager->beginRenderScene();
    // Render scene
    renderScene();
    lightManager->endRenderScene();

    lightManager->beginRenderLightmap();
    // Render edge silhouettes that get extruded to infinity to create shadow volumes
    lightManager->renderLightmap([this]{ renderEdges(); });
    lightManager->endRenderLightmap();

    // Blit compostited scene to screen framebuffer
    lightManager->blitMixSceneAndLights();


    // User interaction: Render light handles
    sgl::Renderer->setViewMatrix(camera->getViewMatrix());
    sgl::Renderer->setProjectionMatrix(camera->getProjectionMatrix());
    for (VolumeLightPtr &light : lightManager->getLights()) {
        sgl::Renderer->setModelMatrix(sgl::matrixTranslation(light->getPosition()));
        plainShader->setUniform("color", sgl::Color(255, 152, 43));
        sgl::Renderer->render(grabPointRenderData);
    }

    // Wireframe mode
    if (wireframe) {
        sgl::Renderer->setModelMatrix(sgl::matrixIdentity());
        sgl::Renderer->setLineWidth(1.0f);
        sgl::Renderer->enableWireframeMode();
        renderScene();
        sgl::Renderer->disableWireframeMode();
    }

    renderGUI();

    //videoWriter->pushWindowFrame();
}

void VolumeLightApp::renderGUI() {
    sgl::ImGuiWrapper::get()->renderStart();
    //ImGuiWrapper::get()->renderDemoWindow();

    if (showSettingsWindow) {
        ImGui::Begin("Settings", &showSettingsWindow);

        bool changeMode = false;
        static int mode = 0;
        changeMode |= ImGui::RadioButton("Shadow Maps", &lightManagerType, 0); ImGui::SameLine();
        changeMode |= ImGui::RadioButton("Shadow Volumes", &lightManagerType, 1);
        if (changeMode) {
            auto lights = lightManager->getLights();
            if (lightManagerType == 0) {
                lightManager = boost::shared_ptr<LightManagerInterface>(new LightManagerMap(camera));
            } else {
                lightManager = boost::shared_ptr<LightManagerInterface>(new LightManagerVolume(camera));
            }
            edgeShader = lightManager->getEdgeShader();
            for (PrimitivePtr &primitive : primitives) {
                primitive->setEdgeShader(edgeShader);
            }
            for (VolumeLightPtr &light : lights) {
                lightManager->addLight(light->getPosition(), light->getRadius(), light->getColor());
            }
        }

        lightManager->renderGUI();

        ImGui::End();
    }

    sgl::ImGuiWrapper::get()->renderEnd();
}

void VolumeLightApp::processSDLEvent(const SDL_Event &event) {
    sgl::ImGuiWrapper::get()->processSDLEvent(event);
}

void VolumeLightApp::resolutionChanged(sgl::EventPtr event) {
    camera->onResolutionChanged(event);
    lightManager->onResolutionChanged();
    camera->onResolutionChanged(event);
}

void VolumeLightApp::update(float dt) {
    AppLogic::update(dt);

    if (benchmark) {
        int numLights = lightManager->getLights().size();

        // Add new FPS snapshot to buffer every 50ms
        if (fabs(benchmarkTimer - sgl::Timer->getTimeInSeconds()) > 0.05f) {
            benchmarkTimer = sgl::Timer->getTimeInSeconds();
            benchmarkData.at(numLights).push_back(getFPS());

        }

        // If we have enough samples/probes, add a new light with random color/position
        if (benchmarkData.at(numLights).size() >= (size_t)numProbes) {
            // Add light randomly
            glm::vec2 randomPos(random.getRandomFloatBetween(-1.0f, 1.0f), random.getRandomFloatBetween(-1.0f, 1.0f));
            VolumeLightPtr light = lightManager->addLight(randomPos, 10.0f, sgl::Color(100, 100, 100));
            numLights = lightManager->getLights().size();
        }

        // Quit if all data has been stored
        if (numLights >= maxLights) {
            benchmarkFinished = true;
            quit();
        }
    }



    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        // Ignore inputs below
        return;
    }

    glm::vec2 mousepos = camera->mousePositionInPlane(0.0f);

    if (sgl::Keyboard->keyPressed(SDLK_RETURN)) {
        auto lights = lightManager->getLights();
        if (lightManagerType == 0) {
            lightManagerType = 1;
            lightManager = boost::shared_ptr<LightManagerInterface>(new LightManagerVolume(camera));
        } else {
            lightManagerType = 0;
            lightManager = boost::shared_ptr<LightManagerInterface>(new LightManagerMap(camera));
        }
        edgeShader = lightManager->getEdgeShader();
        for (PrimitivePtr &primitive : primitives) {
            primitive->setEdgeShader(edgeShader);
        }
        for (VolumeLightPtr &light : lights) {
            lightManager->addLight(light->getPosition(), light->getRadius(), light->getColor());
        }
    }


    if (io.WantCaptureMouse) {
        // Ignore inputs below
        return;
    }

    // --- Start of user interaction ---
    // Right mouse button: Remove light
    if (sgl::Mouse->buttonPressed(3)) {
        auto &lights = lightManager->getLights();
        for (auto it = lights.begin(); it != lights.end(); ++it) {
            VolumeLightPtr &light = *it;
            if (glm::distance(light->getPosition(), mousepos) <= grabPointRadius) {
                lights.erase(it);
                break;
            }
        }
    }

    // Left mouse button: Grab light
    if (sgl::Mouse->buttonPressed(1)) {
        for (VolumeLightPtr &light : lightManager->getLights()) {
            if (glm::distance(light->getPosition(), mousepos) <= grabPointRadius) {
                grabbedLight = light;
                break;
            }
        }
    }

    // Middle mouse button: Add new light
    if (sgl::Mouse->buttonPressed(2)) {
        float randomVal = random.getRandomFloatBetween(0.0f, 360.0f);
        glm::vec3 hsvVec(randomVal, 1.0f, 0.1f);
        glm::vec3 rgbVec = glm::rgbColor(hsvVec);
        sgl::Color col = sgl::colorFromFloat(rgbVec.x, rgbVec.y, rgbVec.z, 1.0f);
        VolumeLightPtr light = lightManager->addLight(mousepos, 10.0f, col);
    }

    // Mouse dragged: Move light
    if (sgl::Mouse->isButtonDown(1) && grabbedLight) {
        grabbedLight->setPosition(mousepos);
    }

    // Left mouse button released: Un-grab light
    if (sgl::Mouse->buttonReleased(1)) {
        grabbedLight = VolumeLightPtr();
    }
}
