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

#include "Circle.hpp"
#include "Arc.hpp"
#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>

CirclePrimitive::CirclePrimitive(
        sgl::ShaderProgramPtr _plainShader, sgl::ShaderProgramPtr _edgeShader, const glm::mat4 &_specialTransform) {
    specialTransform = _specialTransform;
    const float rad = 0.2f;
    const int numSegments = 64;

    getPointsOnCircle(edges, glm::vec2(0.0f, 0.0f), rad, numSegments);
    vertices = edges;

    plainShader = _plainShader;
    edgeShader = _edgeShader;

    circleData = sgl::ShaderManager->createShaderAttributes(plainShader);
    sgl::GeometryBufferPtr geometryBuffer = sgl::Renderer->createGeometryBuffer(sizeof(glm::vec2)*vertices.size(), &vertices.front());
    circleData->addGeometryBuffer(geometryBuffer, "position", sgl::ATTRIB_FLOAT, 2);
    circleData->setVertexMode(sgl::VERTEX_MODE_TRIANGLE_FAN);

    edgeData = sgl::ShaderManager->createShaderAttributes(edgeShader);
    geometryBuffer = sgl::Renderer->createGeometryBuffer(sizeof(glm::vec2)*edges.size(), &edges.front());
    edgeData->addGeometryBuffer(geometryBuffer, "position", sgl::ATTRIB_FLOAT, 2);
    edgeData->setVertexMode(sgl::VERTEX_MODE_LINE_LOOP);
}

void CirclePrimitive::setEdgeShader(sgl::ShaderProgramPtr _edgeShader) {
    edgeData = edgeData->copy(_edgeShader);
}

void CirclePrimitive::render() {
    sgl::Renderer->setModelMatrix(sgl::matrixTranslation(position)*specialTransform);
    plainShader->setUniform("color", sgl::Color(60, 60, 60));
    sgl::Renderer->render(circleData);
}

void CirclePrimitive::renderEdges() {
    sgl::Renderer->setModelMatrix(sgl::matrixTranslation(position)*specialTransform);
    sgl::Renderer->render(edgeData);
}
