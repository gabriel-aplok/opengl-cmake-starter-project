#version 150

in vec4 fPosition;
in vec4 fColor;
in vec4 fLightPosition;
in vec3 fNormal;

out vec4 color;

void main(void)
{       
    vec3 viewDir = normalize(-fPosition.xyz); // View direction
    vec3 normal = normalize(fNormal);
    vec3 lightDir = normalize(fLightPosition.xyz - fPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, normal); // Fixed reflection calculation

    // Material properties
    float ambientStrength = 0.1;
    float diffuseStrength = 0.7;
    float specularStrength = 0.6;
    float shininess = 32.0; // Increased for better specular highlight

    // Lighting calculations
    float ambient = ambientStrength;
    float diffuse = diffuseStrength * max(0.0, dot(normal, lightDir));
    float specular = specularStrength * pow(max(0.0, dot(viewDir, reflectDir)), shininess);

    // Combine lighting components
    color = fColor * vec4(vec3(ambient + diffuse + specular), 1.0);
}