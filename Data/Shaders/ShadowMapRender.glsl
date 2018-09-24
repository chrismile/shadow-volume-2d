-- Vertex

#version 430 core

in vec2 position;
out vec2 fragPosWorld;

void main()
{
	fragPosWorld = position;
	gl_Position = mvpMatrix * vec4(position, 0., 1.);
}


-- Fragment

#version 430 core

uniform sampler2DArray depthMap;
uniform vec2 lightpos;
uniform vec4 lightColor;
uniform float farPlaneDist;
in vec2 fragPosWorld;

#define PI 3.1415926535897



// index: The index of the shadow map
// P: The direction to the fragment seen from the 
float getShadowMapCoordinate(int index, vec2 P)
{
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

float getFragmentDepth(vec2 fragPosLight)
{
	float angle = atan(fragPosLight.y, fragPosLight.x);
	int index = int(mod(floor((angle + 11.0/6.0*PI)/(2.0/3.0*PI)), 3.0));
	float xCoord = getShadowMapCoordinate(index, fragPosLight);
	float depth = texture(depthMap, vec3(xCoord, 0.0, float(index))).r * farPlaneDist;
	return depth;
}

const float BIAS = 0.002;

void main()
{
	float fragDist = length(fragPosWorld - lightpos);
	float occlusionDepth = getFragmentDepth(fragPosWorld - lightpos);
	vec4 color = lightColor;
	if (fragDist > occlusionDepth - BIAS) {
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
	gl_FragColor = color;
}


