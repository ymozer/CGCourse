out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform Light light;
uniform Material material;

uniform sampler2D u_Texture;
uniform vec4 u_TintColor;
uniform vec3 u_ViewPos;
uniform bool u_UseTexture;

void main()
{
    vec3 albedo = material.diffuse;
    if (u_UseTexture) {
        albedo = texture(u_Texture, v_TexCoord).rgb;
    }

    vec3 ambient = light.ambient * albedo;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(light.position - v_FragPos);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * albedo;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0) * u_TintColor;
}