#include"state.h"
#include<vector>
#include<cmath>

State* State::instance = nullptr;

State::State():mHexas(),mSteps(),mHexaColorBackup()
{
}

State::~State()
{
}

State* State::getInstance()
{
	if(instance == nullptr)
		instance = new State();
	return instance;
}

std::vector<Hexa>& State::getHexas()
{
	return mHexas;
}

std::vector<Hexa*>& State::getSteps()
{
	return mSteps;
}

HexaColorBackup& State::getHexaColorBackup()
{
	return mHexaColorBackup;
}

HexaColorBackup::HexaColorBackup(std::vector<Hexa> & hexas)
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
	for(int i = 0;i < mHexaColorBackup.size();i++)
	{
		hexas[i].setColor(mHexaColorBackup[i]);
	}
}