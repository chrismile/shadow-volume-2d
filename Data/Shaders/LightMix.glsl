-- Vertex

#version 430 core

in vec2 position;
in vec2 texcoord;
out vec2 st;

void main()
{
	st = texcoord;
	gl_Position = vec4(position, 0., 1.);
}

-- Fragment

#version 430 core

uniform sampler2D texture; // Scene
uniform sampler2D lightTexture;
uniform vec4 ambientLight;
in vec2 st;
out vec4 fragColor;

void main()
{
	vec4 textureRGBA = texture2D(texture, st).rgba;
	vec3 texture = textureRGBA.rgb;
	vec3 light = clamp(texture2D(lightTexture, st).rgb + ambientLight.rgb, 0., 1.);
	fragColor = vec4(texture * light, textureRGBA.a);
}
