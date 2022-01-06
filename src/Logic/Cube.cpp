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

#include "Cube.hpp"
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>

Cube::Cube(
        sgl::ShaderProgramPtr _plainShader, sgl::ShaderProgramPtr _edgeShader, const glm::vec2 &extent,
        const glm::mat4 &_specialTransform) {
    specialTransform = _specialTransform;
    plainShader = _plainShader;
    edgeShader = _edgeShader;
    edges = {
            glm::vec2(-extent.x, -extent.y),
            glm::vec2(extent.x, -extent.y),
            glm::vec2(extent.x, extent.y),
            glm::vec2(-extent.x, extent.y)
    };
    vertices = {
            glm::vec2(extent.x, -extent.y),
            glm::vec2(extent.x, extent.y),
            glm::vec2(-extent.x, -extent.y),
            glm::vec2(-extent.x, extent.y)
    };

    cubeData = sgl::ShaderManager->createShaderAttributes(plainShader);
    sgl::GeometryBufferPtr geometryBuffer = sgl::Renderer->createGeometryBuffer(
            sizeof(glm::vec2)*vertices.size(), &vertices.front());
    cubeData->addGeometryBuffer(geometryBuffer, "vertexPosition", sgl::ATTRIB_FLOAT, 2);
    cubeData->setVertexMode(sgl::VERTEX_MODE_TRIANGLE_STRIP);

    edgeData = sgl::ShaderManager->createShaderAttributes(edgeShader);
    geometryBuffer = sgl::Renderer->createGeometryBuffer(sizeof(glm::vec2)*edges.size(), &edges.front());
    edgeData->addGeometryBuffer(geometryBuffer, "vertexPosition", sgl::ATTRIB_FLOAT, 2);
    edgeData->setVertexMode(sgl::VERTEX_MODE_LINE_LOOP);
}

void Cube::setEdgeShader(sgl::ShaderProgramPtr _edgeShader) {
    edgeData = edgeData->copy(_edgeShader);
}

void Cube::render() {
    sgl::Renderer->setModelMatrix(sgl::matrixTranslation(position)*specialTransform);
    plainShader->setUniform("color", sgl::Color(60, 60, 60));
    sgl::Renderer->render(cubeData);
}

void Cube::renderEdges() {
    sgl::Renderer->setModelMatrix(sgl::matrixTranslation(position)*specialTransform);
    sgl::Renderer->render(edgeData);
}
