-- Vertex

uniform mat4 mMatrix;

in vec2 position;

void main()
{
	gl_Position = mMatrix * vec4(position, 0., 1.);
}


-- Geometry

layout(lines) in;
// vertices: 4 * 2 * 3 = quad * (up+down) * cameras
layout(triangle_strip, max_vertices = 24) out;

uniform vec2 lightpos;
uniform mat4 camViewProjMatrices[3]; 

out vec2 fragPos;
out vec2 texcoord;

void main()
{
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
			
			/*fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[0].gl_Position;
			EmitVertex();
			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * (gl_in[0].gl_Position + vec4(0.0,0.0,100.0,0.0));
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[1].gl_Position;
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * (gl_in[1].gl_Position + vec4(0.0,0.0,100.0,0.0));
			EmitVertex();
			EndPrimitive();
			
			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[0].gl_Position;
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[1].gl_Position;
			EmitVertex();
			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * (gl_in[0].gl_Position - vec4(0.0,0.0,100.0,0.0));
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * (gl_in[1].gl_Position - vec4(0.0,0.0,100.0,0.0));
			EmitVertex();
			EndPrimitive();*/


			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[0].gl_Position;
			EmitVertex();
			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * dirUp;
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * gl_in[1].gl_Position;
			EmitVertex();
			fragPos = gl_in[1].gl_Position.xy;
			gl_Position = vpMatrix * dirUp;
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
			fragPos = gl_in[0].gl_Position.xy;
			gl_Position = vpMatrix * dirDown;
			EmitVertex();
			EndPrimitive();
		}
	}
}


-- Fragment

in vec2 fragPos;
in float currLayer;
in vec2 texcoord;

uniform vec2 lightpos;
uniform float farPlaneDist;
uniform int lightnum;
uniform vec3 texsize; // in pixels

void main()
{
	float lightDistance = length(fragPos - lightpos);
	lightDistance = clamp(lightDistance / farPlaneDist, 0.0, 1.0); // Map to [0;1]
	gl_FragDepth = lightDistance;
}
