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
in vec2 vertexTexCoord;
out vec2 st;

void main() {
    st = vertexTexCoord;
    gl_Position = vec4(vertexPosition, 0., 1.);
}

-- Fragment

#version 430 core

uniform sampler2D inputTexture; // Scene
uniform sampler2D lightTexture;
uniform vec4 ambientLight;
in vec2 st;
out vec4 fragColor;

void main() {
    vec4 textureColorRgba = texture(inputTexture, st).rgba;
    vec3 textureColorRgb = textureColorRgba.rgb;
    vec3 light = clamp(texture(lightTexture, st).rgb + ambientLight.rgb, 0.0, 1.0);
    fragColor = vec4(textureColorRgb * light, textureColorRgba.a);
}
