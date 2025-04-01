#version 460 core
layout (location = 0) in uint aPositionType; //0:normal 1:2D rotate 2:fake 3D rotate
layout (location = 1) in vec3 aPos;

layout (location = 8) in uint aColorType; // 0:color 1:texture 2:uniform
layout (location = 9) in vec3 aColor;
layout (location = 10) in vec2 aUV;

out vec3 color;
out vec2 uv;

uniform vec3 uColor;
void main()
{
	gl_Position = vec4(aPos.x ,aPos.y ,aPos.z ,1.0);
	
	switch(aColorType)
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