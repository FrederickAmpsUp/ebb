#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos; // For passing position to the fragment shader
out vec3 Normal;  // For passing normals to the fragment shader

uniform mat4 model;      // Object's model matrix
uniform mat4 view;       // Camera's view matrix
uniform mat4 projection; // Projection matrix

void main()
{
    // Transform the vertex position
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    vec4 viewPosition = view * worldPosition;
    gl_Position = projection * viewPosition;

    // Pass position and normal to the fragment shader
    FragPos = worldPosition.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space
}
