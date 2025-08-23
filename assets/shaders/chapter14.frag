out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform bool u_UseTexture;
uniform vec4 u_TintColor;

uniform vec4 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

void main()
{
    vec3 materialColor;
    if (u_UseTexture) {
        materialColor = texture(u_Texture, v_TexCoord).rgb;
    } else {
        materialColor = vec3(1.0);
    }

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor.rgb;

    // Lighting vectors
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor.rgb;

    // Specular (Blinn-Phong) lighting
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * u_LightColor.rgb;

    // Combine lighting components
    vec3 result = (ambient + diffuse) * materialColor + specular;

    // Apply a final tint color
    FragColor = vec4(result, 1.0) * u_TintColor;
}