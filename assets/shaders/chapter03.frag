#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_CubeTintColor;

void main()
{
    vec4 textureColor = texture(u_Texture, TexCoord);
    FragColor = textureColor * u_CubeTintColor;
}