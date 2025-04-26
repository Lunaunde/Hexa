#ifndef STATE_H
#define STATE_H
#define sta State::getInstance()
#include<array>
#include<vector>
#include<random>
#include<functional>
#include "hexa.h"
#include"color.h"
#include"../application/AudioPlayer.h"

class HexaColorBackup
{
public:
	HexaColorBackup(std::vector<Hexa>& hexas);
	HexaColorBackup();
	~HexaColorBackup();
	void restore(std::vector<Hexa>& hexas);
	void softRestore(std::vector<Hexa>& hexas);
private:
	std::vector<Color> mHexaColorBackup;
};

class State
{
public:
	~State();

	static State* getInstance();
	void init();

	std::vector<Hexa>& getHexas();
	std::vector<Hexa*>& getAnsSteps();
	std::vector<Hexa*>& getPlayerSteps();
	HexaColorBackup& getHexaColorBackup();

	void playStoneSound();

	float getHexaRadius() const;
	void setHexaRadius(float hexaRadius);

	short getColorMode() const;
	void setColorMode(short colorMode);

	void colorChange();
	short getColorChange() const;
	void setColorChangeMode(bool colorChangeMode);
	bool getColorChangeMode() const;
	void add1CCFC(int needFrame);
	bool shoudeColorChange() const;

	void setRotationMode(bool bo);
    bool getRotationMode() const;

	uint32_t getSeed();
	void setSeed(uint32_t seed);
	int genInt(int min, int max);

	void setSAST();
	float getSAST() const;
	void setShowAnswer(bool bo);
	bool getShowAnswer() const;
	void add1SAF();
	void clearSAF();
	int getSAF() const;

	void hexasScaleAdd();

	int getCursorXPos() const;
	int getCursorYPos() const;
	int getMouseButton() const;
	int getMouseAction() const;
	void clearMouse();
	int getKey() const;
	int getKeyAction() const;
	void clearKey();
private:
	State();

	void static onCursorPos(double xpos, double ypos);
	void static onMouseButton(int button, int action, int mods);
	void static onKey(int key, int scancode, int action, int mods);

	int mState = 0;
	int mLevelBase = 3;
	int mLevel = 1;

	int mCursorXPos, mCursorYPos, mMouseButton, mMouseAction, mKey, mKeyAction;
	float mHexaRadius;

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mAnsSteps;
	std::vector<Hexa*> mPlayerSteps;
	HexaColorBackup mHexaColorBackup;

	short mColorMode = 2;

	bool mColorChangeMode = false;
	int mColorChangeFrameCount = 0;
	mutable short mColorChange = 0;

	int mShowAnswerFrameCount = 1;
	bool mShowAnswer = false;
	float mShowAnswerStartTime = 0;

	bool mRotationMode = false;

	uint32_t mSeed;
	std::mt19937 mGen;

	static State* instance;
};

#endif