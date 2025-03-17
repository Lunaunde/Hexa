#ifndef LOGIC_H
#define LOGIC_H
#include<vector>
#include<random>
#include"state.h"

class Logic
{
public:
	Logic();
	~Logic();

	static void buildLevel(int size, std::vector<Hexa> & hexa);
	static void initHexa(int size, std::vector<Hexa> & hexa);
	static void setHexaNear(std::vector<Hexa> & hexa);
	static void setHexaColor(int size,std::vector<Hexa> & hexa,std::mt19937& gen);
	static bool badHexaColor(std::vector<Hexa> & hexa);
	static int randomHexaColor(std::vector<Hexa>& hexa,std::mt19937& gen);
	static bool badLevel(std::vector<Hexa> & hexa,int stepCount);
	static void nearSameColorChange(Hexa& hexa);
};

#endif