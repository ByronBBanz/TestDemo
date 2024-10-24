#version 330 core

// interpolated values from the vertex shaders
in vec3 vColor;
flat in vec3 vFlatColor;

// uniform input data
uniform bool uFlatShading;

// output data
out vec3 fColor;

void main()
{
	if(uFlatShading)
		fColor = vFlatColor;
	else
		fColor = vColor;
}
