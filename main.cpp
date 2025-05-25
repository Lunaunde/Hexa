//#include <windows.h>
#include"process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cmath>
#include"thirdParty/enet/include/enet/enet.h"
#include"glframework/core.h"
#include"wrapper/GLErrorCheck.h"
#include"wrapper/Log.h"
#include"application/Application.h"
#include"game/render.h"
#include"game/state.h"
#include"game/net.h"

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
	if (enet_initialize() != 0)
	{
		std::cerr << "初始化 ENet 失败。\n";
		return -1;
	}

	GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));//设置窗口背景色

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	txtdp->init("assets/fonts/MSYH.TTC");
	rdr->init();
	CrystalBackground::getInstance()->init();
	dtbg->init();

	glfwSwapInterval(0);
	while (aplct->update())
	{
		auto start = std::chrono::steady_clock::now();
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm* local_time = std::localtime(&now_time);

		sta->allState();

		rdr->clear();
		CrystalBackground::getInstance()->draw();
		rdr->dataLoad();
		rdr->draw();
		txtdp->draw();
		txtdp->clearQueue();

		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;

		std::chrono::milliseconds waitTime(8);
		if (elapsed < waitTime)
		{
			std::this_thread::sleep_for(waitTime - elapsed);
		}
		else
		{
			logcout << "[WARN/GAME]: Can't keep up! Running " << (elapsed - waitTime).count() << "ms or " << (elapsed - waitTime).count() / 8.0 << "tick behind." << std::endl;
		}
	}
	aplct->destroy();
	atexit(enet_deinitialize);
	return 0;
}