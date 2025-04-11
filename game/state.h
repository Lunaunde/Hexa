#ifndef STATE_H
#define STATE_H
#define sta State::getInstance()
#include<array>
#include<vector>
#include "hexa.h"
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

	int getCursorXPos();
	int getCursorYPos();
	int getMouseButton();
	int getMouseAction();
	void clearMouse();
	int getKey();
	int getKeyAction();
	void clearKey();
private:
	State();

	void static onCursorPos(double xpos, double ypos);
	void static onMouseButton(int button, int action, int mods);
	void static onKey(int key, int scancode, int action, int mods);

	int mCursorXPos, mCursorYPos, mMouseButton, mMouseAction,mKey,mKeyAction;
	float mHexaRadius;

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mAnsSteps;
	std::vector<Hexa*> mPlayerSteps;
	HexaColorBackup mHexaColorBackup;

	short mColorMode=2;

	static State* instance;
};

#endif