#version 330 core

// interpolated values from the vertex shaders
in vec3 vPosition;
in vec3 vNormal;

// light properties
struct Light
{
	vec3 pos;
	vec3 dir;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 att;			// constant, linear, quadratic
	float innerAngle;	// spotlight inner angle
	float outerAngle;	// spotlight outer angle
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
uniform vec3 uViewpoint;
uniform Light uLight;
uniform Material uMaterial;

// output data
out vec3 fColor;

void main()
{
	// fragment normal
    vec3 n = normalize(vNormal);

	// vector toward the viewer
	vec3 v = normalize(uViewpoint - vPosition);

	// vector towards the light
    vec3 l = normalize(uLight.pos - vPosition);

	// halfway vector
	vec3 h = normalize(l + v);

	// calculate ambient, diffuse and specular intensities
	vec3 Ia = uLight.La * uMaterial.Ka;
	vec3 Id = vec3(0.0f);
	vec3 Is = vec3(0.0f);
	float dotLN = max(dot(l, n), 0.0f);
	float cosAngle = dot(l, normalize(-uLight.dir));
	float angle = acos(cosAngle);

	if(angle < uLight.outerAngle && dotLN > 0.0f)
	{
		// spotlight falloff 
		float falloff = clamp((uLight.outerAngle - angle)/(uLight.outerAngle - uLight.innerAngle), 0.0f, 1.0f);

		// attenuation
		float dist = length(uLight.pos - vPosition);
		float attenuation = 1.0f / (uLight.att.x + dist * uLight.att.y + dist * dist * uLight.att.z);

		Id = uLight.Ld * uMaterial.Kd * dotLN * attenuation * falloff;
		Is = uLight.Ls * uMaterial.Ks * pow(max(dot(n, h), 0.0f), uMaterial.shininess) * attenuation * falloff;
	}

	// set output color
	fColor = Ia + Id + Is;
}
