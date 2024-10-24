#version 330 core

// input data
layout(location = 0) in vec3 aPosition;

// uniform variables
uniform int uPointSize;
uniform vec3 uPosition;

// output data
out vec3 vColor;

void main()
{
	// set vertex position
    gl_Position = vec4(aPosition + uPosition, 1.0f);

	// set point size
	gl_PointSize = uPointSize;
}
