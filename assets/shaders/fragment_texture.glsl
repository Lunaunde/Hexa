#version 460 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D sampler;
uniform float alpha;
void main()
{
    vec4 color = texture(sampler,uv);
	color.a *= alpha;
	FragColor = color;
}