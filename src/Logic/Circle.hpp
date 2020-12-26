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

#ifndef LOGIC_VOLUMELIGHT_CIRCLE_HPP_
#define LOGIC_VOLUMELIGHT_CIRCLE_HPP_

#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include "Primitive.hpp"
#include <vector>
#include <glm/glm.hpp>

// TODO: Instance IDs

class CirclePrimitive : public Primitive {
public:
    CirclePrimitive(
            sgl::ShaderProgramPtr _plainShader, sgl::ShaderProgramPtr _edgeShader,
            const glm::mat4 &_specialTransform = sgl::matrixIdentity());
    inline void setPosition(const glm::vec2 &pos) { position = pos; }
    void render();
    void renderEdges();
    void setEdgeShader(sgl::ShaderProgramPtr _edgeShader);

private:
    sgl::ShaderProgramPtr plainShader;
    sgl::ShaderProgramPtr edgeShader;
    sgl::ShaderAttributesPtr circleData;
    sgl::ShaderAttributesPtr edgeData;
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> edges;
    glm::vec2 position;
    glm::mat4 specialTransform;
};


#endif /* LOGIC_VOLUMELIGHT_CIRCLE_HPP_ */
