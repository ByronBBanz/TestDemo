#version 330 core

// input data
layout(location = 0) in vec3 aPosition;

void main()
{
	// set vertex position
    gl_Position = vec4(aPosition, 1.0f);

	// set point size
	gl_PointSize = 10;
}
