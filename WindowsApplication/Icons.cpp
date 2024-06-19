#include "FlatEngine.h"
#include "Sprite.h"
#include "imgui.h"

namespace FlatEngine { namespace FlatGui {

	// Icons / Images
	std::shared_ptr<Texture> transformArrow = std::make_shared<Texture>();
	std::shared_ptr<Texture> transformArrowAllWhite = std::make_shared<Texture>();
	std::shared_ptr<Texture> transformArrowXWhite = std::make_shared<Texture>();
	std::shared_ptr<Texture> transformArrowYWhite = std::make_shared<Texture>();
	std::shared_ptr<Texture> cameraIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> playIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> pauseIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> stopIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> nextFrameIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> expandIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> expandFlippedIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> trashIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> openFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> newFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> saveFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> saveAsFileIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> keyFrameIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> timelineScrubberIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> threeDotsIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> showIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> hideIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> prefabCubeIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> leftIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> rightIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> downIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> upIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> upLeftIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> upRightIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> downLeftIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> downRightIcon = std::make_shared<Texture>();
	std::shared_ptr<Texture> centerIcon = std::make_shared<Texture>();

	Vector2 uv0 = Vector2(0.0f, 0.0f);
	Vector2 uv1 = Vector2(1.0f, 1.0f);
	Vector4 tint_col = Vector4(1.0, 1.0, 1.0, 1.0f);
	Vector4 bg_col = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

	SDL_Texture* transformArrowTexture = nullptr;
	SDL_Texture* transformArrowAllWhiteTexture = nullptr;
	SDL_Texture* transformArrowXWhiteTexture = nullptr;
	SDL_Texture* transformArrowYWhiteTexture = nullptr;
	SDL_Texture* cameraTexture = nullptr;
	SDL_Texture* playTexture = nullptr;
	SDL_Texture* pauseTexture = nullptr;
	SDL_Texture* stopTexture = nullptr;
	SDL_Texture* nextFrameTexture = nullptr;
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
	SDL_Texture* prefabCubeTexture = nullptr;
	SDL_Texture* leftTexture = nullptr;
	SDL_Texture* rightTexture = nullptr;
	SDL_Texture* upTexture = nullptr;
	SDL_Texture* downTexture = nullptr;
	SDL_Texture* upLeftTexture = nullptr;
	SDL_Texture* upRightTexture = nullptr;
	SDL_Texture* downLeftTexture = nullptr;
	SDL_Texture* downRightTexture = nullptr;
	SDL_Texture* centerTexture = nullptr;

	void CreateIcons()
	{
		transformArrow->loadFromFile("assets/images/icons/transformArrow.png");
		transformArrowTexture = transformArrow->getTexture();
		transformArrowAllWhite->loadFromFile("assets/images/icons/transformArrowAllWhite.png");
		transformArrowAllWhiteTexture = transformArrowAllWhite->getTexture();
		transformArrowXWhite->loadFromFile("assets/images/icons/transformArrowXWhite.png");
		transformArrowXWhiteTexture = transformArrowXWhite->getTexture();
		transformArrowYWhite->loadFromFile("assets/images/icons/transformArrowYWhite.png");
		transformArrowYWhiteTexture = transformArrowYWhite->getTexture();
		cameraIcon->loadFromFile("assets/images/icons/camera.png");
		cameraTexture = cameraIcon->getTexture();
		// Attribution for Camera icon
		/*<a href = "https://www.flaticon.com/free-icons/movie-camera" title = "movie camera icons">Movie camera icons created by Freepik - Flaticon< / a>*/

		playIcon->loadFromFile("assets/images/icons/play.png");
		playTexture = playIcon->getTexture();
		pauseIcon->loadFromFile("assets/images/icons/pause.png");
		pauseTexture = pauseIcon->getTexture();
		stopIcon->loadFromFile("assets/images/icons/stop.png");
		stopTexture = stopIcon->getTexture();
		nextFrameIcon->loadFromFile("assets/images/icons/next.png");
		nextFrameTexture = nextFrameIcon->getTexture();
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
		keyFrameIcon->loadFromFile("assets/images/icons/keyFrame.png");
		keyFrameTexture = keyFrameIcon->getTexture();
		threeDotsIcon->loadFromFile("assets/images/icons/threeDots.png");
		threeDotsTexture = threeDotsIcon->getTexture();
		showIcon->loadFromFile("assets/images/icons/show.png");
		showTexture = showIcon->getTexture();
		hideIcon->loadFromFile("assets/images/icons/hide.png");
		hideTexture = hideIcon->getTexture();
		prefabCubeIcon->loadFromFile("assets/images/icons/prefabCube.png");
		prefabCubeTexture = prefabCubeIcon->getTexture();
		leftIcon->loadFromFile("assets/images/icons/left.png");
		leftTexture = leftIcon->getTexture();
		rightIcon->loadFromFile("assets/images/icons/right.png");
		rightTexture = rightIcon->getTexture();
		downIcon->loadFromFile("assets/images/icons/down.png");
		downTexture = downIcon->getTexture();
		upIcon->loadFromFile("assets/images/icons/up.png");
		upTexture = upIcon->getTexture();
		upLeftIcon->loadFromFile("assets/images/icons/upLeft.png");
		upLeftTexture = upLeftIcon->getTexture();
		upRightIcon->loadFromFile("assets/images/icons/upRight.png");
		upRightTexture = upRightIcon->getTexture();
		downLeftIcon->loadFromFile("assets/images/icons/downLeft.png");
		downLeftTexture = downLeftIcon->getTexture();
		downRightIcon->loadFromFile("assets/images/icons/downRight.png");
		downRightTexture = downRightIcon->getTexture();
		centerIcon->loadFromFile("assets/images/icons/center.png");
		centerTexture = centerIcon->getTexture();


		timelineScrubberIcon->loadFromFile("assets/images/icons/timelineScrubber.png");
		timelineScrubberTexture = timelineScrubberIcon->getTexture();
	}
}	
}