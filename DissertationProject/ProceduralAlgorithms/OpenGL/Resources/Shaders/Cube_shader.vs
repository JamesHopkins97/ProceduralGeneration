#version 330 core

layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec4 vertexColour;

uniform mat4 model;
uniform mat4 viewProjection;

out vec4 VertexColour;

void main()
{
	VertexColour = vertexColour;

	gl_Position = viewProjection * model * vertexPos;
}