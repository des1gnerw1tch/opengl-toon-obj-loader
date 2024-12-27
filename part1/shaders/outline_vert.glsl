#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;
layout(location=2) in vec3 vertexNormal;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection
uniform float u_OutlineExtrudeDistance; // Scale by normals for outline


void main()
{
  vec4 worldSpace = u_ModelMatrix * vec4(position,1.0f);
  vec4 worldSpaceNormalVector = u_ModelMatrix * vec4(vertexNormal, 0.0f); // Normal is a direction, so w=0, will not be effected by translation
  worldSpace += normalize(worldSpaceNormalVector) * u_OutlineExtrudeDistance;
  vec4 newPosition = u_Projection * u_ViewMatrix * worldSpace;
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}


