#include "FlatEngine.h"
#include "Texture.h"
#include "Sprite.h"

#include "imgui.h"

using Texture = FlatEngine::Texture;

namespace FlatEngine 
{
	// Icons / Images
	std::string F_ResourceFailedToLoadImagePath = "Source/assets/images/icons/resourceFailedToLoad.png";	
	Texture F_selectProjectImage = Texture();
	Texture F_transformArrow = Texture();
	Texture F_transformArrowAllWhite = Texture();
	Texture F_transformArrowXWhite = Texture();
	Texture F_transformArrowYWhite = Texture();
	Texture F_cameraIcon = Texture();
	Texture F_playIcon = Texture();
	Texture F_pauseIcon = Texture();
	Texture F_stopIcon = Texture();
	Texture F_nextFrameIcon = Texture();
	Texture F_expandIcon = Texture();
	Texture F_expandFlippedIcon = Texture();
	Texture F_trashIcon = Texture();
	Texture F_openFileIcon = Texture();
	Texture F_newFileIcon = Texture();
	Texture F_saveFileIcon = Texture();
	Texture F_saveAsFileIcon = Texture();
	Texture F_keyFrameIcon = Texture();
	Texture F_threeDotsIcon = Texture();
	Texture F_showIcon = Texture();
	Texture F_hideIcon = Texture();
	Texture F_prefabCubeIcon = Texture();
	Texture F_leftIcon = Texture();
	Texture F_rightIcon = Texture();
	Texture F_downIcon = Texture();
	Texture F_upIcon = Texture();
	Texture F_upLeftIcon = Texture();
	Texture F_upRightIcon = Texture();
	Texture F_downLeftIcon = Texture();
	Texture F_downRightIcon = Texture();
	Texture F_centerIcon = Texture();

	Vector2 F_uv0 = Vector2(0.0f, 0.0f);
	Vector2 F_uv1 = Vector2(1.0f, 1.0f);
	Vector4 F_tint_col = Vector4(1.0, 1.0, 1.0, 1.0f);
	Vector4 F_bg_col = Vector4(1.0f, 1.0f, 1.0f, 0.0f);

	void CreateIcons()
	{		
		F_selectProjectImage.LoadFromFile("Source/assets/images/icons/selectProject.png");
		F_transformArrow.LoadFromFile("Source/assets/images/icons/transformArrow.png");
		F_transformArrowAllWhite.LoadFromFile("Source/assets/images/icons/transformArrowAllWhite.png");
		F_transformArrowXWhite.LoadFromFile("Source/assets/images/icons/transformArrowXWhite.png");
		F_transformArrowYWhite.LoadFromFile("Source/assets/images/icons/transformArrowYWhite.png");
		F_cameraIcon.LoadFromFile("Source/assets/images/icons/camera.png");
		// Attribution for Camera icon
		/*<a href = "https://www.flaticon.com/free-icons/movie-camera" title = "movie camera icons">Movie camera icons created by Freepik - Flaticon< / a>*/
		F_playIcon.LoadFromFile("Source/assets/images/icons/play.png");
		F_pauseIcon.LoadFromFile("Source/assets/images/icons/pause.png");
		F_stopIcon.LoadFromFile("Source/assets/images/icons/stop.png");
		F_nextFrameIcon.LoadFromFile("Source/assets/images/icons/next.png");
		F_expandIcon.LoadFromFile("Source/assets/images/icons/expand.png");
		F_expandFlippedIcon.LoadFromFile("Source/assets/images/icons/collapse.png");
		F_trashIcon.LoadFromFile("Source/assets/images/icons/delete.png");
		F_openFileIcon.LoadFromFile("Source/assets/images/icons/open.png");
		F_newFileIcon.LoadFromFile("Source/assets/images/icons/new.png");
		F_saveFileIcon.LoadFromFile("Source/assets/images/icons/save.png");
		F_saveAsFileIcon.LoadFromFile("Source/assets/images/icons/saveAs.png");
		F_keyFrameIcon.LoadFromFile("Source/assets/images/icons/keyFrame.png");
		F_threeDotsIcon.LoadFromFile("Source/assets/images/icons/threeDots.png");
		F_showIcon.LoadFromFile("Source/assets/images/icons/show.png");
		F_hideIcon.LoadFromFile("Source/assets/images/icons/hide.png");
		F_prefabCubeIcon.LoadFromFile("Source/assets/images/icons/prefabCube.png");
		F_leftIcon.LoadFromFile("Source/assets/images/icons/left.png");
		F_rightIcon.LoadFromFile("Source/assets/images/icons/right.png");
		F_downIcon.LoadFromFile("Source/assets/images/icons/down.png");
		F_upIcon.LoadFromFile("Source/assets/images/icons/up.png");
		F_upLeftIcon.LoadFromFile("Source/assets/images/icons/upLeft.png");
		F_upRightIcon.LoadFromFile("Source/assets/images/icons/upRight.png");
		F_downLeftIcon.LoadFromFile("Source/assets/images/icons/downLeft.png");
		F_downRightIcon.LoadFromFile("Source/assets/images/icons/downRight.png");
		F_centerIcon.LoadFromFile("Source/assets/images/icons/center.png");
	}
}