#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord; // Renamed from TexCoord for clarity (v_ for varying/in)

uniform sampler2D u_Texture;
uniform vec4 u_TintColor;

void main()
{
    FragColor = texture(u_Texture, v_TexCoord) * u_TintColor;
}