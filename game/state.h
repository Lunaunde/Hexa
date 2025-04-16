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

	void setColorChangeMode(bool colorChangeMode);
	bool getColorChangeMode() const;
	void add1CCFC();
	void colorChange(int needFrame);
	Color getChangedColor(Color color);

	uint32_t getSeed();
	void setSeed(uint32_t seed);
	int genInt(int min, int max);

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

	int mCursorXPos, mCursorYPos, mMouseButton, mMouseAction, mKey, mKeyAction;
	float mHexaRadius;

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mAnsSteps;
	std::vector<Hexa*> mPlayerSteps;
	HexaColorBackup mHexaColorBackup;

	short mColorMode = 2;

	bool mColorChangeMode = false;
	int mColorChangeFrameCount = 0;
	short mColorChange = 0;

	bool mRotationMode = false;

	uint32_t mSeed;
	std::mt19937 mGen;

	static State* instance;
};

#endif