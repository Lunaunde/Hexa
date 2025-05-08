#include"texture.h"
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& path, unsigned int unit)
{
	mUnit = unit;

	int channel;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channel, STBI_rgb_alpha);

	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0+mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (mWidth == 0 || mHeight == 0) {
		std::cerr << "Failed to load texture: " << path << std::endl;
	}
}

Texture::~Texture()
{
	if(mTexture!=0)
        glDeleteTextures(1, &mTexture);
}

int Texture::getWidth() const
{
	return mWidth;
}
int Texture::getHeight() const
{
	return mHeight;
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0+mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);
}