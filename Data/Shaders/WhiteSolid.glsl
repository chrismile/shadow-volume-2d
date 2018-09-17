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

void main()
{
	gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
