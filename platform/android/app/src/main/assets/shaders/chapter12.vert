layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// The standard MVP matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 v_TexCoord;

void main()
{
    // Standard transformation
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    v_TexCoord = aTexCoord;
}