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

void Logic::buildLevel(int size)
{
	if (sta->getHexas().size() > 0)
		sta->getHexas().clear();
	initHexa(size, sta->getHexas());
	setHexaNear(sta->getHexas());
	do
		setHexaColor(size, sta->getHexas());
	while (badHexaColor(sta->getHexas()) && size > 1);
	HexaColorBackup defaultBackup(sta->getHexas());
	int stepCount;
	do
	{
		defaultBackup.restore(sta->getHexas());
		sta->getAnsSteps().clear();
		stepCount = randomHexaColor(sta->getHexas(), sta->getAnsSteps());
	} while (size > 2 && badLevel(sta->getHexas(), stepCount));
	sta->getHexaColorBackup() = HexaColorBackup(sta->getHexas());
}

void Logic::initHexa(int size, std::vector<Hexa>& hexas)
{
	hexas.clear();
	hexas.emplace_back(0, 0, 0);
	for (int i = 0; i<hexas.size(); i++)//六边形地图位置初始化
	{
		//if (i >= hexas.size())
		//	break;
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
void Logic::setHexaColor(int size, std::vector<Hexa>& hexas)
{
	for (int i = 0; i < size; i++)
	{
		int colorID = 0;
		switch (sta->getColorMode())
		{
		case 2:
			colorID = sta->genInt(0, 1);
			break;
		case 3:
			colorID = sta->genInt(2, 4);
			break;
		}
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
				case 2:
					hexas[j].setColor(Color::pureRed);
					break;
				case 3:
					hexas[j].setColor(Color::pureYellow);
					break;
				case 4:
					hexas[j].setColor(Color::pureBlue);
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
int Logic::randomHexaColor(std::vector<Hexa>& hexas, std::vector<Hexa*>& steps)
{
	std::vector<char> colorBackups;
	Hexa* step = &hexas[sta->genInt(0, hexas.size() - 1)];
	for (int i = 0; true; i++)
	{
		steps.push_back(step);
		switch (sta->getColorMode())
		{
		case 2:
			step->setColor(step->getColor() == Color::pureWhite ? Color::pureBlack : Color::pureWhite);
			break;
		case 3:
			if (step->getColor() == Color::pureRed)
				step->setColor(Color::pureBlue);
			else if (step->getColor() == Color::pureBlue)
				step->setColor(Color::pureYellow);
			else if (step->getColor() == Color::pureYellow)
				step->setColor(Color::pureRed);
			break;
		}
		std::vector<Hexa*> enableNear;
		for (int j = 0; j < 6; j++)
		{
			if (step->getNear(j) != nullptr)
			{
				int count = 0;
				for (int k = 0; k < steps.size(); k++)
					if (steps[k] == step->getNear(j))
						count++;
				switch (sta->getColorMode())
				{
				case 2:
					if (count == 0)
						enableNear.push_back(step->getNear(j));
					break;
				case 3:
					if (count <= 1)
						enableNear.push_back(step->getNear(j));
					break;
				}
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
		step = enableNear[weight[sta->genInt(0, weight.size() - 1)]];
	}
}
bool Logic::badLevel(std::vector<Hexa>& hexas, int stepCount)
{
	switch (sta->getColorMode())
	{
	case 2:
		if (stepCount < hexas.size() / 3)
			return true;
		break;
	case 3:
		if (stepCount < hexas.size() / 3 * 2)
			return true;
		break;
	}
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

void Logic::playerStepCheck()
{
	std::vector<Hexa>& hexas = sta->getHexas();
	float side = sta->getHexaRadius();
	float rotation = 0;
	if (sta->getRotationMode())
		rotation = glfwGetTime();
	if (!sta->getShowAnswer())
		if (sta->getPlayerSteps().size() == 0)
		{
			for (int i = 0; i < hexas.size(); i++)
			{
				if (hexas[i].ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), side, 0.95, rotation) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					sta->getPlayerSteps().push_back(&hexas[i]);
					hexas[i].softChangeColor();
					sta->playStoneSound();
				}
			}
		}
		else
		{
			Hexa* lastStep = sta->getPlayerSteps()[sta->getPlayerSteps().size() - 1];
			for (int i = 0; i < 6; i++)
			{
				Hexa* nowStep = lastStep->getNear(i);
				if (nowStep == nullptr)
					continue;
				if (nowStep->ifPositionInHexa(sta->getCursorXPos(), sta->getCursorYPos(), side, 0.95, rotation) && sta->getMouseButton() == GLFW_MOUSE_BUTTON_1 && sta->getMouseAction() == GLFW_PRESS)
				{
					int count = 0;
					for (int j = 0; j < sta->getPlayerSteps().size(); j++)
					{
						if (sta->getPlayerSteps()[j] == nowStep)
							count++;
					}
					switch (sta->getColorMode())
					{
					case 2:
						if (count > 0)
							return;
						break;
					case 3:
						if (count > 1)
							return;
					}
					sta->getPlayerSteps().push_back(nowStep);
					nowStep->softChangeColor();
					break;
					sta->playStoneSound();
				}
			}
		}
}

void Logic::reloadLevel()
{
	std::vector<Hexa>& hexas = sta->getHexas();
	if (sta->getKey() == GLFW_KEY_R && sta->getKeyAction() == GLFW_PRESS)
	{
		sta->getHexaColorBackup().softRestore(hexas);
		sta->getPlayerSteps().clear();
		sta->setShowAnswer(false);
		for (int i = 0; i < sta->getColorChange(); i++)
			for (auto& hexa : hexas)
				hexa.changeColor();
	}
}
void Logic::showAnswer()
{
	if (sta->getKey() == GLFW_KEY_A && sta->getKeyAction() == GLFW_PRESS)
	{
		sta->setSAST();
		sta->setShowAnswer(true);
		sta->getHexaColorBackup().softRestore(sta->getHexas());
		sta->getPlayerSteps().clear();
		std::cout << sta->getMShowStepIndex() << " " << sta->getAnsSteps().size();
	}
	if (sta->getShowAnswer())
	{
		if (sta->getMShowStepIndex() < (int)sta->getAnsSteps().size())
		{
			std::vector<Hexa*>& ansSteps = sta->getAnsSteps();
			int ms = (glfwGetTime() - sta->getSAST()) * 1000;
			int index = ms / 300;
			while (index > sta->getMShowStepIndex() && sta->getMShowStepIndex() < (int)ansSteps.size())
			{
				ansSteps[sta->getMShowStepIndex()]->softChangeColor();
				sta->add1MShowStepIndex();
				sta->playStoneSound();
			}
		}
	}
}