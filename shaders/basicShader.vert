#version 330 core

in vec3 vertex;
in vec3 color;

uniform mat4 TG;
uniform mat4 PM;
uniform mat4 VM;

uniform vec3 bend;

out vec3 fcolor;

void main()  {
    fcolor = color;
    if (vertex.y >= 25 && vertex.y <= 35) {
        gl_Position = PM * VM * TG * vec4 (vertex + bend, 1.0);
    } else {
        gl_Position = PM * VM * TG * vec4 (vertex, 1.0);
    }
}
