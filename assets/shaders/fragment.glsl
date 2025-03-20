#version 460 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
uniform int type;
uniform sampler2D sampler;
uniform vec3 uColor;
void main()
{
	switch(type)
	{
	case 0:
	{
		FragColor = vec4(color,1.0);
	}
	break;
	case 1:
	{
		FragColor = texture(sampler,uv);
	}
	break;
	case 2:
	{
		FragColor = vec4(uColor,1.0);
	}
	break;
	default:
	{
		FragColor = vec4(0.5,0.5,0.5,1.0);
	}
	break;
	}
}