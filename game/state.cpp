#include"state.h"
#include<vector>
#include<cmath>
#include<iostream>
#include"../application/Application.h"
#include"../glframework/core.h"
#include"logic.h"
#include"render.h"

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
	if (mHexaButtons.size() > 0 && mHexaButtons[0].isDeleted())
		mHexaButtons.clear();
	if (mHexas.size() > 0 && mHexas[0].isDeleted())
		mHexas.clear();
	switch (mState)
	{
	case 0:
	{
		if (mHexaButtons.size() == 0)
		{
			mHexaButtons.push_back(HexaButton(-0.4f, 0.0f, 0.15));
			mHexaButtons.push_back(HexaButton(0.0f, 0.0f, 0.15));
			mHexaButtons.push_back(HexaButton(0.4f, 0.0f, 0.15));
			mHexaButtons[0].setColor(Color(124, 252, 0));
			mHexaButtons[1].setColor(Color(135, 206, 235));
			mHexaButtons[2].setColor(Color(255, 127, 0));
		}
		else
		{
			bool startGame = false;
			if (mHexaButtons[0].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 0;
				}
			}
			if (mHexaButtons[1].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 1;
				}
			}
			if (mHexaButtons[2].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 1, 0) == true)
			{
				if (sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					startGame = true;
					mDifficulty = 2;
				}
			}
			if (startGame)
			{
				mLevel = 1;
				mLevelBase = 3;
				mState = 1;
				randSeed();
				for (auto& hexa : mHexaButtons)
				{
					hexa.deleteModeOn();
				}
			}
		}
	}
	break;
	case 1:
	{
		if (mHexaButtons.size() == 0 && mHexas.size() == 0)
		{
			setRotationMode(false);
			setColorChangeMode(false);
			setColorMode(2);
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
					case 3:
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
					case 3:
						setColorMode(2);
						break;
					}
				}
				}
			}
			Logic::buildLevel(mLevelBase);
		}
		else if (mHexas.size() > 0)
		{
			Logic::playerStepCheck();
			Logic::reloadLevel();
			Logic::showAnswer();

			colorChange();

			Logic::finishPuzzle(getHexas(), mLevelBase);
			clearMouse();
			clearKey();
		}
	}
	}
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

	mSeed = std::random_device{}();
	mSeed %= 524288;
	mGen = std::mt19937(mSeed);
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

void State::playStoneSound()
{
	int soundID = rand() % 4;
	switch (soundID)
	{
	case 0:
	default:
	{
		new AutoDeleteAudioPlayer("assets/sounds/deepStone/0.wav");
	}
	break;
	case 1:
	{
		new AutoDeleteAudioPlayer("assets/sounds/deepStone/1.wav");
	}
	break;
	case 2:
	{
		new AutoDeleteAudioPlayer("assets/sounds/deepStone/2.wav");
	}
	break;
	case 3:
	{
		new AutoDeleteAudioPlayer("assets/sounds/deepStone/3.wav");
	}
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