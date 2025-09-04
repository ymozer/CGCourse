layout (location = 0) in vec3 aPos;

layout (std140) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

out vec3 worldPos_vs;

void main()
{
    worldPos_vs = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}