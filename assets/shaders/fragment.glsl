#version 460 core
out vec4 FragColor;
in vec3 color;
in flat uint colorType;
void main()
{
	switch(colorType)
	{
	case 0:
	case 1:
	case 2:
	{
		FragColor = vec4(color,1.0);
	}
	break;
	default:
	{
		FragColor = vec4(0.5,0.5,0.5,1.0);
	}
	break;
	}
}