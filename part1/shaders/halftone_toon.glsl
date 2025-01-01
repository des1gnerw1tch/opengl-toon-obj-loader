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
uniform vec3 u_dotColor;
uniform int u_numDotsHorizontally;

// Shadows
uniform mat4 u_ShadowLightSpaceMatrix;
uniform sampler2D u_DepthMap;
in vec4 v_vertexShadowLightPos;

bool IsFragInShadow()	{
	float bias = 0.003f;

	if (v_vertexShadowLightPos.w == 0.0)	{ // Make sure no divide by 0
		return false;
	} 

	vec3 clipSpaceVertexShadowLightPos = v_vertexShadowLightPos.xyz / v_vertexShadowLightPos.w; // What is this doing?
	if (clipSpaceVertexShadowLightPos.z > 1.0 || clipSpaceVertexShadowLightPos.z > 1.0)	{ // Only calculate the shadow if we are within the view at all
		return false;
	}
	clipSpaceVertexShadowLightPos = clipSpaceVertexShadowLightPos * 0.5f + 0.5f; // Put in range from -1 to 1 to 0 to 1
	float depthValueFromMap = texture(u_DepthMap, clipSpaceVertexShadowLightPos.xy).r;
	return depthValueFromMap < clipSpaceVertexShadowLightPos.z - bias;
}

void RenderRawShadows()	{
	mat4 m =  u_ShadowLightSpaceMatrix; // So doesn't optimize out
	if (IsFragInShadow())	{
		color = vec4(0, 0, 1, 1);
	} else {
		color = vec4(1, 1, 1, 1);
	}
}

float GetRadiusOfDot(float dotProdTimesAttenuation, float distanceBetweenDots)	{
	// Dot calculations done in texture space (u-v between 0 and 1)
	float maxDotRadius = distanceBetweenDots * 0.25f;
	
	if (IsFragInShadow())	{
		return maxDotRadius;
	} else {
		return max((1.0f - dotProdTimesAttenuation) * maxDotRadius, 0); 
	}
}

bool IsFragInDot(float radiusOfDot, float distanceBetweenDots)	{
	float hor = v_texCoord.x;
	float ver = v_texCoord.y;

	float distanceYFromDotPos = ((ver / distanceBetweenDots) - floor(ver / distanceBetweenDots)) * distanceBetweenDots; // Vertical distance from closest dot on grid
	if (distanceYFromDotPos > distanceBetweenDots / 2.0f)	{
		distanceYFromDotPos -= distanceBetweenDots;
	}

	float distanceXFromDotPos; // Horizontal distance from closest dot on grid
	if (int((ver + (distanceBetweenDots / 2)) / distanceBetweenDots) % 2 == 0)	{ 
		distanceXFromDotPos = ((hor / distanceBetweenDots) - floor(hor / distanceBetweenDots)) * distanceBetweenDots; 
	} else { // If is in an odd row for dots, shift horizontal dots to the right to make dots in lattice pattern
		float adjustedHor = hor + (distanceBetweenDots / 2);
		distanceXFromDotPos = ((adjustedHor / distanceBetweenDots) - floor(adjustedHor / distanceBetweenDots)) * distanceBetweenDots;
	}
	
	if (distanceXFromDotPos > distanceBetweenDots / 2.0f)	{ // In the case is closer to right side of dot
		distanceXFromDotPos -= distanceBetweenDots;
	}
	
	float distanceFromDotPos = sqrt(pow(distanceXFromDotPos, 2) + pow(distanceYFromDotPos, 2));
	return distanceFromDotPos <= radiusOfDot;
}

void main()
{
	// Dot product and attenuation
	vec3 norm = normalize(v_vertexNormals);
	vec3 vecPointingToLight = normalize(u_Light.worldPosition - v_vertexWorldPosition);
	float dotProd = max(dot(norm, vecPointingToLight), 0.0f);
	float distance = length(u_Light.worldPosition - v_vertexWorldPosition);
	float attenuation = 1.0f / (u_Light.constantFallOff + (u_Light.linearFallOff * distance) + (u_Light.quadtraticFallOff * distance * distance));
	float dotProdTimesAttenuation = dotProd * attenuation; // Bigger number means more light

	// Dot calculation
	float distanceBetweenDots = 1.0f / float(u_numDotsHorizontally);
	float radiusOfDot = GetRadiusOfDot(dotProdTimesAttenuation, distanceBetweenDots);
	vec4 objectColor4;

	if (IsFragInDot(radiusOfDot, distanceBetweenDots))	{
		objectColor4 = vec4(u_dotColor, 1);
	} else {
		objectColor4 = vec4(v_vertexColors, 1);
	}

	if (dotProdTimesAttenuation < 0.95f)	{ // Cel shading factor, have some darker color in addition to the dots
		objectColor4 = vec4(objectColor4.r - 0.1, objectColor4.g - 0.1, objectColor4.b, 1.0f);
	}

	//if (dotProdTimesAttenuation > 1.5f)	{ // Cel shading factor, have some darker color in addition to the dots
	//	objectColor4 = vec4(objectColor4.r + 0.1, objectColor4.g + 0.1, objectColor4.b + 0.2, 1.0f);
		//objectColor4 = vec4(1, 1, 1, 1);
	//}

	//if (IsFragInShadow())	{ // Slight darken of pixel if in shadow
		//objectColor4 = max(objectColor4 - 0.05f, 0.0f);
	//}

	//RenderRawShadows();
	color = objectColor4;
	
	
}
