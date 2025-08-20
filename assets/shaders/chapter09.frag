in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec4 u_SquareTintColor;

void main()
{
    vec4 textureColor = texture(u_Texture, v_TexCoord);
    FragColor = textureColor * u_SquareTintColor;
}