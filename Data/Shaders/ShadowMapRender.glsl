/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2017 - 2021, Christoph Neuhauser
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

-- Vertex

#version 430 core

in vec2 vertexPosition;
out vec2 fragPosWorld;

void main() {
    fragPosWorld = vertexPosition;
    gl_Position = mvpMatrix * vec4(vertexPosition, 0., 1.);
}


-- Fragment

#version 430 core

uniform sampler2DArray depthMap;
uniform vec2 lightpos;
uniform vec4 lightColor;
uniform float farPlaneDist;
in vec2 fragPosWorld;
out vec4 fragColor;

#define PI 3.1415926535897

// index: The index of the shadow map
// P: The direction to the fragment seen from the 
float getShadowMapCoordinate(int index, vec2 P) {
    if (index == 1) {
        // Rotate 240° ccw
        P = vec2(-1.0/2.0*P.x + sqrt(3.0)/2.0*P.y,
                -sqrt(3.0)/2.0*P.x - 1.0/2.0*P.y);
    } else if (index == 2) {
        // Rotate 120° ccw
        P = vec2(-1.0/2.0*P.x - sqrt(3.0)/2.0*P.y,
                sqrt(3.0)/2.0*P.x - 1.0/2.0*P.y);
    }

    return (P.x / (sqrt(3.0) * P.y) + 1.0) / 2.0;
}

float getFragmentDepth(vec2 fragPosLight) {
    float angle = atan(fragPosLight.y, fragPosLight.x);
    int index = int(mod(floor((angle + 11.0/6.0*PI)/(2.0/3.0*PI)), 3.0));
    float xCoord = getShadowMapCoordinate(index, fragPosLight);
    float depth = texture(depthMap, vec3(xCoord, 0.0, float(index))).r * farPlaneDist;
    return depth;
}

const float BIAS = 0.002;

void main() {
    float fragDist = length(fragPosWorld - lightpos);
    float occlusionDepth = getFragmentDepth(fragPosWorld - lightpos);
    vec4 color = lightColor;
    if (fragDist > occlusionDepth - BIAS) {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    fragColor = color;
}


