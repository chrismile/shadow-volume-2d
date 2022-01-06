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
    gl_Position = mMatrix * vec4(vertexPosition, 0., 1.);
}


-- Geometry

#version 430 core

layout(lines) in;
// vertices: 4 * 3 = quad * cameras
layout(triangle_strip, max_vertices = 12) out;

uniform vec2 lightpos;
uniform mat4 camViewProjMatrices[3]; 

out vec2 fragPos;

void main() {
    vec2 pt0 = gl_in[0].gl_Position.xy;
    vec2 pt1 = gl_in[1].gl_Position.xy;
    vec2 offsetvec = pt1 - pt0;
    vec2 normal = normalize(vec2(-offsetvec.y, offsetvec.x));
    vec2 lightnormal = normalize((pt0 + pt1) / 2.0f - lightpos);
    if (dot(normal, lightnormal) < 0) {
        // Facing away from camera
        
        // Iterate over all three triangle camera views
        for(int face = 0; face < 3; ++face) {
            gl_Layer = face;
            mat4 vpMatrix = camViewProjMatrices[face];
            
            vec4 dirUp = vec4(0.0, 0.0, 1.0, 0.0);
            vec4 dirDown = vec4(0.0, 0.0, -1.0, 0.0);


            /**
             * Oddly, this code only works on the Intel Mesa3D driver.
             */
            /*fragPos = gl_in[0].gl_Position.xy;
            gl_Position = vpMatrix * gl_in[0].gl_Position;
            EmitVertex();
            fragPos = gl_in[0].gl_Position.xy;
            gl_Position = vpMatrix * dirUp;
            EmitVertex();
            fragPos = gl_in[1].gl_Position.xy;
            gl_Position = vpMatrix * gl_in[1].gl_Position;
            EmitVertex();
            EndPrimitive();
            
            fragPos = gl_in[1].gl_Position.xy;
            gl_Position = vpMatrix * gl_in[1].gl_Position;
            EmitVertex();
            fragPos = gl_in[1].gl_Position.xy;
            gl_Position = vpMatrix * dirDown;
            EmitVertex();
            fragPos = gl_in[0].gl_Position.xy;
            gl_Position = vpMatrix * gl_in[0].gl_Position;
            EmitVertex();
            EndPrimitive();*/


            // Works on NVIDIA & Intel GPUs
            fragPos = gl_in[0].gl_Position.xy;
            gl_Position = vpMatrix * (gl_in[0].gl_Position + dirUp);
            EmitVertex();
            fragPos = gl_in[1].gl_Position.xy;
            gl_Position = vpMatrix * (gl_in[1].gl_Position + dirUp);
            EmitVertex();
            fragPos = gl_in[0].gl_Position.xy;
            gl_Position = vpMatrix * (gl_in[0].gl_Position + dirDown);
            EmitVertex();
            fragPos = gl_in[1].gl_Position.xy;
            gl_Position = vpMatrix * (gl_in[1].gl_Position + dirDown);
            EmitVertex();
            EndPrimitive();
        }
    }
}


-- Fragment

#version 430 core

in vec2 fragPos;
in float currLayer;

uniform vec2 lightpos;
uniform float farPlaneDist;
uniform int lightnum;
uniform vec3 texsize; // in pixels

void main() {
    float lightDistance = length(fragPos - lightpos);
    lightDistance = clamp(lightDistance / farPlaneDist, 0.0, 1.0); // Map to [0;1]
    gl_FragDepth = lightDistance;
}
