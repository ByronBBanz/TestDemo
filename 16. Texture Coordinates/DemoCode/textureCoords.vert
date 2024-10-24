#version 330 core

// input data (different for all executions of this shader)
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

// output data (will be interpolated for each fragment)
out vec2 vTexCoord;

void main()
{
	// set vertex position
    gl_Position = vec4(aPosition, 1.0f);

	// interpolate texture coordinate
	vTexCoord = aTexCoord;
}

