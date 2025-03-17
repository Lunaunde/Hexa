#ifndef STATE_H
#define STATE_H
#define sta State::getInstance()
#include<vector>
#include "hexa.h"

class State
{
public:
    ~State();

    static State* getInstance();
    std::vector<Hexa> mHexas;
private:
    State();
    static State* instance;
};

class HexaColorBackup
{
public:
    HexaColorBackup(std::vector<Hexa> & hexas);
    ~HexaColorBackup();
    void restore(std::vector<Hexa> & hexas);
private:
    std::vector<char> mHexaColorBackup;
};

#endif