//#include <windows.h>
#include"process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cmath>
#include <string>
#include"glframework/core.h"
#include"glframework/shader.h"
#include"glframework/texture.h"
#include"wrapper/GLErrorCheck.h"
#include"application/Application.h"
#include"application/AudioPlayer.h"
#include"application/TextDisplay.h"
#include"game/render.h"
#include"game/state.h"
#include"game/logic.h"
#include"game/picture.h"

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	txtdp->init("assets/fonts/GlowSansSC-Normal-Book.otf");
	rdr->init();
	CrystalBackground::getInstance()->init();
	dtbg->init();

	glfwSwapInterval(0);

	sta->setHexaRadius(0.08f);
	//sta->setSeed();

	//AudioPlayer player("assets/sounds/deepStone/0.wav");
	//player.play(1);
	
	//new AutoDeleteAudioPlayer("assets/sounds/NG.wav");

	Picture pic("assets/textures/title.png",0.0f, 0.0f, 1);
	pic.dataLoad();
	while (aplct->update())
	{
		auto start = std::chrono::steady_clock::now();
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm* local_time = std::localtime(&now_time);

		sta->allState();
		
		rdr->clear();
		CrystalBackground::getInstance()->draw();
		pic.draw();
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
			//std::cout << local_time->tm_year + 1900 << '/' << local_time->tm_mon << '/' << local_time->tm_mday << " " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl << "[WARN]: Can't keep up! Running " << (elapsed - waitTime).count() << "ms or " << (elapsed - waitTime).count() / 8.0 << "tick behind." << std::endl;
		}
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