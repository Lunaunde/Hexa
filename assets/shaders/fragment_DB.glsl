//Distorted background
// Fragment Shader
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform vec2 iResolution;    // 屏幕分辨率
uniform float iTime;         // 时间
uniform vec3 colour1 = vec3(0.77, 0.90, 0.11);
uniform vec3 colour2 = vec3(0.85, 0.0, 0.0);
uniform vec3 colour3 = vec3(1.0, 0.0, 0.0);
uniform vec3 colour4 = vec3(0.90, 0.84, 0.0);
uniform int contrast = 5;
uniform float gradual = 2.0;
uniform float width1 = 0.04;
uniform float width2 = 0.04;
uniform float scale1 = 10.0;
uniform float scale2 = 1.0;
uniform vec2 offset = vec2(0.0);
uniform float intensity = 0.2;
uniform float spin_speed = 0.05;
uniform float spin_amount = 1.5;

void main() {
    float speed = iTime * spin_speed;
    vec2 uv = gl_FragCoord.xy / iResolution.y;
    
    // 计算宽高比补偿
    float aspect = iResolution.x / iResolution.y;
    uv.y -= 0.5;
    uv.x -= 0.5 * aspect;
    uv *= 2.0;
    uv += offset;
    
    float uv_len = length(uv);
    float angle = atan(uv.y, uv.x);
    angle -= spin_amount * uv_len;
    angle += speed;
    
    uv = vec2(uv_len * cos(angle), uv_len * sin(angle)) * scale2;
    uv *= scale1;
    
    vec2 uv2 = vec2(uv.x + uv.y);
    for (int i = 0; i < contrast; i++) {
        uv2 += sin(uv);
        uv += vec2(cos(intensity * uv2.y + speed), 
                  sin(intensity * uv2.x - speed));
        uv -= cos(uv.x + uv.y) - sin(uv.x - uv.y);
    }
    
    float paint_res = smoothstep(0.0, gradual, length(uv) / scale1);
    float c3p = 1.0 - min(width2, abs(paint_res - 0.5)) * (1.0 / width2);
    float c_out = max(0.0, (paint_res - (1.0 - width1))) * (1.0 / width1);
    float c_in = max(0.0, -(paint_res - width1)) * (1.0 / width1);
    float c4p = c_out + c_in;
    
    vec3 ret_col = mix(colour1, colour2, paint_res);
    ret_col = mix(ret_col, colour3, c3p);
    ret_col = mix(ret_col, colour4, c4p);
    
    FragColor = vec4(ret_col, 1.0);
}