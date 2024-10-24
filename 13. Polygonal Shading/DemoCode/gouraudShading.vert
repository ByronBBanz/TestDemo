#version 330 core

// input data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

// light properties
struct Light
{
	vec3 pos;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// material properties
struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float shininess;
};

// uniform input data
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform vec3 uViewpoint;
uniform Light uLight;
uniform Material uMaterial;

// output data
out vec3 vColor;
flat out vec3 vFlatColor;

void main()
{
	// set vertex position
    gl_Position = uModelViewProjectionMatrix * vec4(aPosition, 1.0f);

	// vertex position in world space
	vec3 position = (uModelMatrix * vec4(aPosition, 1.0f)).xyz;

	// fragment normal
    vec3 n = normalize(uNormalMatrix * aNormal);

	// vector toward the viewer
	vec3 v = normalize(uViewpoint - position);

	// vector towards the light
    vec3 l = normalize(uLight.pos - position);

	// reflection vector
	vec3 r = reflect(-l, n);

	// calculate ambient, diffuse and specular intensities
	vec3 Ia = uLight.La * uMaterial.Ka;
	vec3 Id = vec3(0.0f);
	vec3 Is = vec3(0.0f);
	float dotLN = max(dot(l, n), 0.0f);

	if(dotLN > 0.0f)
	{
		Id = uLight.Ld * uMaterial.Kd * dotLN;
	    Is = uLight.Ls * uMaterial.Ks * pow(max(dot(v, r), 0.0f), uMaterial.shininess);
	}
	
	// set output color (attenuation not implemented)
	vColor = Ia + Id + Is;
	vFlatColor = Ia + Id + Is;
}
