
out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Light {
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 attenuation; // .x = constant, .y = linear, .z = quadratic
    vec4 spotParams;  // .x = cos(inner), .y = cos(outer), .z = type, .w = is_flashlight
};

struct Material {
    vec4  tintColor;
    vec3  baseDiffuse;
    vec3  baseSpecular;
    float shininess;
    int   useTexture;
};

#define MAX_LIGHTS 16
layout (std140) uniform LightsUBO {
    Light lights[MAX_LIGHTS];
};

uniform vec3 u_ViewPos;
uniform int u_ActiveLights;
uniform Material material;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    
    vec3 diffuseColor = texture(u_DiffuseMap, v_TexCoord).rgb;
    vec3 specularColor = texture(u_SpecularMap, v_TexCoord).rgb;
    
    if (material.useTexture == 0) {
        diffuseColor = material.baseDiffuse;
        specularColor = material.baseSpecular;
    }
    
    vec3 totalResult = vec3(0.0);
    for(int i = 0; i < u_ActiveLights; i++) {
        totalResult += CalcLight(lights[i], norm, v_FragPos, viewDir, diffuseColor, specularColor);
    }
    
    FragColor = vec4(totalResult, 1.0) * material.tintColor;
}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir;
    float attenuation = 1.0;
    float lightType = light.spotParams.z; // 0=Dir, 1=Point, 2=Spot
    
    if (lightType == 0.0) { // Directional Light
        lightDir = normalize(-light.direction.xyz);
    } else { // Point or Spot Light
        vec3 lightVector = light.position.xyz - fragPos;
        float dist = length(lightVector);
        lightDir = normalize(lightVector);
        attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * (dist*dist));
    }
    
    vec3 ambient = light.ambient.rgb * diffuseColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.rgb * diff * diffuseColor;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular.rgb * spec * specularColor;

    float intensity = 1.0;
    if (lightType == 2.0) { // Spot Light
        float theta = dot(lightDir, normalize(-light.direction.xyz));
        float epsilon = light.spotParams.x - light.spotParams.y;
        intensity = clamp((theta - light.spotParams.y) / epsilon, 0.0, 1.0);
    }

    return ambient + (diffuse + specular) * attenuation * intensity;
}