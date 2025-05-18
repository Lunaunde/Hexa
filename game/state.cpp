#include"state.h"
#include<vector>
#include<cmath>
#include<iostream>
#include<fstream>
#include<filesystem>
#include"../application/Application.h"
#include"../application/TextDisplay.h"
#include"../glframework/core.h"
#include"render.h"
#include"logic.h"

const double PI = 3.141592653589793238463;
State* State::instance = nullptr;

HexaColorBackup::HexaColorBackup(std::vector<Hexa>& hexas)
{
	for (int i = 0; i < hexas.size(); i++)
	{
		mHexaColorBackup.push_back(hexas[i].getColor());
	}
}
HexaColorBackup::HexaColorBackup()
{
}
HexaColorBackup::~HexaColorBackup()
{
}
void HexaColorBackup::restore(std::vector<Hexa>& hexas)
{
	for (int i = 0; i < mHexaColorBackup.size(); i++)
	{
		hexas[i].setColor(mHexaColorBackup[i]);
	}
}
void HexaColorBackup::softRestore(std::vector<Hexa>& hexas)
{
	for (int i = 0; i < mHexaColorBackup.size(); i++)
	{
		hexas[i].freeChangeColor(mHexaColorBackup[i]);
	}
}

State::State() : mHexas(), mAnsSteps(), mPlayerSteps(), mHexaColorBackup()
{
}
State::~State()
{
	delete instance;
}

void State::allState()
{
	hexasScaleAdd();
	for (auto& hexaButton : mHexaButtons)
		hexaButton.loadAllText();
	if (mHexaButtons.size() > 0 && mHexaButtons[0].isDeleted())
		mHexaButtons.clear();
	if (mHexas.size() > 0 && mHexas[0].isDeleted())
		mHexas.clear();

	fMouseOnButton = false;
	for (auto& hexaButton : mHexaButtons)
	{
		if (hexaButton.ifPositionInHexa(getCursorXPos(), getCursorYPos(), 1, 0) && sta->getMouseAction() == GLFW_PRESS)
			adap->addAudio("assets/sounds/click.wav");
		if (hexaButton.ifPositionInHexa(getCursorXPos(), getCursorYPos(), 1, 0))
		{
			if (mouseOnButton != &hexaButton)
			{
				mouseOnButton = &hexaButton;
				adap->addAudio("assets/sounds/mouseOn.wav");
			}
			fMouseOnButton = true;
		}
	}

	switch (mState)
	{
	case 0:
		menuState();
		break;
	case 1:
		gameState();
		break;
	case 2:
		lostState();
		break;
	case 3:
		sandboxSetState();
		break;
	case 4:
		sandboxPlayState();
		break;
	case 5:
		onlineState();
		break;
	case 6:
        onlineHosterSandboxState();
		break;
	}

	if (!fMouseOnButton)
		mouseOnButton = nullptr;

	clearMouse();
	clearKey();
}
void State::menuState()
{
	if (!mStateChanging)
	{
		if (mHexaButtons.size() == 0)
		{

			mPictures["title"]->zoomIn();
			mHexaButtons.push_back(HexaButton(-0.25f, -0.3f, 0.15));
			mHexaButtons.push_back(HexaButton(0.0f, -0.3f, 0.15));
			mHexaButtons.push_back(HexaButton(0.25f, -0.3f, 0.15));
			mHexaButtons.push_back(HexaButton(-0.6f, -0.3f, 0.15));
			mHexaButtons.push_back(HexaButton(0.6f, -0.3f, 0.15));

			mHexaButtons[0].setColor(Color(124, 252, 0));
			mHexaButtons[0].addText(L"简单模式", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[1].setColor(Color(135, 206, 235));
			mHexaButtons[1].addText(L"普通模式", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[2].setColor(Color(255, 127, 0));
			mHexaButtons[2].addText(L"困难模式", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[3].setColor(Color(248, 213, 104));
			mHexaButtons[3].addText(L"沙盒模式", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[4].setColor(Color(138, 43, 226));
			mHexaButtons[4].addText(L"对战模式", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);

			std::wstring text;
			text = L"最高记录:";
			for (int i = 0; i < 3; i++)
			{
				mHexaButtons[i].addText(text + std::to_wstring(mHRLevelBase[i]) + L"_" + std::to_wstring(mHRLevel[i]), 0.0f, -0.08f, 0.16f, 1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		else
		{
			bool startGame = false;
			if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				txtdp->loadText(L"无时间限制,无重置限制,关底无加强", 0.0f, -0.15f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 0;
					mNum = 1;
					CrystalBackground::getInstance()->colorTo(0);
				}
			}
			if (mHexaButtons[1].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				txtdp->loadText(L"有时间限制,无重置限制,有关底加强", 0.0f, -0.15f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 1;
					mNum = 1;
					CrystalBackground::getInstance()->colorTo(1);
				}
			}
			if (mHexaButtons[2].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				txtdp->loadText(L"时间更加少,有重置限制,有双倍加强", 0.0f, -0.15f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 2;
					mNum = 1;
					CrystalBackground::getInstance()->colorTo(2);
				}
			}
			if (mHexaButtons[3].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				txtdp->loadText(L"自由设置地图大小和加强", 0.0f, -0.15f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mNum = 3;
					CrystalBackground::getInstance()->colorTo(3);
				}
			}
			if (mHexaButtons[4].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				txtdp->loadText(L"进入联网对战模式", 0.0f, -0.15f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mNum = 5;
					CrystalBackground::getInstance()->colorTo(4);
				}
			}
			if (startGame)
			{
				mStateChanging = true;
				for (auto& hexa : mHexaButtons)
				{
					hexa.deleteModeOn();
				}
				mPictures["title"]->zoomOut();
			}
		}
	}
	else
	{
		if (mHexaButtons.size() == 0)
		{
			mStateChanging = false;
			mPictures["ghost"]->zoomIn();
			switch (mNum)
			{
			case 1:
				if (mDifficulty > 0)
					mPictures["clock"]->zoomIn();
				if (mDifficulty == 2)
					mPictures["reset"]->zoomIn();

				mLevel = 1;
				mLevelBase = 2;
				mState = 1;
				newRecord = false;
				break;
			case 3:
				mState = 3;
				mLevelBase = 2;
				break;
			case 5:
				mState = 5;
				mPictures["namebar"]->zoomIn();
				break;
			}
			randSeed();
			mNum = 0;
		}
	}
}
void State::gameState()
{
	float timeLimit = 0;
	if (mDifficulty == 1)
		timeLimit = mLevelBase * 30;
	if (mDifficulty == 2)
		timeLimit = mLevelBase * 20;
	float leftTime = timeLimit - (glfwGetTime() - mClock);

	float sinScale = abs(sin(mPictures["ghost"]->getZoom() * PI / 2));

	if (mPictures["rotationS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
		txtdp->loadText(L"地图会顺时针旋转", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	if (mPictures["colorChangeS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
		txtdp->loadText(L"颜色会随时间变换", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	if (mPictures["colorfulS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
	{
		txtdp->loadText(L"六边形具有三种颜色", 0.70f, 0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
		txtdp->loadText(L"变化为红 黄 蓝 红", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
		txtdp->loadText(L"且六边形可通过两次", 0.70f, -0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	}

	txtdp->loadText(L"关卡:" + std::to_wstring(mLevelBase) + L"_" + std::to_wstring(mLevel), -0.885f, 0.9f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	txtdp->loadText(L"点击六边形改变颜色", -0.70f, 0.2f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	txtdp->loadText(L"每层内颜色相同通关", -0.70f, 0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	txtdp->loadText(L"按R键可以重制步数", -0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	txtdp->loadText(L"鼠标移动到右上角可", -0.70f, -0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	txtdp->loadText(L"查看该图标对于效果", -0.70f, -0.2f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	if (mDifficulty > 0)
		txtdp->loadText(std::to_wstring((int)leftTime), -0.85f, 0.74f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	if (mDifficulty == 2)
		txtdp->loadText(std::to_wstring(mReset), -0.85f, 0.56f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);

	if (!mStateChanging)
	{
		if (mHexas.size() == 0)
		{
			setRotationMode(false);
			setColorChangeMode(false);
			setColorMode(2);
			if (mLevelBase < 4)
				sta->setHexaRadius(0.1);
			else
				sta->setHexaRadius(0.3 / mLevelBase);
			if (mLevel == 3)
			{
				switch (mDifficulty)
				{
				case 1:
				{
					switch (genInt(0, 2))
					{
					case 0:
						setRotationMode(true);
						break;
					case 1:
						setColorChangeMode(true);
						break;
					case 2:
						setColorMode(3);
						break;
					}
				}
				break;
				case 2:
				{
					setRotationMode(true);
					setColorChangeMode(true);
					setColorMode(3);
					switch (genInt(0, 2))
					{
					case 0:
						setRotationMode(false);
						break;
					case 1:
						setColorChangeMode(false);
						break;
					case 2:
						setColorMode(2);
						break;
					}
				}
				}

				if (getRotationMode())
					mPictures["rotationS"]->zoomIn();
				if (getColorChangeMode())
					mPictures["colorChangeS"]->zoomIn();
				if (getColorMode() == 3)
					mPictures["colorfulS"]->zoomIn();
			}
			Logic::buildLevel(mLevelBase);
			mReset = 5;
			mClock = glfwGetTime();
		}
		else
		{
			if (!Logic::finishPuzzle(getHexas(), mLevelBase))
				Logic::playerStepCheck();
			Logic::reloadLevel();
			//Logic::showAnswer();

			colorChange();

			if ((Logic::finishPuzzle(getHexas(), mLevelBase) == true && mHexas[0].getDeleteMode() == false) || (sta->getKey() == GLFW_KEY_RIGHT_ALT && sta->getKeyAction() == GLFW_PRESS))
			{
				if (mLevelBase > mHRLevelBase[mDifficulty])
				{
					mHRLevelBase[mDifficulty] = mLevelBase;
					mHRLevel[mDifficulty] = mLevel;
					newRecord = true;
				}
				else if (mLevelBase == mHRLevelBase[mDifficulty] && mLevel > mHRLevel[mDifficulty])
				{
					mHRLevel[mDifficulty] = mLevel;
					newRecord = true;
				}
				if (newRecord)
				{
					std::string data;
					std::ofstream file("data.txt");
					if (file.is_open())
						for (int i = 0; i < 3; i++)
							data += std::to_string(mHRLevelBase[i]) + " " + std::to_string(mHRLevel[i]) + "\n";
					file.write(data.c_str(), strlen(data.c_str()));
				}
				if ((mPlayerSteps.size() > 0 && mPlayerSteps[mPlayerSteps.size() - 1]->getScale() == 1.0f) || (sta->getKey() == GLFW_KEY_RIGHT_ALT && sta->getKeyAction() == GLFW_PRESS))
				{
					finishSound();
					if (mPictures["rotationS"]->getZoom() > 0.0f)
						mPictures["rotationS"]->zoomOut();
					if (mPictures["colorChangeS"]->getZoom() > 0.0f)
						mPictures["colorChangeS"]->zoomOut();
					if (mPictures["colorfulS"]->getZoom() > 0.0f)
						mPictures["colorfulS"]->zoomOut();
					for (auto& hexa : mHexas)
						hexa.deleteModeOn();
					mPlayerSteps.clear();
					mLevel++;
					if (mLevel >= 4)
					{
						mLevel = 1;
						mLevelBase++;
					}
				}
			}
			bool lost = (sta->getKey() == GLFW_KEY_ESCAPE && sta->getKeyAction() == GLFW_PRESS) || (mDifficulty > 0 && leftTime <= 0) || (mDifficulty == 2 && mReset == 0);
			if (lost)
			{
				if (mPictures["rotationS"]->getZoom() > 0.0f)
					mPictures["rotationS"]->zoomOut();
				if (mPictures["colorChangeS"]->getZoom() > 0.0f)
					mPictures["colorChangeS"]->zoomOut();
				if (mPictures["colorfulS"]->getZoom() > 0.0f)
					mPictures["colorfulS"]->zoomOut();
				for (auto& hexa : mHexas)
					hexa.deleteModeOn();
				mPlayerSteps.clear();
				mStateChanging = true;
				mPictures["ghost"]->zoomOut();
				if (mDifficulty > 0)
					mPictures["clock"]->zoomOut();
				if (mDifficulty == 2)
					mPictures["reset"]->zoomOut();
				CrystalBackground::getInstance()->colorBack();
			}
		}
	}
	else
	{
		if (mHexas.size() == 0)
		{
			mState = 2;
			mStateChanging = false;
		}
	}
}
void State::lostState()
{
	if (!mStateChanging)
	{
		if (mHexaButtons.size() == 0)
		{
			mHexaButtons.push_back(HexaButton(-0.25f, -0.2f, 0.15));
			mHexaButtons.push_back(HexaButton(0.25f, -0.2f, 0.15));

			mHexaButtons[0].setColor(Color(0, 0, 0));
			mHexaButtons[0].addText(L"返回主界面", 0.0f, -0.025f, 0.32f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[1].setColor(Color(255, 255, 255));
			mHexaButtons[1].addText(L"重新开始", 0.0f, -0.025f, 0.32f, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
				{
					mNum = 0;
					mStateChanging = true;
					for (auto& hexaButton : mHexaButtons)
						hexaButton.deleteModeOn();
				}
				if (mHexaButtons[1].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
				{
					mNum = 1;
					mStateChanging = true;
					for (auto& hexaButton : mHexaButtons)
						hexaButton.deleteModeOn();
					CrystalBackground::getInstance()->colorTo(mDifficulty);
				}
			}
		}
	}
	else
	{
		if (mHexaButtons.size() == 0)
		{
			mStateChanging = false;
			if (mNum == 0)
			{
				mState = 0;
			}
			else
			{
				mPictures["ghost"]->zoomIn();
				if (mDifficulty > 0)
					mPictures["clock"]->zoomIn();
				if (mDifficulty == 2)
					mPictures["reset"]->zoomIn();
				mLevel = 1;
				mLevelBase = 2;
				mState = 1;
				randSeed();
				newRecord = false;
			}
		}
	}
	if (mHexaButtons.size() > 0)
	{
		float sinScale = abs(sin(mHexaButtons[0].getScale() * PI / 2));
		if (newRecord)
		{
			txtdp->loadText(L"新历史记录!", 0.0f, 0.2f, 0.32f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			txtdp->loadText(L"当前模式历史最高记录:" + std::to_wstring(mHRLevelBase[mDifficulty]) + L"_" + std::to_wstring(mHRLevel[mDifficulty]), 0.0f, 0.2f, 0.32f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		int finishLevel = mLevel - 1;
		int finishLevelBase = mLevelBase;
		if (finishLevel <= 0 && mLevelBase > 2)
		{
			finishLevelBase--;
			finishLevel = 3;
		}
		txtdp->loadText(L"完成关卡:" + std::to_wstring(finishLevelBase) + L"_" + std::to_wstring(finishLevel), 0.0f, 0.0f, 1.0f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
	}
}
void State::sandboxSetState()
{
	if (!mStateChanging)
	{
		for (auto& picture : mPictures)
		{
			if (picture.second->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				adap->addAudio("assets/sounds/click.wav");
			if (picture.second->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				if (mouseOnButton != &picture)
				{
					mouseOnButton = &picture;
					adap->addAudio("assets/sounds/mouseOn.wav");
				}
				fMouseOnButton = true;
			}
		}
		if (!fMouseOnButton)
			mouseOnButton = nullptr;

		if (mHexaButtons.size() == 0)
		{
			mHexaButtons.push_back(HexaButton(0.0f, -0.3f, 0.10));

			mHexaButtons[0].setColor(Color(255, 255, 255));
			mHexaButtons[0].addText(L"创建并开始", 0.0f, -0.025f, 0.20f, 0.0f, 0.0f, 0.0f, 1.0f);

			mPictures["left"]->zoomIn();
			mPictures["right"]->zoomIn();

			if (!mSandBoxMode[0])
				mPictures["rotation"]->setAlpha(0.2f);
			else
				mPictures["rotation"]->setAlpha(1.0f);
			mPictures["rotation"]->zoomIn();
			if (!mSandBoxMode[1])
				mPictures["colorChange"]->setAlpha(0.2f);
			else
				mPictures["colorChange"]->setAlpha(1.0f);
			mPictures["colorChange"]->zoomIn();
			if (!mSandBoxMode[2])
				mPictures["colorful"]->setAlpha(0.2f);
			else
				mPictures["colorful"]->setAlpha(1.0f);
			mPictures["colorful"]->zoomIn();
		}

		{
			if (mPictures["rotation"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"地图会顺时针旋转", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[0])
						mPictures["rotation"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["rotation"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[0] = !mSandBoxMode[0];
				}
			}
			if (mPictures["colorChange"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"颜色会随时间变换", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[1])
						mPictures["colorChange"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["colorChange"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[1] = !mSandBoxMode[1];
				}
			}
			if (mPictures["colorful"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"六边形具有三种颜色", 0.0f, 0.7f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"变化为红 黄 蓝 红", 0.0f, 0.6f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"且六边形可通过两次", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[2])
						mPictures["colorful"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["colorful"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[2] = !mSandBoxMode[2];
				}
			}
			if (mPictures["left"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				if (mLevelBase > 2)
					mLevelBase--;
			if (mPictures["right"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				if (mLevelBase < 12)
					mLevelBase++;
			if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				mStateChanging = true;
				mNum = 4;
			}
		}

		if (sta->getKey() == GLFW_KEY_ESCAPE && sta->getKeyAction() == GLFW_PRESS)
		{
			mStateChanging = true;
			mNum = 0;
			CrystalBackground::getInstance()->colorBack();
		}

		if (mStateChanging)
		{
			mHexaButtons[0].deleteModeOn();
			mPictures["left"]->zoomOut();
			mPictures["right"]->zoomOut();
			mPictures["rotation"]->zoomOut();
			mPictures["colorChange"]->zoomOut();
			mPictures["colorful"]->zoomOut();
		}
	}
	else
	{
		if (mHexaButtons.size() == 0)
		{
			mStateChanging = false;
			mState = mNum;
			if (mNum == 4)
				mPictures["ghost"]->zoomIn();
		}
	}

	float sinScale = sin(mPictures["left"]->getZoom() * 3.1415926 / 2);
	txtdp->loadText(std::to_wstring(mLevelBase), 0, -0.25, 0.8 * sinScale, 1.0, 1.0, 1.0, 1.0);
	txtdp->loadText(L"点击设置地图大小与加强,ESC返回主菜单", 0, 0.4, 0.4 * sinScale, 1.0, 1.0, 1.0, 1.0);
}
void State::sandboxPlayState()
{
	float sinScale = sin(mPictures["ghost"]->getZoom() * 3.1415926 / 2);
	txtdp->loadText(L"关卡完成或按ESC返回设置界面", 0, 0.9, 0.4 * sinScale, 1.0, 1.0, 1.0, 1.0);

	if (!mStateChanging)
	{
		if (mHexas.size() == 0)
		{
			if (mLevelBase < 4)
				sta->setHexaRadius(0.1);
			else
				sta->setHexaRadius(0.3 / mLevelBase);

			if (mSandBoxMode[0])
			{
				setRotationMode(true);
				mPictures["rotationS"]->zoomIn();
			}
			else
				setRotationMode(false);
			if (mSandBoxMode[1])
			{
				setColorChangeMode(true);
				mPictures["colorChangeS"]->zoomIn();
			}
			else
				setColorChangeMode(false);
			if (mSandBoxMode[2])
			{
				setColorMode(3);
				mPictures["colorfulS"]->zoomIn();
			}
			else
				setColorMode(2);
			Logic::buildLevel(mLevelBase);
		}
		else
		{
			if (mPictures["rotationS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
				txtdp->loadText(L"地图会顺时针旋转", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
			if (mPictures["colorChangeS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
				txtdp->loadText(L"颜色会随时间变换", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
			if (mPictures["colorfulS"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"六边形具有三种颜色", 0.70f, 0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"变化为红 黄 蓝 红", 0.70f, 0.0f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"且六边形可通过两次", 0.70f, -0.1f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
			}

			if (!Logic::finishPuzzle(getHexas(), mLevelBase))
				Logic::playerStepCheck();

			colorChange();

			bool back = false;

			if (Logic::finishPuzzle(getHexas(), mLevelBase) == true && mHexas[0].getDeleteMode() == false && mPlayerSteps.size() > 0 && mPlayerSteps[mPlayerSteps.size() - 1]->getScale() == 1.0f)
			{
				finishSound();
				back = true;
			}
			if (sta->getKey() == GLFW_KEY_ESCAPE && sta->getKeyAction() == GLFW_PRESS)
				back = true;
			if (back == true)
			{
				mStateChanging = true;
				mPictures["ghost"]->zoomOut();
				if (mPictures["rotationS"]->getZoom() > 0.0f)
					mPictures["rotationS"]->zoomOut();
				if (mPictures["colorChangeS"]->getZoom() > 0.0f)
					mPictures["colorChangeS"]->zoomOut();
				if (mPictures["colorfulS"]->getZoom() > 0.0f)
					mPictures["colorfulS"]->zoomOut();
				for (auto& hexa : mHexas)
					hexa.deleteModeOn();
				mPlayerSteps.clear();
			}
		}
	}
	else
	{
		if (mHexas.size() == 0)
		{
			mStateChanging = false;
			mState = 3;
		}
	}
}
void State::onlineState()
{
	if (!mStateChanging)
	{
		if (mHexaButtons.size() == 0)
		{
			mHexaButtons.push_back(HexaButton(-0.4f, 0.0f, 0.15));
			mHexaButtons.push_back(HexaButton(0.0f, 0.0f, 0.15));
			mHexaButtons.push_back(HexaButton(0.4f, 0.0f, 0.15));
			mHexaButtons[0].setColor(Color(124, 252, 0));
			mHexaButtons[0].addText(L"创建房间", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[1].setColor(Color(135, 206, 235));
			mHexaButtons[1].addText(L"加入房间", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
			mHexaButtons[2].setColor(Color(255, 127, 0));
			mHexaButtons[2].addText(L"搜索房间", 0.0f, -0.025f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
		{
			if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				mServer = new Server(12345);
				if (mServer->getUseful() == true)
				{
					mStateChanging = true;
					mNum = 6;
				}
				mClient = new Client( { 127,0,0,1 } , 12345);
			}
		}
		if (sta->getKey() == GLFW_KEY_ESCAPE && sta->getKeyAction() == GLFW_PRESS)
		{
			mStateChanging = true;
			mNum = 0;
			CrystalBackground::getInstance()->colorBack();
		}
		if (mStateChanging == true)
		{
			for (auto& hexaButtons : mHexaButtons)
				hexaButtons.deleteModeOn();
			mPictures["namebar"]->zoomOut();
		}
	}
	else
	{
		if (mHexaButtons.size() == 0)
		{
			mStateChanging = false;
			switch (mNum)
			{
			case 0:
				mState = 0;
				break;
			case 6:
				mState = 6;
				break;
			}
		}
	}
	float sinScale = sin(mPictures["namebar"]->getZoom() * 3.1415926 / 2);
	txtdp->loadText(userName, 0.0f, -0.525f, 0.4f * sinScale, 1.0f, 1.0f, 1.0f, 1.0f);
}
void State::onlineHosterSandboxState()
{
	if (!mStateChanging)
	{
		for (auto& picture : mPictures)
		{
			if (picture.second->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				adap->addAudio("assets/sounds/click.wav");
			if (picture.second->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				if (mouseOnButton != &picture)
				{
					mouseOnButton = &picture;
					adap->addAudio("assets/sounds/mouseOn.wav");
				}
				fMouseOnButton = true;
			}
		}
		if (!fMouseOnButton)
			mouseOnButton = nullptr;

		if (mHexaButtons.size() == 0)
		{
			mHexaButtons.push_back(HexaButton(0.0f, -0.3f, 0.10));

			mHexaButtons[0].setColor(Color(255, 255, 255));
			mHexaButtons[0].addText(L"创建并开始", 0.0f, -0.025f, 0.20f, 0.0f, 0.0f, 0.0f, 1.0f);

			mPictures["left"]->zoomIn();
			mPictures["right"]->zoomIn();

			if (!mSandBoxMode[0])
				mPictures["rotation"]->setAlpha(0.2f);
			else
				mPictures["rotation"]->setAlpha(1.0f);
			mPictures["rotation"]->zoomIn();
			if (!mSandBoxMode[1])
				mPictures["colorChange"]->setAlpha(0.2f);
			else
				mPictures["colorChange"]->setAlpha(1.0f);
			mPictures["colorChange"]->zoomIn();
			if (!mSandBoxMode[2])
				mPictures["colorful"]->setAlpha(0.2f);
			else
				mPictures["colorful"]->setAlpha(1.0f);
			mPictures["colorful"]->zoomIn();
		}

		{
			if (mPictures["rotation"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"地图会顺时针旋转", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[0])
						mPictures["rotation"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["rotation"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[0] = !mSandBoxMode[0];
				}
			}
			if (mPictures["colorChange"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"颜色会随时间变换", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[1])
						mPictures["colorChange"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["colorChange"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[1] = !mSandBoxMode[1];
				}
			}
			if (mPictures["colorful"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()))
			{
				txtdp->loadText(L"六边形具有三种颜色", 0.0f, 0.7f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"变化为红 黄 蓝 红", 0.0f, 0.6f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				txtdp->loadText(L"且六边形可通过两次", 0.0f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					if (!mSandBoxMode[2])
						mPictures["colorful"]->changeAlpha(1.0f, 0.5f);
					else
						mPictures["colorful"]->changeAlpha(0.2f, 0.5f);
					mSandBoxMode[2] = !mSandBoxMode[2];
				}
			}
			if (mPictures["left"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				if (mLevelBase > 2)
					mLevelBase--;
			if (mPictures["right"]->inPicture(sta->getCursorXPos(), sta->getCursorYPos()) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				if (mLevelBase < 12)
					mLevelBase++;
			if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				mStateChanging = true;
				mNum = 7;
			}
		}

		if (sta->getKey() == GLFW_KEY_ESCAPE && sta->getKeyAction() == GLFW_PRESS)
		{
			mStateChanging = true;
			mNum = 6;
			delete mServer;   
			mServer = nullptr;
		}

		if (mStateChanging)
		{
			mHexaButtons[0].deleteModeOn();
			mPictures["left"]->zoomOut();
			mPictures["right"]->zoomOut();
			mPictures["rotation"]->zoomOut();
			mPictures["colorChange"]->zoomOut();
			mPictures["colorful"]->zoomOut();
		}
	}
	else
	{
		if (mHexaButtons.size() == 0)
		{
			mStateChanging = false;
			mState = mNum;
			if (mNum == 4)
				mPictures["ghost"]->zoomIn();
		}
	}

	float sinScale = sin(mPictures["left"]->getZoom() * 3.1415926 / 2);
	txtdp->loadText(std::to_wstring(mLevelBase), 0, -0.25, 0.8 * sinScale, 1.0, 1.0, 1.0, 1.0);
	txtdp->loadText(L"点击设置地图大小与加强,ESC返回主菜单", 0, 0.4, 0.4 * sinScale, 1.0, 1.0, 1.0, 1.0);
}

int State::getLevelBase()
{
	return mLevelBase;
}

State* State::getInstance()
{
	if (instance == nullptr)
	{
		instance = new State();
		instance->init();
	}
	return instance;
}

void State::init()
{
	aplct->setCursorPosCallback(State::onCursorPos);
	aplct->setMouseButtonCallback(State::onMouseButton);
	aplct->setKeyCallback(State::onKey);

	std::string filePath = "data.txt";
	if (!std::filesystem::exists(filePath))
	{
		std::ofstream file(filePath);
		if (file.is_open())
			file.close();
		else
			std::cerr << "无法创建文件 " << filePath << std::endl;
	}
	else
	{
		std::ifstream file(filePath);
		if (file.is_open())
			for (int i = 0; i < 3; i++)
			{
				file >> mHRLevelBase[i];
				file >> mHRLevel[i];
			}
	}

	mPictures["title"] = new Picture("assets/textures/title.png", 0.0, 0.4, 0.8);
	mPictures["clock"] = new Picture("assets/textures/clock.png", -0.95, 0.77, 0.085);
	mPictures["reset"] = new Picture("assets/textures/reset.png", -0.95, 0.60, 0.1);
	mPictures["ghost"] = new Picture("assets/textures/ghost.png", 0.0, 0.0, 1);
	mPictures["rotationS"] = new Picture("assets/textures/rotation.png", 0.75, 0.9, 0.1);
	mPictures["colorChangeS"] = new Picture("assets/textures/colorChange.png", 0.85, 0.9, 0.1);
	mPictures["colorfulS"] = new Picture("assets/textures/colorful.png", 0.95, 0.9, 0.1);
	mPictures["rotation"] = new Picture("assets/textures/rotation.png", -0.3, 0.2, 0.2);
	mPictures["colorChange"] = new Picture("assets/textures/colorChange.png", 0.0, 0.2, 0.2);
	mPictures["colorful"] = new Picture("assets/textures/colorful.png", 0.3, 0.2, 0.2);
	mPictures["left"] = new Picture("assets/textures/left.png", -0.3, -0.2, 0.5);
	mPictures["right"] = new Picture("assets/textures/right.png", 0.3, -0.2, 0.5);
	mPictures["namebar"] = new Picture("assets/textures/bar.png", 0.0, -0.5, 1.0);
}

void State::onCursorPos(double xpos, double ypos)
{
	getInstance()->mCursorXPos = xpos;
	getInstance()->mCursorYPos = aplct->getLength() - ypos;
}
void State::onMouseButton(int button, int action, int mods)
{
	getInstance()->mMouseButton = button;
	getInstance()->mMouseAction = action;
}
void State::onKey(int key, int scancode, int action, int mods)
{
	getInstance()->mKey = key;
	getInstance()->mKeyAction = action;
}

std::vector<Hexa>& State::getHexas()
{
	return mHexas;
}
std::vector<Hexa*>& State::getAnsSteps()
{
	return mAnsSteps;
}
std::vector<Hexa*>& State::getPlayerSteps()
{
	return mPlayerSteps;
}
HexaColorBackup& State::getHexaColorBackup()
{
	return mHexaColorBackup;
}

std::vector<HexaButton>& State::getHexaButtons()
{
	return mHexaButtons;
}

void State::clickHexaSound()
{
	int soundID = rand() % 6;
	switch (soundID)
	{
	case 0:
	default:
	{
		adap->addAudio("assets/sounds/sound0.wav");
	}
	break;
	case 1:
	{
		adap->addAudio("assets/sounds/sound1.wav");
	}
	break;
	case 2:
	{
		adap->addAudio("assets/sounds/sound2.wav");
	}
	break;
	case 3:
	{
		adap->addAudio("assets/sounds/sound3.wav");
	}
	break;
	case 4:
	{
		adap->addAudio("assets/sounds/sound4.wav");
	}
	break;
	case 5:
	{
		adap->addAudio("assets/sounds/sound5.wav");
	}
	break;
	}
}
void State::finishSound()
{
	int soundID = rand() % 4;
	switch (soundID)
	{
	case 0:
		adap->addAudio("assets/sounds/finish0.wav");
		break;
	case 1:
		adap->addAudio("assets/sounds/finish1.wav");
		break;
	case 2:
		adap->addAudio("assets/sounds/finish2.wav");
		break;
	case 3:
		adap->addAudio("assets/sounds/finish3.wav");
		break;
	}
}

float State::getHexaRadius() const
{
	return mHexaRadius;
}
void State::setHexaRadius(float hexaRadius)
{
	mHexaRadius = hexaRadius;
}

void State::randSeed()
{
	mSeed = std::random_device{}();
	mSeed %= 524288;
	mGen = std::mt19937(mSeed);
}
uint32_t State::getSeed()
{
	return mSeed;
}
void State::setSeed(uint32_t seed)
{
	mSeed = seed;
	mGen = std::mt19937(mSeed);
}
int State::genInt(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(mGen);
}

short State::getColorMode() const
{
	return mColorMode;
}
void State::setColorMode(short colorMode)
{
	mColorMode = colorMode;
}

void State::colorChange()
{
	if (!mColorChangeMode)
		return;
	if (shoudeColorChange())
		for (auto& hexa : mHexas)
			hexa.softChangeColor();
}
void State::setColorChangeMode(bool colorChangeMode)
{
	mColorChangeMode = colorChangeMode;
	mColorChangeModeTime = glfwGetTime();
	mColorChange = 0;
}
bool State::getColorChangeMode() const
{
	return mColorChangeMode;
}
bool State::shoudeColorChange() const
{
	if (glfwGetTime() - mColorChangeModeTime > 3)
	{
		mColorChangeModeTime += 3;
		mColorChange++;
		mColorChange %= mColorMode;
		return true;
	}
	return false;
}
int State::getColorChange() const
{
	return mColorChange;
}


void State::setRotationMode(bool bo)
{
	mRotationMode = bo;
}
bool State::getRotationMode() const
{
	return mRotationMode;
}

void State::setSAST()
{
	mShowAnswerStartTime = glfwGetTime() + 2;
	mShowStepIndex = 0;
}
float State::getSAST()const
{
	return mShowAnswerStartTime;
}
void State::setShowAnswer(bool bo)
{
	mShowAnswer = bo;
}
bool State::getShowAnswer() const
{
	return mShowAnswer;
}
void State::add1MShowStepIndex()
{
	mShowStepIndex++;
}
int State::getMShowStepIndex()
{
	return mShowStepIndex;
}

void State::hexasScaleAdd()
{
	for (auto& hexaButton : mHexaButtons)
		hexaButton.mScaleAdd();
	for (auto& hexa : mHexas)
		hexa.mScaleAdd();
}

void State::resetMM()
{
	mReset--;
}

std::map< std::string, Picture*>& State::getPictures()
{
	return mPictures;
}

int State::getCursorXPos() const
{
	return mCursorXPos;
}
int State::getCursorYPos() const
{
	return mCursorYPos;
}
int State::getMouseAction() const
{
	return mMouseAction;
}
int State::getMouseButton() const
{
	return mMouseButton;
}
void State::clearMouse()
{
	mMouseAction = NULL;
	mMouseButton = NULL;
}
int State::getKey() const
{
	return mKey;
}
int State::getKeyAction() const
{
	return mKeyAction;
}
void State::clearKey()
{
	mKey = NULL;
	mKeyAction = NULL;
}