#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  // Usar la posición pasada desde el vertex shader

uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Normalización de la normal y dirección de luz
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Componente difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Componente especular
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Color de la textura
    vec4 texColor = texture(texture1, TexCoord);

    // Combinación de la textura con la iluminación difusa y especular
    vec3 result = ((diffuse + specular)+ vec3(0.1f)) * texColor.rgb;
    

    // Asignar el resultado al color de salida, usando el alpha de la textura
    FragColor = vec4(result, texColor.a);
}
