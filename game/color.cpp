#include"color.h"

const Color Color::pureBlack(0, 0, 0);
const Color Color::pureWhite(255, 255, 255);
const Color Color::pureRed(255, 0, 0);
const Color Color::pureYellow(255, 255, 0);
const Color Color::pureBlue(0, 0, 255);

Color::Color(int r, int g, int b)
{
	mRed = r;
    mGreen = g;
    mBlue = b;
}
Color::Color(float r, float g, float b)
{
    mRed = 255 * r;
    mGreen = 255 * g;
    mBlue = 255 * b;
}

Color::~Color()
{
}

std::string Color::toHex() const
{
	std::stringstream ss;
	ss << std::hex << mRed << mGreen << mBlue;
	return ss.str();
}

void Color::getRGB(int &r,int &g,int &b) const
{
    r = mRed;
    g = mGreen;
    b = mBlue;
    return;
}

void Color::getRGBPercent(float &r,float &g,float &b) const
{
    r = mRed/255.0f;
    g = mGreen/255.0f;
    b = mBlue/255.0f;
    return;
}

int Color::getRed() const
{
    return mRed;
}
int Color::getGreen() const
{
    return mGreen;
}
int Color::getBlue() const
{
    return mBlue;
} 

float Color::getRedPercent() const
{
    return mRed/255.0f;
}
float Color::getGreenPercent() const
{
    return mGreen/255.0f;
}
float Color::getBluePercent() const
{
    return mBlue/255.0f;
}

bool Color::operator==(const Color& other) const
{
    return mRed == other.getRed() && mGreen == other.getGreen() && mBlue == other.getBlue();
}

bool Color::operator!=(const Color& other) const
{
    return !(*this == other);
}
