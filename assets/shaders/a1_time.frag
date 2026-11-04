#version 430 core

in vec3 colour;
out vec4 frag_colour;

uniform float time;

void main()
{
    vec3 c = 0.5 + 0.5 * vec3(sin(time), sin(time * 2.0), sin(time * 3.0));
    frag_colour = vec4(c, 1.0);
}