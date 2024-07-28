#include "Texture.h"

namespace FlatEngine
{
	//Initializes variables
	Texture::Texture(std::string path)
	{
		//Initialize
		texture = NULL;
		surface = NULL;
		textureWidth = 0;
		textureHeight = 0;
		if (path != "")
			LoadFromFile(path);
		font = TTF_OpenFont("Source/assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
	}


	//Deallocatees memory
	Texture::~Texture()
	{
		FreeTexture();
		FreeSurface();
	}


	bool Texture::LoadFromFile(std::string path)
	{
		//Remove existing texture
		FreeTexture();

		//The final texture
		SDL_Texture* newTexture = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}
		else
		{
			//Color key image **Whatever color you put in here it will treat as transparent**
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 0));

			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(Window::GetRenderer(), loadedSurface);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}
			else
			{
				//Set image dimensions
				textureWidth = loadedSurface->w;
				textureHeight = loadedSurface->h;
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//Return success
		texture = newTexture;
		return texture != NULL;
	}

	bool Texture::LoadSurface(std::string path, SDL_Surface* screenSurface)
	{
		FreeSurface();

		//The final optimized image
		SDL_Surface* optimizedSurface = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());

		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}
		else
		{
			//Convert surface to screen format
			optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
			if (surface == NULL)
			{
				printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//Return success
		surface = optimizedSurface;
		return surface != NULL;
	}

	//Creates image from font string
	bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
	{
		//Get rid of preexisting texture
		FreeSurface();

		//The final texture
		SDL_Texture* newTexture = NULL;

		//Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);

		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(Window::GetRenderer(), textSurface);

			if (texture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				textureWidth = textSurface->w;
				textureHeight = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		//Return success
		texture = newTexture;
		return texture != NULL;
	}

	//Deallocates texture
	void Texture::FreeTexture()
	{
		//Free texture if it exists
		if (texture != NULL)
		{
			SDL_DestroyTexture(texture);
			texture = NULL;
			textureWidth = 0;
			textureHeight = 0;
		}
	}

	SDL_Texture* Texture::GetTexture()
	{
		return texture;
	}

	//Deallocates texture
	void Texture::FreeSurface()
	{
		//Free texture if it exists
		if (surface != NULL)
		{
			SDL_FreeSurface(surface);
			surface = NULL;
		}
	}

	//Gets image dimensions
	int Texture::GetWidth()
	{
		return textureWidth;
	}
	int Texture::GetHeight()
	{
		return textureHeight;
	}

	void Texture::SetDimensions(int width, int height)
	{
		textureWidth = width;
		textureHeight = height;
	}

	void Texture::SetFont(std::string path)
	{
		font = TTF_OpenFont(path.c_str(), 46);
	}
}