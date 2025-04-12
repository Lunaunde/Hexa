#ifndef COLOR_H
#define COLOR_H

#include<string>
#include<sstream>

class Color {
public:
    explicit Color(int r, int g, int b);
    explicit Color(float r, float g, float b);
    ~Color();

    static const Color pureBlack;
    static const Color pureWhite;
    static const Color pureRed;
    static const Color pureYellow;
    static const Color pureBlue;

    std::string toHex() const;
    void getRGB(int& r, int& g, int& b) const;
    void getRGBPercent(float& r, float& g, float& b) const;

    int getRed() const;
    int getGreen() const;
    int getBlue() const;

    float getRedPercent() const;
    float getGreenPercent() const;
    float getBluePercent() const;

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

private:
    int mRed, mGreen, mBlue;
};

#endif