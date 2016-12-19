#version 330 core 
in vec3 fragNor;
in vec3 wPos;
out vec4 color;

uniform mat4 V;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform vec3 MatAmb;
uniform float shine;

void main()
{
    vec3 view = -1 * wPos;
    vec3 lightColor = vec3(1, 1, 1);

    vec3 lightP = vec3(V * vec4(vec3(-2, 2, 2), 0));
    vec3 lightV = lightP - wPos; 
    vec3 reflectionV = normalize(-lightV) + 2 * dot(normalize(lightV), normalize(fragNor)) * normalize(fragNor);
    vec3 specular = MatSpec * pow(max(0, dot(normalize(view), normalize(reflectionV))), shine) * lightColor; 
    vec3 diffuse = MatDif * max(dot(normalize(fragNor), normalize(lightV)), 0) * lightColor;
    vec3 ambient = MatAmb * lightColor;

    color = vec4(diffuse, 1) + vec4(specular, 1) + vec4(ambient, 1);
}
