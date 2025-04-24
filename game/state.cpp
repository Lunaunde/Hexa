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
void HexaColorBackup::softRestore(std::vector<Hexa>& hexas)
{
	for (int i = 0; i < mHexaColorBackup.size(); i++)
	{
		hexas[i].changeColor(mHexaColorBackup[i]);
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

void State::setColorChangeMode(bool colorChangeMode)
{
	mColorChangeMode = colorChangeMode;
}
bool State::getColorChangeMode() const
{
	return mColorChangeMode;
}
void State::add1CCFC()
{
	mColorChangeFrameCount++;
}
void State::colorChange(int needFrame)
{
	mColorChange = mColorChangeFrameCount / needFrame;
}
Color State::getChangedColor(Color color)
{
	if (mColorMode == 2)
	{
		if (mColorChange % 2 == 1)
			color = (color == Color::pureWhite) ? Color::pureBlack : Color::pureWhite;
	}
	else if (mColorMode == 3)
	{
		if (mColorChange % 3 == 1)
		{
			if (color == Color::pureRed)
				color = Color::pureYellow;
			else if (color == Color::pureYellow)
				color = Color::pureBlue;
			else if (color == Color::pureBlue)
				color = Color::pureRed;
		}
		else if (mColorChange % 3 == 2)
		{
			if (color == Color::pureRed)
				color = Color::pureBlue;
			else if (color == Color::pureBlue)
				color = Color::pureYellow;
			else if (color == Color::pureYellow)
				color = Color::pureRed;
		}
	}
	return color;
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
	mShowAnswerStartTime = glfwGetTime();
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
void State::add1SAF()
{
	mShowAnswerFrameCount++;
}
void State::clearSAF()
{
	mShowAnswerFrameCount = 0;
}
int State::getSAF() const
{
	return mShowAnswerFrameCount;
}

void State::hexasScaleAdd()
{
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