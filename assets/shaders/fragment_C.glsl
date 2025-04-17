#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uBackground;
uniform vec2 uResolution;
uniform float uTime;
uniform float uEta = 0.666;
uniform float uDistortionStrength = 0.1;
uniform float uNoiseScale = 8.0;
uniform float uQuantization = 1.0; 
uniform float uVerticalStretch = 2.0;
uniform float uSlopeSteepness = 1.5;

// ���������
vec2 random2(vec2 p) {
    return fract(
        sin(vec2(
            dot(p, vec2(127.1, 311.7)),
            dot(p, vec2(269.5, 183.3))
        )) * 43758.5453
    );
}

// Voronoi����
float voronoi(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float minDist = 1.0;
    for(int y = -1; y <= 1; y++) {
        for(int x = -1; x <= 1; x++) {
            vec2 neighbor = vec2(x, y);
            vec2 point = random2(i + neighbor);
            vec2 diff = neighbor + point - f;
            minDist = min(minDist, length(diff));
        }
    }
    return minDist;
}

// ����Voronoi
float fractalVoronoi(vec2 p) {
    float total = 0.0;
    float amplitude = 0.5;
    for(int i = 0; i < 2; i++) {
        total += voronoi(p) * amplitude;
        p *= vec2(2.0, 1.8); // �Ǿ�������
        amplitude *= 0.6;
    }
    return 1.0 - total;
}

vec3 calculateHardNormal(vec2 uv) {
    float d = 0.005;
    vec2 stretchedUV = uv * vec2(uNoiseScale, uNoiseScale / uVerticalStretch);
    float h = fractalVoronoi(stretchedUV);
    
    float dx = fractalVoronoi(stretchedUV + vec2(d, 0.0)) - h;
    float dy = (fractalVoronoi(stretchedUV + vec2(0.0, d)) - h) * 1.5;
    
    vec3 normal = normalize(vec3(-dx*0.8, -dy*uSlopeSteepness, 0.15));
    return floor(normal * uQuantization) / uQuantization;
}

void main() {
    // UV����
    vec2 baseUV = vTexCoord;
    
    // ���ɷ���
    vec2 noiseCoord = baseUV * uNoiseScale + vec2(0.0, uTime * -0.4);
    vec3 normal = calculateHardNormal(noiseCoord*0.2);
    
    // �������
    vec3 viewDir = normalize(vec3((baseUV * 2.0 - 1.0), 1.0));
    vec3 refracted = refract(viewDir, normal, uEta);
    vec2 finalUV = baseUV + refracted.xy * uDistortionStrength;
    
    // ���ܱ�Ե�������Գ������ֻ�ϣ�
    vec2 safeUV = finalUV;
    float edgeBlend = 0.0;
    
    // ˮƽ�߽�
    if(finalUV.x < 0.0 || finalUV.x > 1.0) {
        safeUV.x = clamp(finalUV.x, 0.0, 1.0);
        edgeBlend = max(edgeBlend, 0.5);
    }
    
    // ��ֱ�߽�
    if(finalUV.y < 0.0 || finalUV.y > 1.0) {
        safeUV.y = clamp(finalUV.y, 0.0, 1.0);
        edgeBlend = max(edgeBlend, 0.5);
    }
    
    // ��ϲ���
    vec3 refractColor = texture(uBackground, safeUV).rgb;
    vec3 baseColor = texture(uBackground, baseUV).rgb;
    vec3 finalColor = mix(refractColor, baseColor, edgeBlend);
    
    FragColor = vec4(finalColor, 1.0);
}