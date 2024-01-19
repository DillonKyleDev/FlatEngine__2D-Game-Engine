#include "Texture.h"


//Initializes variables
Texture::Texture(std::string path)
{
	//Initialize
	this->texture = NULL;
	this->surface = NULL;
	this->textureWidth = 0;
	this->textureHeight = 0;
	if (path != "")
		this->loadFromFile(path);
	this->font = TTF_OpenFont("assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
}


//Deallocatees memory
Texture::~Texture()
{
	this->freeTexture();
	this->freeSurface();
}


bool Texture::loadFromFile(std::string path)
{
	//Remove existing texture
	this->freeTexture();

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
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0, 0));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(Window::GetRenderer(), loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Set image dimensions
			this->textureWidth = loadedSurface->w;
			this->textureHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	this->texture = newTexture;
	return this->texture != NULL;
}


bool Texture::loadSurface(std::string path, SDL_Surface* screenSurface)
{
	this->freeSurface();

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
		if (this->surface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	this->surface = optimizedSurface;
	return this->surface != NULL;
}


bool Texture::blitSurface(SDL_Surface* screenSurface)
{
	return SDL_BlitSurface(this->surface, NULL, screenSurface, NULL);
}


//Creates image from font string
bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
{
	//Get rid of preexisting texture
	this->freeSurface();

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

		if (this->texture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			this->textureWidth = textSurface->w;
			this->textureHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	this->texture = newTexture;
	return this->texture != NULL;
}


//Deallocates texture
void Texture::freeTexture()
{
	//Free texture if it exists
	if (this->texture != NULL)
	{
		SDL_DestroyTexture(this->texture);
		this->texture = NULL;
		this->textureWidth = 0;
		this->textureHeight = 0;
	}
}


SDL_Texture* Texture::getTexture()
{
	return this->texture;
}


//Deallocates texture
void Texture::freeSurface()
{
	//Free texture if it exists
	if (this->surface != NULL)
	{
		SDL_FreeSurface(this->surface);
		this->surface = NULL;
	}
}


//Set color modulation
void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{

}


//Set blending
void Texture::setBlendMode(SDL_BlendMode blending)
{

}


//Set alpha modulation
void Texture::setAlpha(Uint8 alpha)
{

}


//Render texture at given point not using ImGui method, just SDL_2
void Texture::render(int x=Window::WINDOW_WIDTH/2, int y=Window::WINDOW_HEIGHT/2, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space
	SDL_Rect renderQuad = { x, y, this->textureWidth, this->textureHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	//SDL_RenderCopyEx(renderer, this->texture, clip, &renderQuad, angle, center, flip);
	SDL_RenderCopy(Window::GetRenderer(), this->texture, clip, &renderQuad);
}


//Gets image dimensions
int Texture::getWidth()
{
	return this->textureWidth;
}
int Texture::getHeight()
{
	return this->textureHeight;
}


void Texture::setDimensions(int width, int height)
{
	this->textureWidth = width;
	this->textureHeight = height;
}

void Texture::setFont(std::string path)
{
	this->font = TTF_OpenFont(path.c_str(), 46);
}
