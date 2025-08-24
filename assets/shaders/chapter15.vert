
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

layout (std140) uniform CameraUBO {
    mat4 view;
    mat4 projection;
};
uniform mat4 model;

uniform mat3 u_NormalMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    v_FragPos = vec3(model * vec4(aPos, 1.0));
    v_Normal = u_NormalMatrix * aNormal;
    v_TexCoord = aTexCoord;
}