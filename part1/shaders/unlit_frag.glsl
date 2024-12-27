#version 410 core

in vec3 v_vertexColors;

out vec4 color;

// Entry point of program
void main()
{
	color = vec4(v_vertexColors.x, v_vertexColors.y, v_vertexColors.z, 1.0f);
}
