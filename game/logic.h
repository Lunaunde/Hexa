#ifndef LOGIC_H
#define LOGIC_H
#include<vector>
#include<random>
#include<functional>
#include"state.h"

class Logic
{
public:
	Logic();
	~Logic();

	static void buildLevel(int size);
	static void initHexa(int size, std::vector<Hexa>& hexas);
	static void setHexaNear(std::vector<Hexa>& hexas);
	static void setHexaColor(int size, std::vector<Hexa>& hexas);
	static bool badHexaColor(std::vector<Hexa>& hexas);
	static int randomHexaColor(std::vector<Hexa>& hexas, std::vector<Hexa*>& steps);
	static bool badLevel(std::vector<Hexa>& hexas, int stepCount);
	static void nearSameColorChange(Hexa& hexa);

	static bool finishPuzzle(std::vector<Hexa>& hexas,int size);

	static void playerStepCheck(std::vector<Hexa>& hexas, float side);
	static void clickChangeHexa(std::vector<Hexa>& hexas, float side);
};

#endif