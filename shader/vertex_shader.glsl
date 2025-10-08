#version 150

in vec3 position;
in vec3 normal;
in vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // Added model matrix for object transformations
uniform vec3 lightPos; // Use vec3 for light position in world space

out vec4 fPosition;
out vec4 fColor;
out vec4 fLightPosition;
out vec3 fNormal;

void main(void)
{
    // Apply model transformation to position
    vec4 worldPosition = model * vec4(position, 1.0);
    fPosition = view * worldPosition;
    fLightPosition = vec4(lightPos, 1.0); // Light position in world space
    fColor = color;
    
    // Transform normal using inverse transpose of model matrix
    fNormal = mat3(transpose(inverse(model))) * normal;
    
    gl_Position = projection * fPosition;
}