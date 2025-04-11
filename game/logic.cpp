#include "logic.h"
#include<random>
#include<cmath>
#include<iostream>
#include"../glframework/core.h"
#include"../application/Application.h"
#include"../application/AudioPlayer.h"

Logic::Logic()
{
}
Logic::~Logic()
{
}

void Logic::buildLevel(int size, State* state)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	if (state->getHexas().size() > 0)
		state->getHexas().clear();
	initHexa(size, state->getHexas());
	setHexaNear(state->getHexas());
	do
		setHexaColor(size, state->getHexas(), gen);
	while (badHexaColor(state->getHexas()) && size > 1);
	HexaColorBackup defaultBackup(state->getHexas());
	int stepCount;
	do
	{
		defaultBackup.restore(state->getHexas());
		state->getAnsSteps().clear();
		stepCount = randomHexaColor(state->getHexas(), state->getAnsSteps(), gen);
	} while (size > 2 && badLevel(state->getHexas(), stepCount));
	state->getHexaColorBackup() = HexaColorBackup(state->getHexas());
}

void Logic::initHexa(int size, std::vector<Hexa>& hexas)
{
	hexas.emplace_back(0, 0, 0);
	for (int i = 0; true; i++)//六边形地图位置初始化
	{
		if (i >= hexas.size())
			break;
		Hexa temp[6] = {
			Hexa(hexas[i].getXPos() + 1,hexas[i].getYPos(),hexas[i].getZPos() - 1),
			Hexa(hexas[i].getXPos() - 1,hexas[i].getYPos(),hexas[i].getZPos() + 1),
			Hexa(hexas[i].getXPos() + 1,hexas[i].getYPos() - 1,hexas[i].getZPos()),
			Hexa(hexas[i].getXPos() - 1,hexas[i].getYPos() + 1,hexas[i].getZPos()),
			Hexa(hexas[i].getXPos(),hexas[i].getYPos() + 1,hexas[i].getZPos() - 1),
			Hexa(hexas[i].getXPos(),hexas[i].getYPos() - 1,hexas[i].getZPos() + 1)
		};
		for (int j = 0; j < 6; j++)
		{
			if (temp[j].distanceToCenter() < size)
			{
				bool flag = true;
				for (Hexa& hexa : hexas)
				{
					if (hexa.getXPos() == temp[j].getXPos() && hexa.getYPos() == temp[j].getYPos() && hexa.getZPos() == temp[j].getZPos())
					{
						flag = false;
						break;
					}
				}
				if (flag)
				{
					hexas.push_back(temp[j]);
				}
			}
		}
	}
}
void Logic::setHexaNear(std::vector<Hexa>& hexas)
{
	for (int i = 0; true; i++)//六边形地图指针指向
	{
		if (i >= hexas.size())
			break;
		for (int j = 0; true; j++)
		{
			if (j >= hexas.size())
				break;

			if (hexas[i].getXPos() == hexas[j].getXPos() && hexas[i].getYPos() == hexas[j].getYPos() && hexas[i].getZPos() == hexas[j].getZPos())
				continue;
			int dx = hexas[i].getXPos() - hexas[j].getXPos();
			int dy = hexas[i].getYPos() - hexas[j].getYPos();
			int dz = hexas[i].getZPos() - hexas[j].getZPos();
			if (std::max(std::max(abs(dx), abs(dy)), abs(dz)) == 1)
			{
				for (int k = 0; k < 6; k++)
				{
					if (hexas[i].getNear(k) == nullptr)
					{
						hexas[i].setNear(k, &hexas[j]);
						break;
					}
				}
			}
		}
	}
}
void Logic::setHexaColor(int size, std::vector<Hexa>& hexas, std::mt19937& gen)
{
	std::uniform_real_distribution<> dis(0, 2147483647);
	for (int i = 0; i < size; i++)
	{
		int colorID = (int)(dis(gen)) % 2;
		for (int j = 0; j < hexas.size(); j++)
			if (hexas[j].distanceToCenter() == i)
			{
				switch (colorID)
				{
				case 0:
					hexas[j].setColor(Color::pureWhite);
					break;
				case 1:
					hexas[j].setColor(Color::pureBlack);
					break;
				}
			}
	}
}
bool Logic::badHexaColor(std::vector<Hexa>& hexas)
{
	for (int i = 0; i < hexas.size(); i++)
	{
		if (hexas[i].getColor() != hexas[0].getColor())
			return false;
	}
	return true;
}
int Logic::randomHexaColor(std::vector<Hexa>& hexas, std::vector<Hexa*>& steps, std::mt19937& gen)
{
	std::uniform_real_distribution<> dis(0, 2147483647);
	std::vector<char> colorBackups;
	Hexa* step = &hexas[(int)(dis(gen)) % hexas.size()];
	for (int i = 0; true; i++)
	{
		steps.push_back(step);
		step->setColor(step->getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
		std::vector<Hexa*> enableNear;
		for (int j = 0; j < 6; j++)
		{
			if (step->getNear(j) != nullptr)
			{
				int flag = true;
				for (int k = 0; k < steps.size(); k++)
					if (steps[k] == step->getNear(j))
						flag = false;
				if (flag)
					enableNear.push_back(step->getNear(j));
			}
		}
		std::vector<int> weight;
		for (int k = 0; k < enableNear.size(); k++)
		{
			for (int j = 0; j < 6; j++)
			{
				if (enableNear[k]->getNear(j) != nullptr)
				{
					int flag = true;
					for (int l = 0; l < steps.size(); l++)
						if (steps[l] == enableNear[k]->getNear(j))
							flag = false;
					if (flag)
						weight.push_back(k);
				}
			}
		}
		if (weight.size() == 0)
			return steps.size();
		step = enableNear[weight[(int)dis(gen) % weight.size()]];
	}
}
bool Logic::badLevel(std::vector<Hexa>& hexas, int stepCount)
{
	if (stepCount < hexas.size() / 3)
		return true;
	HexaColorBackup backup(hexas);
	for (int i = 0; i < hexas.size(); i++)
	{
		if (hexas[i].getColor() == Color::pureWhite)
		{
			nearSameColorChange(hexas[i]);
			bool flag = true;
			for (int j = 0; j < hexas.size(); j++)
			{
				if (hexas[j].distanceToCenter() == 0)
					continue;
				if (hexas[j].getColor() == Color::pureWhite)
					flag = false;
			}
			if (flag)
				return true;
			break;
		}
	}
	backup.restore(hexas);
	for (int i = 0; i < hexas.size(); i++)
	{
		if (hexas[i].getColor() == Color::pureBlack)
		{
			nearSameColorChange(hexas[i]);
			bool flag = true;
			for (int j = 0; j < hexas.size(); j++)
			{
				if (hexas[j].distanceToCenter() == 0)
					continue;
				if (hexas[j].getColor() == Color::pureBlack)
					flag = false;
			}
			if (flag)
				return true;
			break;
		}
	}
	backup.restore(hexas);
	return false;
}
void Logic::nearSameColorChange(Hexa& hexa)
{
	switch (sta->getColorMode())
	{
	case 2:
		hexa.setColor(hexa.getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
		break;
	}
	for (int i = 0; i < 6; i++)
	{
		if (hexa.getNear(i) == nullptr)
			break;
		if (hexa.getNear(i)->getColor() != hexa.getColor())
		{
			nearSameColorChange(*(hexa.getNear(i)));
		}
	}
	return;
}

bool Logic::finishPuzzle(std::vector<Hexa>& hexas, int size)
{
	for (int i = 0; i < size; i++)
	{
		Color color(-1, -1, -1);
		for (int j = 0; j < hexas.size(); j++)
		{
			if (hexas[j].distanceToCenter() == i)
			{
				if (color.getRed() == -1 && color.getGreen() == -1 && color.getBlue() == -1)
					color = hexas[j].getColor();
				else if (color != hexas[j].getColor())
					return false;
			}
		}
	}
	return true;
}

void Logic::playerStepCheck(std::vector<Hexa>& hexas, float side)
{
	if (sta->getKey() == GLFW_KEY_R && sta->getKeyAction() == GLFW_PRESS)
	{
		sta->getHexaColorBackup().restore(hexas);
		sta->getPlayerSteps().clear();
	}
	if (sta->getPlayerSteps().size() == 0)
	{
		for (int i = 0; i < hexas.size(); i++)
		{
			if (hexas[i].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), side) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				sta->getPlayerSteps().push_back(&hexas[i]);
				hexas[i].setColor(hexas[i].getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
				//sta->playStoneSound();
			}
		}
	}
	else
	{
		Hexa* lastStep = sta->getPlayerSteps()[sta->getPlayerSteps().size() - 1];
		for (int i = 0; i < 6; i++)
		{
			if (lastStep->getNear(i) == nullptr)
				continue;
			if (lastStep->getNear(i)->ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), side) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
			{
				bool flag = true;
				for (int j = 0;j < sta->getPlayerSteps().size();j++)
				{
					if (sta->getPlayerSteps()[j] == lastStep->getNear(i))
						flag = false;
				}
				if (!flag)
					return;
				sta->getPlayerSteps().push_back(lastStep->getNear(i));
				lastStep->getNear(i)->setColor(lastStep->getNear(i)->getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
				//sta->playStoneSound();
			}
		}
	}
}
void Logic::clickChangeHexa(std::vector<Hexa>& hexas, float side)
{
	for (int i = 0; i < hexas.size(); i++)
	{
		if (hexas[i].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), 0.05f) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
		{
			hexas[i].setColor(hexas[i].getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
		}
	}
}