#include"state.h"
#include<vector>
#include<cmath>
#include"../application/Application.h"
#include"../glframework/core.h"

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

State::State() : mHexas(), mAnsSteps(), mPlayerSteps(), mHexaColorBackup()
{
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

void State::playStoneSound()
{
	int soundID = rand() % 4;
	switch (soundID)
	{
	case 0:
	default:
	{
		AudioPlayer Sound("assets/sounds/deepStone/3.wav");
		Sound.play(1);
	}
		break;
	}
}

float State::getHexaRadius()
{
	return mHexaRadius;
}
void State::setHexaRadius(float hexaRadius)
{
	mHexaRadius = hexaRadius;
}

int State::getCursorXPos()
{
	return mCursorXPos;
}
int State::getCursorYPos()
{
	return mCursorYPos;
}
int State::getMouseAction()
{
	return mMouseAction;
}
int State::getMouseButton()
{
	return mMouseButton;
}
void State::clearMouse()
{
	mMouseAction = NULL;
	mMouseButton = NULL;
}
int State::getKey()
{
	return mKey;
}
int State::getKeyAction()
{
	return mKeyAction;
}
void State::clearKey()
{
	mKey = NULL;
	mKeyAction = NULL;
}