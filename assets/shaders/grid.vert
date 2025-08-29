layout (location = 0) in vec3 aPos;

layout (std140) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

out VS_OUT {
    vec3 worldPos;
} v_out;

void main()
{
    v_out.worldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(v_out.worldPos, 1.0);
}