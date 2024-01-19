#pragma once
#include <SDL.h>
#include <string>
#include <sstream>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Window.h"


class Texture
{
public:
	Texture(std::string path = "");
	~Texture();
	bool loadFromFile(std::string path);
	bool loadSurface(std::string path, SDL_Surface* screenSurface);
	bool blitSurface(SDL_Surface* screenSurface);
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font);
	//Deallocates texture
	void freeTexture();
	//Returns SDL_Texture
	SDL_Texture* getTexture();
	//Deallocates surface
	void freeSurface();
	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	//Set blending
	void setBlendMode(SDL_BlendMode blending);
	//Set alpha modulation
	void setAlpha(Uint8 alpha);
	//Render texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//Get image dimensions
	int getWidth();
	int getHeight();
	//Set image dimensions
	void setDimensions(int width, int height);
	void setFont(std::string path);

private:
	//The actual hardware texture
	SDL_Texture* texture;
	//Loaded from file surface
	SDL_Surface* surface;

	TTF_Font* font;

	//Image dimensions
	int textureWidth;
	int textureHeight;
};