-- Vertex

attribute vec2 position;
attribute vec2 texcoord;
varying vec2 st;

void main()
{
	st = texcoord;
	gl_Position = vec4(position, 0., 1.);
}

-- Fragment

uniform sampler2D texture; // Scene
uniform sampler2D lightTexture;
uniform vec4 ambientLight;
varying vec2 st;

void main()
{
	vec4 textureRGBA = texture2D(texture, st).rgba;
	vec3 texture = textureRGBA.rgb;
	vec3 light = clamp(texture2D(lightTexture, st).rgb + ambientLight.rgb, 0., 1.);
	gl_FragColor = vec4(texture * light, textureRGBA.a);
}
