out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    // Type: 0=Directional, 1=Point, 2=Spot
    int type;

    // Shared Properties
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    // Directional & Spot
    vec3 direction; 

    // Point & Spot
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    // Spot only (Note: these are cosines of the angles)
    float cutOff;
    float outerCutOff;
};

uniform Light light;
uniform Material material;
uniform vec3 u_ViewPos;
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform vec4 u_TintColor;
uniform bool u_UseTexture;

// Function Prototypes
vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcPointLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcSpotLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main()
{
    vec3 diffuseColor = material.diffuse;
    if (u_UseTexture) {
        diffuseColor *= texture(u_DiffuseMap, v_TexCoord).rgb;
    }
    vec3 specularColor = material.specular;
    if (u_UseTexture) {
        specularColor = texture(u_SpecularMap, v_TexCoord).rgb;
    }
    
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    
    vec3 result;
    if(light.type == 0) { // Directional Light
        result = CalcDirLight(norm, viewDir, diffuseColor, specularColor);
    }
    else if(light.type == 1) { // Point Light
        result = CalcPointLight(norm, viewDir, diffuseColor, specularColor);
    }
    else if(light.type == 2) { // Spot Light
        result = CalcSpotLight(norm, viewDir, diffuseColor, specularColor);
    }

    FragColor = vec4(result, 1.0) * u_TintColor;
}


vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir = normalize(-light.direction);
    
    // Ambient
    vec3 ambient = light.ambient * diffuseColor;
    
    // Diffuse (Blinn-Phong)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularColor;
    
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir = normalize(light.position - v_FragPos);
    
    // Ambient
    vec3 ambient = light.ambient * diffuseColor;
    
    // Diffuse (Blinn-Phong)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularColor;
    
    // Attenuation
    float distance = length(light.position - v_FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir = normalize(light.position - v_FragPos);
    
    // Spotlight intensity calculation
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // The base ambient light
    vec3 ambient = light.ambient * diffuseColor;

    vec3 lighting = CalcPointLight(normal, viewDir, diffuseColor, specularColor);
    
    return ambient + (lighting - ambient) * intensity;
}