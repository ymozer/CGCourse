
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
uniform vec3 u_ViewPos;
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform vec4 u_TintColor;
uniform bool u_UseTexture;

void main()
{
    vec3 diffuseColor = material.diffuse;
    if (u_UseTexture) {
        diffuseColor *= texture(u_DiffuseMap, v_TexCoord).rgb;
    }
    
    vec3 ambient = light.ambient * diffuseColor;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(light.position - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    vec3 specularColor = texture(u_SpecularMap, v_TexCoord).rgb;
    
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0) * u_TintColor;
}