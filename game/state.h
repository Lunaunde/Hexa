﻿#ifndef STATE_H
#define STATE_H
#define sta State::getInstance()
#include<vector>
#include "hexa.h"

class HexaColorBackup
{
public:
	HexaColorBackup(std::vector<Hexa>& hexas);
	HexaColorBackup();
	~HexaColorBackup();
	void restore(std::vector<Hexa>& hexas);
private:
	std::vector<char> mHexaColorBackup;
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

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mAnsSteps;
	std::vector<Hexa*> mPlayerSteps;
	HexaColorBackup mHexaColorBackup;
	static State* instance;
};

#endif