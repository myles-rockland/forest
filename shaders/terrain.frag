#version 460

//Colour coordinates from last stage
in vec3 Ambient;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in float MixFactor;

//Colour value to send to next stage
out vec4 FragColor;

// Uniforms
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 
uniform vec3 cameraPos;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // Ambient lighting
    vec3 ambient = Ambient * light.ambient;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(FragPos - light.position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;

    // Specular lighting
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    //Set colour of terrain to ambient + diffuse from lighting
    vec3 result = ambient + diffuse + specular;
    //FragColor = vec4(result, 1.0) * texture(texture1, TexCoord);
    FragColor = vec4(result, 1.0) *  mix(texture(texture1, TexCoord), texture(texture2, TexCoord), MixFactor);
}