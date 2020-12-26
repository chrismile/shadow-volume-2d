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

#ifndef RENDERABLES_FUNCTIONS_ARC_HPP_
#define RENDERABLES_FUNCTIONS_ARC_HPP_

#include <vector>
#include <glm/glm.hpp>

struct SvgEllipticalArcDataIn {
    SvgEllipticalArcDataIn() : x1(0), y1(0), x2(0), y2(0), rx(0), ry(0), deg(0), fa(0), fs(0) {}
    SvgEllipticalArcDataIn(float _x1, float _y1, float _x2, float _y2, float _rx, float _ry, float _deg, int _fa, int _fs)
        : x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), deg(_deg), fa(_fa), fs(_fs) {}
    float x1, y1, x2, y2, rx, ry, deg, fa, fs;
};

void getPointsOnSvgEllipticalArc(std::vector<glm::vec2> &points, const SvgEllipticalArcDataIn &in);
void getPointsOnCircleArc(std::vector<glm::vec2> &points, const glm::vec2 &center, float radius, const glm::vec2 &start, const glm::vec2 &end, int direction);
void getPointsOnCircle(std::vector<glm::vec2> &points, const glm::vec2 &center, float radius, int numSegments);
void getPointsOnCircleArc(std::vector<glm::vec2> &points, const glm::vec2 &center, float radius, float startAngle, float arcAngle, int numSegments);
void getPointsOnEllipse(std::vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy, int numSegments);
void getPointsOnEllipseArc(std::vector<glm::vec2> &points, const glm::vec2 &center, float radiusx, float radiusy, float startAngle, float arcAngle, int numSegments);

// Get appropriate subdivision segment number
int getNumCircleSegments(float radius);

#endif /* RENDERABLES_FUNCTIONS_ARC_HPP_ */
