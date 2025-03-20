//#include <windows.h>
#include"process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include"glframework/core.h"
#include"glframework/shader.h"
#include"glframework/texture.h"
#include"wrapper/GLErrorCheck.h"
#include"application/Application.h"
#include"application/AudioPlayer.h"
#include"game/render.h"
#include"game/state.h"
#include"game/logic.h"

int xPos, yPos;
void onKey(int key, int scancode, int action, int mods)
{
	//std::cout << "按键检测:" << key << " " << action << " " << mods << std::endl;
}
void onResize(int width, int height)
{
	GL_CALL(glViewport(0, 0, width, height));
	//std::cout << "窗体大小:" << width << " " << height << std::endl;
}
void onScroll(double xoffset, double yoffset)
{
	//	std::cout << "鼠标滚轮:" << xoffset << " " << yoffset << std::endl;
}

int main()
{
//	HWND hWnd = GetConsoleWindow();
//	ShowWindow(hWnd, SW_HIDE);

	if (!aplct->init())
		return -1;
	GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));//设置窗口背景色

	Render::init();
	Logic::buildLevel(5,sta);

	while (aplct->update())
	{
		Logic::clickChangeHexa(sta->getHexas(), 0.05f);
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
		Render::hexasRender(sta->getHexas(), 0.05f);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	aplct->destroy();

	return 0;
}

//	AudioPlayer bgm("D:\\space.wav");
//
//	bgm.play(-1);
//	//player1.setVolume(0.1f);
//
//	bgm.stop();