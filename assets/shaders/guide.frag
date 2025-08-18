#version 300 es
precision mediump float;
out vec4 FragColor;

in vec3 v_Color;

void main()
{
    // The final color is simply the interpolated color from the vertex shader.
    FragColor = vec4(v_Color, 1.0);
}