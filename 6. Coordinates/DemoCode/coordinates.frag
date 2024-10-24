#version 330 core

// uniform variables
uniform vec3 uColor;

// output data
out vec3 fColor;

void main()
{
	// set output color
	fColor = uColor;
}
