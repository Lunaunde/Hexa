#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;
out vec3 color;
out vec2 uv;
uniform int type;
uniform vec3 uColor;
void main()
{
	gl_Position = vec4(aPos.x ,aPos.y ,aPos.z ,1.0);
	switch(type)
	{
	case 0:
	{
	color=aColor;
	}
	break;
	case 2:
	{
	color=uColor;
	}
	break;
	}
	uv=aUV;
}