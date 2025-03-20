#ifndef STATE_H
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
	std::vector<Hexa*>& getSteps();
	HexaColorBackup& getHexaColorBackup();


	int getCursorXPos();
	int getCursorYPos();
	int getMouseButton();
	int getMouseAction();
	double getMouseTime();
private:
	State();

	void static onCursorPos(double xpos, double ypos);
	void static onMouseButton(int button, int action, int mods);
	int mCursorXPos, mCursorYPos, mMouseButton, mMouseAction;
	double mMouseTime;

	std::vector<Hexa> mHexas;
	std::vector<Hexa*> mSteps;
	HexaColorBackup mHexaColorBackup;
	static State* instance;
};

#endif