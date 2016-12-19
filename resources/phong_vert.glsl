#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform vec3 MatAmb;
uniform float shine;
out vec3 fragNor;
out vec3 wPos;

void main()
{
    fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
    wPos = vec3(V * M * vertPos);

	gl_Position = P * V * M * vertPos;
}
