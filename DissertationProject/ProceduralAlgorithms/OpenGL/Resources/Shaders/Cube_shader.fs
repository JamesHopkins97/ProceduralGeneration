#version 330 core

in vec4 VertexColour;

out vec4 FragColor;

void main()
{
	FragColor = VertexColour;
}