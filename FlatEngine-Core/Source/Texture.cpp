#include "Texture.h"

namespace FlatEngine
{
	//Initializes variables
	Texture::Texture(std::string path)
	{
		//Initialize
		m_path = path;
		m_texture = NULL;
		m_surface = NULL;
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_font = TTF_OpenFont("Source/assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
		if (path != "")
			LoadFromFile(path);
	}

	Texture::Texture(const Texture* toCopy)
	{
		m_path = toCopy->m_path;	
		m_texture = NULL;
		m_surface = NULL;
		m_textureWidth = toCopy->m_textureWidth;
		m_textureHeight = toCopy->m_textureHeight;
		m_font = TTF_OpenFont("Source/assets/fonts/Cinzel/Cinzel-Black.ttf", 46);
		if (m_path != "")
		LoadFromFile(m_path);
	}


	//Deallocatees memory
	Texture::~Texture()
	{
		FreeTexture();
		FreeSurface();
	}


	bool Texture::LoadFromFile(std::string path)
	{
		m_path = path;

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
				m_textureWidth = loadedSurface->w;
				m_textureHeight = loadedSurface->h;
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//Return success
		m_texture = newTexture;
		return m_texture != NULL;
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
			if (m_surface == NULL)
			{
				printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//Return success
		m_surface = optimizedSurface;
		return m_surface != NULL;
	}

	//Creates image from font string
	bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
	{
		//Get rid of preexisting texture
		FreeSurface();		

		//Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);

		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			m_texture = SDL_CreateTextureFromSurface(Window::GetRenderer(), textSurface);

			if (m_texture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				m_textureWidth = textSurface->w;
				m_textureHeight = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		//Return success
		return m_texture != NULL;
	}

	//Deallocates texture
	void Texture::FreeTexture()
	{			
		//Free texture if it exists
		if (m_texture != NULL)
		{
			SDL_DestroyTexture(m_texture);
			m_texture = NULL;
			m_textureWidth = 0;
			m_textureHeight = 0;
		}
	}

	SDL_Texture* Texture::GetTexture()
	{
		return m_texture;
	}

	//Deallocates texture
	void Texture::FreeSurface()
	{
		//Free texture if it exists
		if (m_surface != NULL)
		{
			SDL_FreeSurface(m_surface);
			m_surface = NULL;
		}
	}

	//Gets image dimensions
	int Texture::GetWidth()
	{
		return m_textureWidth;
	}
	int Texture::GetHeight()
	{
		return m_textureHeight;
	}

	void Texture::SetDimensions(int width, int height)
	{
		m_textureWidth = width;
		m_textureHeight = height;
	}

	void Texture::SetFont(std::string path)
	{
		m_font = TTF_OpenFont(path.c_str(), 46);
	}
}