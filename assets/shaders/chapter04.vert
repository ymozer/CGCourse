// Chapter 4 - Color Fragment Shader
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // New color attribute

// We need to pass the color to the fragment shader.
out vec3 v_Color;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    v_Color = aColor; // Pass the color through
}