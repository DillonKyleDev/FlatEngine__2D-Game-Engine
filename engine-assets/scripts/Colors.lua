--------------------------
-- Global Engine Colors --
--------------------------
F_Colors = {
	transparentColor = Vector4:new(0.0, 0.0, 0.0, 0.0),
	whiteColor = Vector4:new(1.0, 1.0, 1.0, 1.0),
	----------------------
	-- Components --------
	----------------------
	componentBorderColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Colliders
	colliderActiveColor = Vector4:new(0.19, 0.9, 0.2, 0.5),
	colliderInactiveColor = Vector4:new(0.9, 0.2, 0.2, 0.5),
	colliderCollidingColor = Vector4:new(0.76, 0.42, 0.0, 0.5),
	-- Button Components
	buttonComponentActiveColor = Vector4:new(0.19, 0.9, 0.2, 0.5),
	buttonComponentInctiveColor = Vector4:new(0.9, 0.2, 0.2, 0.5),
	-- Canvas Orange
	canvasBorderColor = Vector4:new(0.76, 0.42, 0.0, 0.5),
	-- Camera
	cameraBoxColor = Vector4:new(1.0, 0.11, 0.11, 0.27),
	----------------------
	-- Windows -----------
	----------------------
	selectProjectBgColor = Vector4:new(0.08, 0.08, 0.10, 1.0),
	windowBgColor = Vector4:new(0.08, 0.08, 0.10, 1.0),
	outerWindowColor = Vector4:new(0.13, 0.13, 0.15, 1.0),
	innerWindowColor = Vector4:new(0.1, 0.1, 0.12, 1.0),
	singleItemColor = Vector4:new(0.16, 0.16, 0.17, 1.0),
	singleItemDark = Vector4:new(0.09, 0.09, 0.13, 1.0),
	windowTitleBg = Vector4:new(0.25, 0.25, 0.25, 1.0),
	-- Log
	logTextColor = Vector4:new(0.75, 0.75, 0.75, 1.0),
	logBgColor = Vector4:new(0.2, 0.2, 0.22, 1.0),
	logOutlineColor = Vector4:new(0.25, 0.25, 0.27, 1.0),
	-- File Explorerfefsfef
	explorerFilesBg = Vector4:new(0.2, 0.2, 0.22, 1.0),
	-- Docking
	dockingPreviewColor = Vector4:new(0.3, 0.3, 0.65, 1.0),
	dockingPreviewEmptyColor = Vector4:new(0.3, 0.3, 0.65, 1.0),
	-- ImGui Key colors
	frameBgColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgActiveColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgHoveredColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- tabs
	tabColor = Vector4:new(0.15, 0.15, 0.2, 1.0),
	tabActiveColor = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabHoveredColor = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabUnfocusedColor = Vector4:new(0.1, 0.1, 0.1, 1.0),
	tabUnfocusedActiveColor = Vector4:new(0.3, 0.3, 0.31, 1.0),
	-- titles
	titleBgColor = Vector4:new(0.0, 0.0, 0.0, 1.0),
	titleBgActiveColor = Vector4:new(0.2, 0.2, 0.25, 1.0),
	titleBgCollapsedColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Text
	textSelectedBgColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Resizers
	resizeGripColor = Vector4:new(0.3, 0.3, 0.65, 1.0),
	resizeGripHoveredColor = Vector4:new(0.35, 0.35, 0.75, 1.0),
	resizeGripActiveColor = Vector4:new(0.2, 0.2, 0.5, 0.8),
	-- Misc (not sure what they're or)
	popupBgColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowHighlightColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navHighlightColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowDimBgColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	modalWindowDimBgColor = Vector4:new(0.2, 0.2, 0.2, 1.0),
	---------------------
	--- Custom Colors ---
	---------------------
	-- Inputs
	inputColor = Vector4:new(0.3, 0.3, 0.32, 1.0),
	-- Tables
	noEditTableTextColor = Vector4:new(0.75, 0.75, 0.81, 1.0),
	noEditTableRowFieldBgColor = Vector4:new(0.1, 0.1, 0.5, 0.2),
	noEditTableRowValueBgColor = Vector4:new(0.1, 0.1, 0.5, 0.2),
	tableCellLightColor = Vector4:new(0.19, 0.19, 0.21, 1.0),
	tableCellDarkColor = Vector4:new(0.24, 0.24, 0.27, 1.0),
	-- Trees
	treeSelectableColor = Vector4:new(0.15, 0.15, 0.15, 1.0),
	treeSelectableHoveredColor = Vector4:new(0.3, 0.35, 0.65, 1.0),
	treeSelectableActiveColor = Vector4:new(0.2, 0.25, 0.45, 1.0),
	treeSelectableSelectedColor = Vector4:new(0.5, 0.5, 0.8, 1.0),
	hierarchyChildObjectColor = Vector4:new(0.3, 0.3, 0.7, 0.2),
	-- Combos
	comboBgColor = Vector4:new(0.19, 0.19, 0.21, 1.0),
	comboHoveredColor = Vector4:new(0.25, 0.25, 0.26, 1.0),
	comboSelectableColor = Vector4:new(0.34, .34, .4, 1.0),
	comboSelectedColor = Vector4:new(0.45, 0.45, 0.50, 1.0),
	comboHighlightedColor = Vector4:new(0.25, 0.25, 0.28, 1.0),
	comboArrowColor = Vector4:new(0.11, 0.11, 0.13, 1.0),
	comboArrowHoveredColor = Vector4:new(0.15, 0.15, 0.16, 1.0),
	-- Buttons
	buttonColor = Vector4:new(0.3, 0.3, 0.65, 1.0),
	buttonHoveredColor = Vector4:new(0.35, 0.35, 0.75, 1.0),
	buttonActiveColor = Vector4:new(0.2, 0.2, 0.5, 0.8),
	imageButtonColor = Vector4:new(0.18, 0.18, 0.18, 1.0),
	imageButtonDarkColor = Vector4:new(0.15, 0.15, 0.15, 1.0),
	imageButtonHoveredColor = Vector4:new(0.3, 0.3, 0.3, 1.0),
	imageButtonActiveColor = Vector4:new(0.1, 0.1, 0.1, 1.0),
	imageButtonTintColor = whiteColor,
	-- Sliders/Drags
	sliderColor = Vector4:new(0.09, 0.09, 0.13, 1.0),
	sliderHoveredColor = Vector4:new(0.09, 0.09, 0.13, 1.0),
	sliderActiveColor = Vector4:new(0.09, 0.09, 0.13, 1.0),
	dragColor = Vector4:new(0.2, 0.2, 0.2, 0.0),
	dragHoveredColor = Vector4:new(0.45, 0.45, 0.45, 1.0),
	dragActiveColor = Vector4:new(0.10, 0.10, 0.10, 1.0),
	-- Checkboxes
	checkboxBgColor = Vector4:new(0.28, 0.28, 0.29, 1.0),
	checkboxCheckColor = Vector4:new(0.45, 0.45, 0.9, 1.0),
	checkboxHoveredColor = Vector4:new(0.31, 0.31, 0.32, 1.0),
	checkboxActiveColor = Vector4:new(0.15, 0.15, 0.23, 1.0),
}