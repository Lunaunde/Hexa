#ifndef STATE_H
#define STATE_H
#define sta State::getInstance()
#include<array>
#include<vector>
#include<random>
#include<functional>
#include "hexa.h"
#include"color.h"
#include"../glframework/texture.h"
#include"../application/AudioPlayer.h"

class Picture
{
public:
private:
};

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

	void allState();

	static State* getInstance();
	void init();

	std::vector<Hexa>& getHexas();
	std::vector<Hexa*>& getAnsSteps();
	std::vector<Hexa*>& getPlayerSteps();
	HexaColorBackup& getHexaColorBackup();

	std::vector<HexaButton>& getHexaButtons();

	void playStoneSound();

	float getHexaRadius() const;
	void setHexaRadius(float hexaRadius);

	short getColorMode() const;
	void setColorMode(short colorMode);

	void colorChange();
	void setColorChangeMode(bool colorChangeMode);
	bool getColorChangeMode() const;
	bool shoudeColorChange() const;
	int getColorChange() const;

	void setRotationMode(bool bo);
    bool getRotationMode() const;

	void randSeed();
	uint32_t getSeed();
	void setSeed(uint32_t seed);
	int genInt(int min, int max);

	void setSAST();
	float getSAST() const;
	void setShowAnswer(bool bo);
	bool getShowAnswer() const;
	void add1MShowStepIndex();
	int getMShowStepIndex();

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

	std::array<int,3> mHRLevelBase = { 3,3,3 };
	std::array<int,3> mHRLevel = { 0,0,0 };
	bool newRecord;

	int mLevelBase = 0;
	int mLevel = 0;
	int mDifficulty = 0;
	int mState = 0;// 0 menu,1 game,2 lose
	float mClock = 0;

	int mCursorXPos, mCursorYPos, mMouseButton, mMouseAction, mKey, mKeyAction;
	float mHexaRadius;

	std::vector<HexaButton> mHexaButtons;

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mAnsSteps;
	std::vector<Hexa*> mPlayerSteps;
	HexaColorBackup mHexaColorBackup;

	short mColorMode = 2;

	bool mColorChangeMode = false;
	mutable short mColorChange = 0;
	mutable float mColorChangeModeTime = -1;

	bool mShowAnswer = false;
	float mShowAnswerStartTime = 0;
	int mShowStepIndex = -1;

	bool mRotationMode = false;

	uint32_t mSeed;
	std::mt19937 mGen;

	static State* instance;
};

#endif