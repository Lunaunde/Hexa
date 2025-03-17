#include"state.h"
#include<vector>
#include<cmath>

State* State::instance = nullptr;

State::State():mHexas()
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

HexaColorBackup::HexaColorBackup(std::vector<Hexa> & hexas)
{
	for (int i = 0; i < hexas.size(); i++)
	{
		mHexaColorBackup.push_back(hexas[i].getColor());
	}
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