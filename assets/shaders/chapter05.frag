#version 330 core
//same as chapter04.frag
out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_TintColor;

void main()
{
    FragColor = texture(u_Texture, v_TexCoord) * u_TintColor;
}