#include "FlatEngine.h"
#include "Sprite.h"
#include "imgui.h"

namespace FlatEngine { namespace FlatGui {

	// Icons
	std::shared_ptr<Texture> playIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> pauseIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> stopIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> expandIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> expandFlippedIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> trashIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> openFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> newFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> saveFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> saveAsFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> transformArrow = std::make_shared<Texture>();
	std::shared_ptr<Texture> cameraTexture = std::make_shared<Texture>();
	std::shared_ptr<Texture> keyFrameIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> timelineScrubberIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> threeDotsIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> showIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> hideIcon = std::make_shared<Texture>();

	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);
	ImVec4 tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
	ImVec4 bg_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

	SDL_Texture* playTexture = nullptr;
	SDL_Texture* pauseTexture = nullptr;
	SDL_Texture* stopTexture = nullptr;
	SDL_Texture* expandTexture = nullptr;
	SDL_Texture* expandFlippedTexture = nullptr;
	SDL_Texture* trashTexture = nullptr;
	SDL_Texture* openFileTexture = nullptr;
	SDL_Texture* newFileTexture = nullptr;
	SDL_Texture* saveFileTexture = nullptr;
	SDL_Texture* saveAsFileTexture = nullptr;
	SDL_Texture* keyFrameTexture = nullptr;
	SDL_Texture* timelineScrubberTexture = nullptr;
	SDL_Texture* threeDotsTexture = nullptr;
	SDL_Texture* showTexture = nullptr;
	SDL_Texture* hideTexture = nullptr;

	void CreateIcons()
	{
		playIcon->loadFromFile("assets/images/icons/PlayIcon.png");
		playTexture = playIcon->getTexture();
		pauseIcon->loadFromFile("assets/images/icons/PauseIcon.png");
		pauseTexture = pauseIcon->getTexture();
		stopIcon->loadFromFile("assets/images/icons/StopIcon.png");
		stopTexture = stopIcon->getTexture();
		expandIcon->loadFromFile("assets/images/icons/expand.png");
		expandTexture = expandIcon->getTexture();
		expandFlippedIcon->loadFromFile("assets/images/icons/collapse.png");
		expandFlippedTexture = expandFlippedIcon->getTexture();
		trashIcon->loadFromFile("assets/images/icons/delete.png");
		trashTexture = trashIcon->getTexture();
		openFileIcon->loadFromFile("assets/images/icons/open.png");
		openFileTexture = openFileIcon->getTexture();
		newFileIcon->loadFromFile("assets/images/icons/new.png");
		newFileTexture = newFileIcon->getTexture();
		saveFileIcon->loadFromFile("assets/images/icons/save.png");
		saveFileTexture = saveFileIcon->getTexture();
		saveAsFileIcon->loadFromFile("assets/images/icons/saveAs.png");
		saveAsFileTexture = saveAsFileIcon->getTexture();
		keyFrameIcon->loadFromFile("assets/images/icons/KeyFrameIcon.png");
		keyFrameTexture = keyFrameIcon->getTexture();
		threeDotsIcon->loadFromFile("assets/images/icons/threeDots.png");
		threeDotsTexture = threeDotsIcon->getTexture();
		showIcon->loadFromFile("assets/images/icons/show.png");
		showTexture = showIcon->getTexture();
		hideIcon->loadFromFile("assets/images/icons/hide.png");
		hideTexture = hideIcon->getTexture();

		timelineScrubberIcon->loadFromFile("assets/images/icons/timelineScrubber.png");
		timelineScrubberTexture = timelineScrubberIcon->getTexture();
	}
}	
}