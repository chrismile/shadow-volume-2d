-- Vertex

#version 430 core

in vec2 position;

void main()
{
	gl_Position = mMatrix * vec4(position, 0., 1.);
}


-- Geometry

#version 430 core

// Contour lines in world space
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 lightpos;

const float bias = 0.002;

void main()
{
	vec2 pt0 = gl_in[0].gl_Position.xy;
	vec2 pt1 = gl_in[1].gl_Position.xy;
	vec2 offsetvec = pt1 - pt0;
	vec2 normal = normalize(vec2(-offsetvec.y, offsetvec.x));
	vec2 lightnormal = normalize((pt0 + pt1) / 2.0f - lightpos);
	if (dot(normal, lightnormal) < 0) {
		// Facing away from camera
		mat4 vpMatrix = pMatrix * vMatrix;
		
		vec4 dir0 = vec4(pt0 - lightpos,0.,0.);
		vec4 dir1 = vec4(pt1 - lightpos,0.,0.);
		
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

void main()
{
	fragColor = vec4(1., 1., 1., 1.);
}
