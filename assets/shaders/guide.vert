layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// We use the same transformation matrix to ensure the guide
// is in the same space as our square.
uniform mat4 u_Transform;

out vec3 v_Color;

void main()
{
    gl_Position = u_Transform * vec4(aPos, 1.0);
    v_Color = aColor;
}