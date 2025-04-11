#version 460 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
uniform sampler2D sampler;
uniform int colorType;
void main()
{
	switch(colorType)
	{
	case 0:
	case 2:
	case 3:
	{
		FragColor = vec4(color,1.0);
	}
	break;
	case 1:
	{
		FragColor = texture(sampler,uv);
	}
	break;
	default:
	{
		FragColor = vec4(0.5,0.5,0.5,1.0);
	}
	break;
	}
}