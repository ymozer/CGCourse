layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (std140) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

out vec2 v_TexCoord;

void main()
{
    // Standard transformation
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    v_TexCoord = aTexCoord;
}