#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;
layout(location=2) in vec3 vertexNormals;
layout(location=3) in vec2 texCoord;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection

// Pass into the fragment shader
out vec3 v_vertexColors;
out vec3 v_vertexNormals;
out vec3 v_vertexWorldPosition;
out vec2 v_texCoord;

// Shadows
uniform mat4 u_ShadowLightSpaceMatrix;
out vec4 v_vertexShadowLightPos;

void main()
{
  v_vertexColors = vertexColors;
  v_vertexNormals= vertexNormals;
  v_vertexWorldPosition = vec3(u_ModelMatrix * vec4(position, 1.0f)); // 1 in w because this is a point
  v_texCoord = texCoord;
  v_vertexShadowLightPos = u_ShadowLightSpaceMatrix * u_ModelMatrix * vec4(position, 1.0f);

  vec4 newPosition = u_Projection * u_ViewMatrix * vec4(v_vertexWorldPosition, 1.0f); // 1 in w because this is a point
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}


