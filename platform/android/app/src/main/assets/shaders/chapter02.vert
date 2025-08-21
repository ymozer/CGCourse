// Chapter 2 - Point Vertex Shader
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = 20.0;
}