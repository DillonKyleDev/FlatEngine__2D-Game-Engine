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
	std::shared_ptr<Texture> transformArrow = std::make_shared<Texture>();
	std::shared_ptr<Texture> cameraTexture = std::make_shared<Texture>();
	std::shared_ptr<Texture> keyFrameIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> timelineScrubberIcon = std::make_shared<Texture>();

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
	SDL_Texture* keyFrameTexture = nullptr;
	SDL_Texture* timelineScrubberTexture = nullptr;

	void CreateIcons()
	{
		uv0 = ImVec2(0.0f, 0.0f);
		uv1 = ImVec2(1.0f, 1.0f);
		tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
		bg_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

		// Play Icon
		playIcon->loadFromFile("assets/images/icons/PlayIcon.png");
		playTexture = playIcon->getTexture();
		// Pause Icon
		pauseIcon->loadFromFile("assets/images/icons/PauseIcon.png");
		pauseTexture = pauseIcon->getTexture();
		// Stop Icon
		stopIcon->loadFromFile("assets/images/icons/StopIcon.png");
		stopTexture = stopIcon->getTexture();

		// Expander Icons
		expandIcon->loadFromFile("assets/images/icons/Expand.png");
		expandTexture = expandIcon->getTexture();
		expandFlippedIcon->loadFromFile("assets/images/icons/ExpandFlipped.png");
		expandFlippedTexture = expandFlippedIcon->getTexture();
		float expandWidth = (float)expandIcon->getWidth();
		float expandHeight = (float)expandIcon->getHeight();
		// Trashcan Icon
		trashIcon->loadFromFile("assets/images/icons/Trashcan.png");
		trashTexture = trashIcon->getTexture();
		// Open File Icon
		openFileIcon->loadFromFile("assets/images/icons/OpenFileIcon.png");
		openFileTexture = openFileIcon->getTexture();
		//float openFileWidth = (float)openFileIcon->getWidth();
		//float openFileHeight = (float)openFileIcon->getHeight();
		//ImVec2 openFileSize = ImVec2(12, 12);
		//ImVec4 openFile_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
		//std::string openFileID = "##openFileIconAnimator";
		// New File Icon
		newFileIcon->loadFromFile("assets/images/icons/NewFileIcon.png");
		newFileTexture = newFileIcon->getTexture();
		saveFileIcon->loadFromFile("assets/images/icons/SaveIcon.png");
		saveFileTexture = saveFileIcon->getTexture();
		keyFrameIcon->loadFromFile("assets/images/icons/KeyFrameIcon.png");
		keyFrameTexture = keyFrameIcon->getTexture();

		timelineScrubberIcon->loadFromFile("assets/images/icons/timelineScrubber.png");
		timelineScrubberTexture = timelineScrubberIcon->getTexture();
	}
}	
}