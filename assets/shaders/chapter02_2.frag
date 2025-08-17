#version 300 es

precision mediump float;
precision mediump int;

out vec4 FragColor;
uniform vec4 u_TriangleColor;

void main() {
    FragColor = u_TriangleColor;
}
