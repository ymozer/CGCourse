out vec4 FragColor;

in vec3 worldPos_vs;

uniform vec3 u_GridColorFine;
uniform vec3 u_GridColorMajor;
uniform float u_FadeStart;
uniform float u_FadeEnd;
uniform float u_LinePixelWidth;

uniform vec3 u_GridColorOriginX;
uniform vec3 u_GridColorOriginZ;


vec2 aaLine(vec2 pos)
{
    vec2 pixelSize = fwidth(pos);
    vec2 thickness = u_LinePixelWidth * pixelSize;
    vec2 distToLine = min(fract(pos), 1.0 - fract(pos));
    return 1.0 - smoothstep(thickness - pixelSize, thickness + pixelSize, distToLine);
}

void main()
{
    float d = length(worldPos_vs.xz);
    float fade = 1.0 - smoothstep(u_FadeStart, u_FadeEnd, d);

    vec2 pos = worldPos_vs.xz;

    vec2 fineIntensity  = aaLine(pos);
    vec2 majorIntensity = aaLine(pos * 0.1);
    
    vec2 originDist = abs(pos);
    vec2 pixelSize = fwidth(pos);
    vec2 originThickness = u_LinePixelWidth * pixelSize * 1.5; // Make origin lines thicker
    vec2 originIntensity = 1.0 - smoothstep(originThickness - pixelSize, originThickness + pixelSize, originDist);

    float finalIntensity = max(fineIntensity.x, fineIntensity.y);
    vec3 finalColor = u_GridColorFine;

    float major = max(majorIntensity.x, majorIntensity.y);
    if (major > finalIntensity) {
        finalIntensity = major;
        finalColor = u_GridColorMajor;
    }

    if (originIntensity.x > finalIntensity) {
        finalIntensity = originIntensity.x;
        finalColor = u_GridColorOriginZ;
    }

    if (originIntensity.y > finalIntensity) {
        finalIntensity = originIntensity.y;
        finalColor = u_GridColorOriginX;
    }

    FragColor = vec4(finalColor, finalIntensity * fade);

    if (FragColor.a < 0.01) {
        discard;
    }
}