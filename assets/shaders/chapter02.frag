#version 310 es
#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

out vec4 FragColor;
uniform vec4 u_TriangleColor;

void main() {
    FragColor = u_TriangleColor;
}
