/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2020 - 2021, Christoph Neuhauser
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

in vec4 vertexPosition;
in float radius;

out VertexData {
    float radius;
} vertexOut;

void main() {
    vertexOut.radius = radius;
    gl_Position = vertexPosition;
}


-- Geometry

#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 mvpMatrix;

in VertexData {
    float radius;
} VertexIn[1];

out PixelData {
    vec2 uv;
    float radius;
} vertexOut;

void main() {
    float radius = VertexIn[0].radius;
    vertexOut.radius = radius;
    
    vertexOut.uv = vec2(0,0);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(-radius, -radius, 0.0, 0.0));
    EmitVertex();

    vertexOut.uv = vec2(1,0);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(+radius, -radius, 0.0, 0.0));
    EmitVertex();
    
    vertexOut.uv = vec2(0,1);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(-radius, +radius, 0.0, 0.0));
    EmitVertex();
    
    vertexOut.uv = vec2(1,1);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(+radius, +radius, 0.0, 0.0));
    EmitVertex();
    
    EndPrimitive();
}


-- Fragment

#version 430 core

uniform vec4 color;
uniform vec2 resolution;

in PixelData {
    vec2 uv;
    float radius;
} PixelIn;
out vec4 fragColor;

void main() {
    float delta = 0.5f/PixelIn.radius;
    
    float distance = distance(PixelIn.uv, vec2(0.5,0.5));
    float value = 1.-smoothstep(0.5-delta, 0.5+delta, distance);    
    fragColor = vec4(color.rgb, value*color.a);
}
