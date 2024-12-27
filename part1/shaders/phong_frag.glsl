#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;
in vec3 v_vertexWorldPosition;

out vec4 color;

struct PointLight	{
	vec3 worldPosition; // World position of light

	vec3 ambientColor;
	float ambientStrength; // Between 0 and 1
	vec3 diffuseColor;
	vec3 specularColor;

	float constantFallOff;
	float linearFallOff;
	float quadtraticFallOff;
};

uniform PointLight u_Light;
uniform vec3 u_CameraWorldPos;

// Entry point of program
void main()
{
	float objectR = (v_vertexNormals.x + 1) / 2; // Because normals between 1f and -1f, want to get color between 0f and 1f
	float objectG = (v_vertexNormals.y + 1) / 2;
	float objectB = (v_vertexNormals.z + 1) / 2;
	vec3 objectColor = vec3(objectR, objectG, objectB);

	// Multiply blending of colors for now
	// Ambient
	vec3 objectAmbientColorBlend =  (objectColor * u_Light.ambientColor) * u_Light.ambientStrength;

	// Diffuse
	vec3 norm = normalize(v_vertexNormals);
	vec3 vecPointingToLight = normalize(u_Light.worldPosition - v_vertexWorldPosition);
	float dotProd = max(dot(norm, vecPointingToLight), 0.0f);
	float distance = length(u_Light.worldPosition - v_vertexWorldPosition);
	float attenuation = 1.0f / (u_Light.constantFallOff + (u_Light.linearFallOff * distance) + (u_Light.quadtraticFallOff * distance * distance));
	vec3 objectDiffuseColorBlend = (objectColor * u_Light.diffuseColor) * dotProd * attenuation;

	// Specular
	vec3 pointingToCamera = normalize(u_CameraWorldPos - v_vertexWorldPosition);
	vec3 lightBounceOffObject = reflect(vecPointingToLight, norm); // Should already be normalized as vecPointingToLight is normalized
	float specular = pow(max(dot(lightBounceOffObject, pointingToCamera), 0.0f), 32); // The higher the power, the shinier the object
	vec3 objectSpecularColorBlend = specular * u_Light.specularColor;

	// Final color output
	color = vec4(objectSpecularColorBlend + objectDiffuseColorBlend + objectAmbientColorBlend, 1.0f);
}
