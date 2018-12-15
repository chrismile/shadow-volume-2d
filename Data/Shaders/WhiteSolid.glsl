-- Vertex

#version 430 core

in vec4 position;

void main()
{
	gl_Position = mvpMatrix * position;
}

-- Fragment

#version 430 core

uniform sampler2D texture;
out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
