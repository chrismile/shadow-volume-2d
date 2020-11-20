-- Vertex

#version 430 core

in vec4 position;
in float radius;

out VertexData {
    float radius;
} VertexOut;

void main()
{
    VertexOut.radius = radius;
    gl_Position = position;
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
} VertexOut;

void main()
{
    float radius = VertexIn[0].radius;
    VertexOut.radius = radius;
    
    VertexOut.uv = vec2(0,0);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(-radius, -radius, 0.0, 0.0));
    EmitVertex();

    VertexOut.uv = vec2(1,0);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(+radius, -radius, 0.0, 0.0));
    EmitVertex();
    
    VertexOut.uv = vec2(0,1);
    gl_Position = mvpMatrix * (gl_in[0].gl_Position + vec4(-radius, +radius, 0.0, 0.0));
    EmitVertex();
    
    VertexOut.uv = vec2(1,1);
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

void main()
{
    float delta = 0.5f/PixelIn.radius;
    
    float distance = distance(PixelIn.uv, vec2(0.5,0.5));
    float value = 1.-smoothstep(0.5-delta, 0.5+delta, distance);    
    fragColor = vec4(color.rgb, value*color.a);
}
