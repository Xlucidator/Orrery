#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 color;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 offset = vec3(sin(time)/2.0, 0.0, 0.0);
    gl_Position = projection * view * model * vec4(aPos + offset, 1.0);
    color = (sin(time) + 1.0)/2.0 * aColor;
}