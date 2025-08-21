// Chapter 1 - Window Vertex Shader
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // or gl_Position = vec4(aPos, 1.0);
    // This is called swizzling
}