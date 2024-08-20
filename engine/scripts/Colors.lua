--------------------------
-- Global Engine Colors --
--------------------------

F_Colors = {
	transparent = Vector4:new(0.0, 0.0, 0.0, 0.0),
	white = Vector4:new(1.0, 1.0, 1.0, 1.0),
	----------------------
	-- Components --------
	----------------------
	componentBorder = Vector4:new(0.2, 0.2, 0.2, 1.0),
	componentBg = Vector4:new(0.16, 0.16, 0.17, 1.0),
	-- Colliders
	colliderActive = Vector4:new(0.19, 0.9, 0.2, 0.5),
	colliderInactive = Vector4:new(0.9, 0.2, 0.2, 0.5),
	colliderColliding = Vector4:new(0.76, 0.42, 0.0, 0.5),
	-- Button Components
	buttonComponentActive = Vector4:new(0.19, 0.9, 0.2, 0.5),
	buttonComponentInctive = Vector4:new(0.9, 0.2, 0.2, 0.5),
	-- Canvas
	canvasBox = Vector4:new(0.76, 0.42, 0.0, 0.5),
	-- Camera
	cameraBox = Vector4:new(1.0, 0.11, 0.11, 0.27),
	----------------------
	-- Windows -----------
	----------------------
	windowBg = Vector4:new(0.08, 0.08, 0.10, 1.0),
	outerWindow = Vector4:new(0.13, 0.13, 0.15, 1.0),
	innerWindow = Vector4:new(0.1, 0.1, 0.12, 1.0),
	singleItem = Vector4:new(0.16, 0.16, 0.17, 1.0),
	singleItemDark = Vector4:new(0.09, 0.09, 0.13, 1.0),
	windowTitleBg = Vector4:new(0.25, 0.25, 0.25, 1.0),
	-- Project Hub
	selectProjectBg = Vector4:new(0.08, 0.08, 0.10, 1.0),
	projectHubButton = Vector4:new(0.3, 0.3, 0.55, 1.0),
	projectHubButtonActive = Vector4:new(0.2, 0.2, 0.45, 1.0),
	projectHubButtonHovered = Vector4:new(0.4, 0.4, 0.65, 1.0),
	projectSelectionTable = Vector4:new(0.16, 0.16, 0.17, 1.0),
	projectSelectionTableOutline = Vector4:new(0.16, 0.16, 0.17, 1.0),
	-- Log
	logText = Vector4:new(0.75, 0.75, 0.75, 1.0),
	logBg = Vector4:new(0.2, 0.2, 0.22, 1.0),
	logOutline = Vector4:new(0.25, 0.25, 0.27, 1.0),
	-- File Explorer
	explorerFilesPanelBg = Vector4:new(0.2, 0.2, 0.22, 1.0),
	panelTitleBg =  Vector4:new(0.14, 0.14, 0.14, 1.0),
	filePanelTopBar = Vector4:new(0.3, 0.3, 0.38, 1.0),
	filesPanelOutline = Vector4:new(0.25, 0.25, 0.27, 1.0),
	-- Docking
	dockingPreview = Vector4:new(0.3, 0.3, 0.65, 1.0),
	dockingPreviewEmpty = Vector4:new(0.3, 0.3, 0.65, 1.0),
	-- ImGui Key Colors
	frameBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgActive = Vector4:new(0.2, 0.2, 0.2, 1.0),
	frameBgHovered = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- tabs
	tab = Vector4:new(0.15, 0.15, 0.2, 1.0),
	tabActive = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabHovered = Vector4:new(0.4, 0.4, 0.42, 1.0),
	tabUnfocused = Vector4:new(0.1, 0.1, 0.1, 1.0),
	tabUnfocusedActive = Vector4:new(0.3, 0.3, 0.31, 1.0),
	-- titles
	titleBg = Vector4:new(0.0, 0.0, 0.0, 1.0),
	titleBgActive = Vector4:new(0.2, 0.2, 0.25, 1.0),
	titleBgCollapsed = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Text
	textSelectedBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	-- Resizers
	resizeGrip = Vector4:new(0.3, 0.3, 0.65, 1.0),
	resizeGripHovered = Vector4:new(0.35, 0.35, 0.75, 1.0),
	resizeGripActive = Vector4:new(0.2, 0.2, 0.5, 0.8),
	-- Misc (not sure what they're or)
	popupBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowHighlight = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navHighlight = Vector4:new(0.2, 0.2, 0.2, 1.0),
	navWindowDimBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	modalWindowDimBg = Vector4:new(0.2, 0.2, 0.2, 1.0),
	---------------------
	--- Custom Colors ---
	---------------------
	-- Inputs
	input = Vector4:new(0.3, 0.3, 0.32, 1.0),
	-- Tables
	noEditTableText = Vector4:new(0.75, 0.75, 0.81, 1.0),
	noEditTableRowFieldBg = Vector4:new(0.1, 0.1, 0.5, 0.2),
	noEditTableRowValueBg = Vector4:new(0.1, 0.1, 0.5, 0.2),
	tableCellLight = Vector4:new(0.19, 0.19, 0.21, 1.0),
	tableCellDark = Vector4:new(0.24, 0.24, 0.27, 1.0),
	-- Trees
	treeSelectable = Vector4:new(0.15, 0.15, 0.15, 1.0),
	treeSelectableHovered = Vector4:new(0.3, 0.35, 0.65, 1.0),
	treeSelectableActive = Vector4:new(0.2, 0.25, 0.45, 1.0),
	treeSelectableSelected = Vector4:new(0.5, 0.5, 0.8, 1.0),
	hierarchyChildObject = Vector4:new(0.3, 0.3, 0.7, 0.2),
	-- Combos
	comboBg = Vector4:new(0.19, 0.19, 0.21, 1.0),
	comboHovered = Vector4:new(0.25, 0.25, 0.26, 1.0),
	comboSelectable = Vector4:new(0.34, .34, .4, 1.0),
	comboSelected = Vector4:new(0.45, 0.45, 0.50, 1.0),
	comboHighlighted = Vector4:new(0.25, 0.25, 0.28, 1.0),
	comboArrow = Vector4:new(0.11, 0.11, 0.13, 1.0),
	comboArrowHovered = Vector4:new(0.15, 0.15, 0.16, 1.0),
	-- Buttons
	button = Vector4:new(0.3, 0.3, 0.65, 1.0),
	buttonHovered = Vector4:new(0.35, 0.35, 0.75, 1.0),
	buttonActive = Vector4:new(0.2, 0.2, 0.5, 0.8),
	imageButton = Vector4:new(0.18, 0.18, 0.18, 1.0),
	imageButtonDark = Vector4:new(0.15, 0.15, 0.15, 1.0),
	imageButtonHovered = Vector4:new(0.3, 0.3, 0.3, 1.0),
	imageButtonActive = Vector4:new(0.1, 0.1, 0.1, 1.0),
	imageButtonTint = Vector4:new(1.0, 1.0, 1.0, 1.0),
	-- Sliders/Drags
	slider = Vector4:new(0.09, 0.09, 0.13, 1.0),
	sliderHovered = Vector4:new(0.09, 0.09, 0.13, 1.0),
	sliderActive = Vector4:new(0.09, 0.09, 0.13, 1.0),
	drag = Vector4:new(0.2, 0.2, 0.2, 0.0),
	dragHovered = Vector4:new(0.45, 0.45, 0.45, 1.0),
	dragActive = Vector4:new(0.10, 0.10, 0.10, 1.0),
	-- Checkboxes
	checkboxBg = Vector4:new(0.28, 0.28, 0.29, 1.0),
	checkboxCheck = Vector4:new(0.45, 0.45, 0.9, 1.0),
	checkboxHovered = Vector4:new(0.31, 0.31, 0.32, 1.0),
	checkboxActive = Vector4:new(0.15, 0.15, 0.23, 1.0),
}