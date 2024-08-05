#include "Vector4.h"


//////////////////////
//Global      Colors//
//  //////VV///////  /
//     UUUUUUUU     //
///\-------------/////
//////////////////////
namespace FlatEngine
{
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
}