#include "FlatEngine.h"
#include "FlatGui.h"
#include "Texture.h"
#include "Sprite.h"

#include "imgui.h"

using Texture = FlatEngine::Texture;

namespace FlatGui {

	// Icons / Images
	Texture transformArrow = Texture();
	Texture transformArrowAllWhite = Texture();
	Texture transformArrowXWhite = Texture();
	Texture transformArrowYWhite = Texture();
	Texture cameraIcon = Texture();
	Texture playIcon = Texture();
	Texture pauseIcon = Texture();
	Texture stopIcon = Texture();
	Texture nextFrameIcon = Texture();
	Texture expandIcon = Texture();
	Texture expandFlippedIcon = Texture();
	Texture trashIcon = Texture();
	Texture openFileIcon = Texture();
	Texture newFileIcon = Texture();
	Texture saveFileIcon = Texture();
	Texture saveAsFileIcon = Texture();
	Texture keyFrameIcon = Texture();
	Texture threeDotsIcon = Texture();
	Texture showIcon = Texture();
	Texture hideIcon = Texture();
	Texture prefabCubeIcon = Texture();
	Texture leftIcon = Texture();
	Texture rightIcon = Texture();
	Texture downIcon = Texture();
	Texture upIcon = Texture();
	Texture upLeftIcon = Texture();
	Texture upRightIcon = Texture();
	Texture downLeftIcon = Texture();
	Texture downRightIcon = Texture();
	Texture centerIcon = Texture();

	Vector2 uv0 = Vector2(0.0f, 0.0f);
	Vector2 uv1 = Vector2(1.0f, 1.0f);
	Vector4 tint_col = Vector4(1.0, 1.0, 1.0, 1.0f);
	Vector4 bg_col = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

	void CreateIcons()
	{
		transformArrow.LoadFromFile("Source/assets/images/icons/transformArrow.png");
		transformArrowAllWhite.LoadFromFile("Source/assets/images/icons/transformArrowAllWhite.png");
		transformArrowXWhite.LoadFromFile("Source/assets/images/icons/transformArrowXWhite.png");
		transformArrowYWhite.LoadFromFile("Source/assets/images/icons/transformArrowYWhite.png");
		cameraIcon.LoadFromFile("Source/assets/images/icons/camera.png");
		// Attribution for Camera icon
		/*<a href = "https://www.flaticon.com/free-icons/movie-camera" title = "movie camera icons">Movie camera icons created by Freepik - Flaticon< / a>*/
		playIcon.LoadFromFile("Source/assets/images/icons/play.png");
		pauseIcon.LoadFromFile("Source/assets/images/icons/pause.png");
		stopIcon.LoadFromFile("Source/assets/images/icons/stop.png");
		nextFrameIcon.LoadFromFile("Source/assets/images/icons/next.png");
		expandIcon.LoadFromFile("Source/assets/images/icons/expand.png");
		expandFlippedIcon.LoadFromFile("Source/assets/images/icons/collapse.png");
		trashIcon.LoadFromFile("Source/assets/images/icons/delete.png");
		openFileIcon.LoadFromFile("Source/assets/images/icons/open.png");
		newFileIcon.LoadFromFile("Source/assets/images/icons/new.png");
		saveFileIcon.LoadFromFile("Source/assets/images/icons/save.png");
		saveAsFileIcon.LoadFromFile("Source/assets/images/icons/saveAs.png");
		keyFrameIcon.LoadFromFile("Source/assets/images/icons/keyFrame.png");
		threeDotsIcon.LoadFromFile("Source/assets/images/icons/threeDots.png");
		showIcon.LoadFromFile("Source/assets/images/icons/show.png");
		hideIcon.LoadFromFile("Source/assets/images/icons/hide.png");
		prefabCubeIcon.LoadFromFile("Source/assets/images/icons/prefabCube.png");
		leftIcon.LoadFromFile("Source/assets/images/icons/left.png");
		rightIcon.LoadFromFile("Source/assets/images/icons/right.png");
		downIcon.LoadFromFile("Source/assets/images/icons/down.png");
		upIcon.LoadFromFile("Source/assets/images/icons/up.png");
		upLeftIcon.LoadFromFile("Source/assets/images/icons/upLeft.png");
		upRightIcon.LoadFromFile("Source/assets/images/icons/upRight.png");
		downLeftIcon.LoadFromFile("Source/assets/images/icons/downLeft.png");
		downRightIcon.LoadFromFile("Source/assets/images/icons/downRight.png");
		centerIcon.LoadFromFile("Source/assets/images/icons/center.png");
	}
}