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
    std::vector<Hexa>& getHexas();
    std::vector<Hexa*>& getSteps();
    HexaColorBackup& getHexaColorBackup();

private:
    State();
    std::vector<Hexa> mHexas;
    std::vector<Hexa*> mSteps;
    HexaColorBackup mHexaColorBackup;
    static State* instance;
};

#endif