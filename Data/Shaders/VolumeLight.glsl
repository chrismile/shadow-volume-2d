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

void main() {
    gl_Position = mMatrix * vec4(vertexPosition, 0.0, 1.0);
}


-- Geometry

#version 430 core

// Contour lines in world space
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 lightpos;

const float bias = 0.002;

void main() {
    vec2 pt0 = gl_in[0].gl_Position.xy;
    vec2 pt1 = gl_in[1].gl_Position.xy;
    vec2 offsetvec = pt1 - pt0;
    vec2 normal = normalize(vec2(-offsetvec.y, offsetvec.x));
    vec2 lightnormal = normalize((pt0 + pt1) / 2.0f - lightpos);
    if (dot(normal, lightnormal) < 0) {
        // Facing away from camera
        mat4 vpMatrix = pMatrix * vMatrix;
        
        vec4 dir0 = vec4(pt0 - lightpos, 0.0, 0.0);
        vec4 dir1 = vec4(pt1 - lightpos, 0.0, 0.0);
        
        gl_Position = vpMatrix * gl_in[0].gl_Position - bias * normalize(vec4(pt0 - lightpos, 0.0, 0.0));
        EmitVertex();

        gl_Position = vpMatrix * dir0;
        EmitVertex();
        
        gl_Position = vpMatrix * gl_in[1].gl_Position - bias * normalize(vec4(pt1 - lightpos, 0.0, 0.0));
        EmitVertex();

        gl_Position = vpMatrix * dir1;
        EmitVertex();
        
        EndPrimitive();
    }
}


-- Fragment

#version 430 core

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
