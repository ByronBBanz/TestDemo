#version 330 core

// interpolated values from the vertex shaders
in vec2 vTexCoord;

// uniform input data
uniform sampler2D uTextureSampler;

// output data
out vec3 fColor;

void main()
{
	fColor = texture(uTextureSampler, vTexCoord).rgb;
}