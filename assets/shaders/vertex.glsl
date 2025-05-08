#version 460 core
layout (location = 0) in uint aPositionType; //0:normal 1:2D rotate 2:fake 3D rotate
layout (location = 1) in vec3 aPos;

layout (location = 8) in uint aColorType; // 0:fColor 1:uniform 2:0xColor
layout (location = 9) in vec3 aColor;

out vec3 color;
out flat uint colorType;

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
	case 1:
	{
	color=uColor;
	}
	break;
	case 2:
	{
	color.r = aColor.r/255.0;
    color.g = aColor.g/255.0;
    color.b = aColor.b/255.0;
	}
	break;
	}
	colorType=aColorType;
}