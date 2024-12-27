#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;
in vec3 v_vertexWorldPosition;
in vec2 v_texCoord;

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
uniform sampler2D u_TextureSampler;

// Entry point of program
void main()
{
	vec4 objectColor4 = texture(u_TextureSampler, v_texCoord);
	vec3 objectColor = vec3(objectColor4.r, objectColor4.g, objectColor4.b);

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
