#include "FlatEngine.h"
#include "Texture.h"
#include "Sprite.h"
#include "Vector4.h"

#include "imgui.h"

using Texture = FlatEngine::Texture;

namespace FlatEngine 
{
	// Icons / Images
	
	// Project Hub
	Texture F_flatEngineLogo = Texture();
	Texture F_flatEngineLogoGradient = Texture();
	Texture F_selectProjectImage = Texture();
	Texture F_projectHubBgImage = Texture();
	// File Explorer
	Texture F_cppFileIcon = Texture();
	Texture F_hFileIcon = Texture();
	Texture F_luaFileIcon = Texture();
	Texture F_pngFileIcon = Texture();
	// Scene View
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


	Vector4 F_transparentColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4 F_whiteColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//////////////////////
	// Components ////////
	//////////////////////
	Vector4 F_componentBorderColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Colliders
	Vector4 F_colliderActiveColor = Vector4(0.19f, 0.9f, 0.2f, 0.5f);
	Vector4 F_colliderInactiveColor = Vector4(0.9f, 0.2f, 0.2f, 0.5f);
	Vector4 F_colliderCollidingColor = Vector4(0.76f, 0.42f, 0.0f, 0.5f);
	// Button Components
	Vector4 F_buttonComponentActiveColor = Vector4(0.19f, 0.9f, 0.2f, 0.5f);
	Vector4 F_buttonComponentInctiveColor = Vector4(0.9f, 0.2f, 0.2f, 0.5f);
	// Canvas Orange
	Vector4 F_canvasBorderColor = Vector4(0.76f, 0.42f, 0.0f, 0.5f);
	// Camera
	Vector4 F_cameraBoxColor = Vector4(1.0f, 0.11f, 0.11f, 0.27f);
	//////////////////////
	// Windows ///////////
	//////////////////////
	Vector4 F_selectProjectBgColor = Vector4(0.08f, 0.08f, 0.10f, 1.0f);
	Vector4 F_windowBgColor = Vector4(0.08f, 0.08f, 0.10f, 1.0f);
	Vector4 F_outerWindowColor = Vector4(0.13f, 0.13f, 0.15f, 1.0f);
	Vector4 F_innerWindowColor = Vector4(0.1f, 0.1f, 0.12f, 1.0f);
	Vector4 F_singleItemColor = Vector4(0.16f, 0.16f, 0.17f, 1.0f);
	Vector4 F_singleItemDark = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 F_windowTitleBg = Vector4(0.25f, 0.25f, 0.25f, 1.0f);
	// Log
	Vector4 F_logTextColor = Vector4(0.75f, 0.75f, 0.75f, 1.0f);
	Vector4 F_logBgColor = Vector4(0.2f, 0.2f, 0.22f, 1.0f);
	Vector4 F_logOutlineColor = Vector4(0.25f, 0.25f, 0.27f, 1.0f);
	// File Explorer
	Vector4 F_explorerFilesBg = Vector4(0.2f, 0.2f, 0.22f, 1.0f);
	// Docking
	Vector4 F_dockingPreviewColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 F_dockingPreviewEmptyColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	// ImGui Key colors
	Vector4 F_frameBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_frameBgActiveColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_frameBgHoveredColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// tabs
	Vector4 F_tabColor = Vector4(0.15f, 0.15f, 0.2f, 1.0f);
	Vector4 F_tabActiveColor = Vector4(0.4f, 0.4f, 0.42f, 1.0f);
	Vector4 F_tabHoveredColor = Vector4(0.4f, 0.4f, 0.42f, 1.0f);
	Vector4 F_tabUnfocusedColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4 F_tabUnfocusedActiveColor = Vector4(0.3f, 0.3f, 0.31f, 1.0f);
	// titles
	Vector4 F_titleBgColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 F_titleBgActiveColor = Vector4(0.2f, 0.2f, 0.25f, 1.0f);
	Vector4 F_titleBgCollapsedColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Text
	Vector4 F_textSelectedBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	// Resizers
	Vector4 F_resizeGripColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 F_resizeGripHoveredColor = Vector4(0.35f, 0.35f, 0.75f, 1.0f);
	Vector4 F_resizeGripActiveColor = Vector4(0.2f, 0.2f, 0.5f, 0.8f);
	// Misc (not sure what they're for)
	Vector4 F_popupBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_navWindowHighlightColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_navHighlightColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_navWindowDimBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	Vector4 F_modalWindowDimBgColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	////////////////////
	// Custom Colors ///
	//////////////////// 
	// Inputs
	Vector4 F_inputColor = Vector4(0.3f, 0.3f, 0.32f, 1.0f);
	// Tables
	Vector4 F_noEditTableTextColor = Vector4(0.75f, 0.75f, 0.81f, 1.0f);
	Vector4 F_noEditTableRowFieldBgColor = Vector4(0.1f, 0.1f, 0.5f, 0.2f);// Vector4(0.3f, 0.35f, 0.55f, 1.0f);// Vector4(0.2f, 0.25f, 0.45f, 1.0f);
	Vector4 F_noEditTableRowValueBgColor = Vector4(0.1f, 0.1f, 0.5f, 0.2f);// Vector4(0.3f, 0.3f, 0.7f, 0.2f);
	Vector4 F_tableCellLightColor = Vector4(0.19f, 0.19f, 0.21f, 1.0f);
	Vector4 F_tableCellDarkColor = Vector4(0.24f, 0.24f, 0.27f, 1.0f);
	// Trees
	Vector4 F_treeSelectableColor = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	Vector4 F_treeSelectableHoveredColor = Vector4(0.3f, 0.35f, 0.65f, 1.0f);
	Vector4 F_treeSelectableActiveColor = Vector4(0.2f, 0.25f, 0.45f, 1.0f);
	Vector4 F_treeSelectableSelectedColor = Vector4(0.5f, 0.5f, 0.8f, 1.0f);
	Vector4 F_hierarchyChildObjectColor = Vector4(0.3f, 0.3f, 0.7f, 0.2f);
	// Combos
	Vector4 F_comboBgColor = Vector4(0.19f, 0.19f, 0.21f, 1.0f);
	Vector4 F_comboHoveredColor = Vector4(0.25f, 0.25f, 0.26f, 1.0f);
	Vector4 F_comboSelectableColor = Vector4(0.34f, .34f, .4f, 1.0f);
	Vector4 F_comboSelectedColor = Vector4(0.45f, 0.45f, 0.50f, 1.0f);
	Vector4 F_comboHighlightedColor = Vector4(0.25f, 0.25f, 0.28f, 1.0f);
	Vector4 F_comboArrowColor = Vector4(0.11f, 0.11f, 0.13f, 1.0f);
	Vector4 F_comboArrowHoveredColor = Vector4(0.15f, 0.15f, 0.16f, 1.0f);
	// Buttons
	Vector4 F_buttonColor = Vector4(0.3f, 0.3f, 0.65f, 1.0f);
	Vector4 F_buttonHoveredColor = Vector4(0.35f, 0.35f, 0.75f, 1.0f);
	Vector4 F_buttonActiveColor = Vector4(0.2f, 0.2f, 0.5f, 0.8f);
	Vector4 F_imageButtonColor = Vector4(0.18f, 0.18f, 0.18f, 1.0f);
	Vector4 F_imageButtonDarkColor = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	Vector4 F_imageButtonHoveredColor = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	Vector4 F_imageButtonActiveColor = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4 F_imageButtonTintColor = F_whiteColor;
	// Sliders/Drags
	Vector4 F_sliderColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 F_sliderHoveredColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 F_sliderActiveColor = Vector4(0.09f, 0.09f, 0.13f, 1.0f);
	Vector4 F_dragColor = Vector4(0.2f, 0.2f, 0.2f, 0.0f);
	Vector4 F_dragHoveredColor = Vector4(0.45f, 0.45f, 0.45f, 1.0f);
	Vector4 F_dragActiveColor = Vector4(0.10f, 0.10f, 0.10f, 1.0f);
	// Checkboxes
	Vector4 F_checkboxBgColor = Vector4(0.28f, 0.28f, 0.29f, 1.0f);
	Vector4 F_checkboxCheckColor = Vector4(0.45f, 0.45f, 0.9f, 1.0f);
	Vector4 F_checkboxHoveredColor = Vector4(0.31f, 0.31f, 0.32f, 1.0f);
	Vector4 F_checkboxActiveColor = Vector4(0.15f, 0.15f, 0.23f, 1.0f);

	void CreateIcons()
	{		
		// Project Hub
		F_flatEngineLogo.LoadFromFile("Source/assets/images/icons/FlatEngineLogo.png");
		F_flatEngineLogoGradient.LoadFromFile("Source/assets/images/icons/FlatEngineLogoGradient.png");
		F_selectProjectImage.LoadFromFile("Source/assets/images/icons/selectProject.png");
		F_projectHubBgImage.LoadFromFile("Source/assets/images/icons/projectHubBg.png");
		// File Explorer
		F_cppFileIcon.LoadFromFile("Source/assets/images/icons/cpp.png");
		F_hFileIcon.LoadFromFile("Source/assets/images/icons/h.png");
		F_luaFileIcon.LoadFromFile("Source/assets/images/icons/lua.png");
		F_pngFileIcon.LoadFromFile("Source/assets/images/icons/png.png");
		// Scene View
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