#pragma once
#include <map>
#include <string>
#include "SDL.h"
#include <memory>
#include <vector>


namespace FlatEngine 
{
	enum FKeyboardKeys {
		Keyboard_up = SDLK_UP,
		Keyboard_down = SDLK_DOWN,
		Keyboard_left = SDLK_LEFT,
		Keyboard_right = SDLK_RIGHT,
		Keyboard_space = SDLK_SPACE,
		Keyboard_leftCtrl = SDLK_LCTRL,
		Keyboard_leftShift = SDLK_LSHIFT,
		Keyboard_leftAlt = SDLK_LALT,
		Keyboard_rightCtrl = SDLK_RCTRL,
		Keyboard_rightShift = SDLK_RSHIFT,
		Keyboard_rightAlt = SDLK_RALT,
		Keyboard_capsLock = SDLK_CAPSLOCK,
		Keyboard_numLock = SDLK_NUMLOCKCLEAR,
		Keyboard_backspace = SDLK_BACKSPACE,
		Keyboard_escape = SDLK_ESCAPE,
		Keyboard_tab = SDLK_TAB,

		Keyboard_printScreen = SDLK_PRINTSCREEN,
		Keyboard_insert = SDLK_INSERT,
		Keyboard_home = SDLK_HOME,
		Keyboard_pageUp = SDLK_PAGEUP,
		Keyboard_pageDown = SDLK_PAGEDOWN,
		Keyboard_delete = SDLK_DELETE,

		Keyboard_backSlash = SDLK_BACKSLASH,
		Keyboard_forwardSlash = SDLK_SLASH,
		Keyboard_semicolin = SDLK_SEMICOLON,
		Keyboard_apostrophe = SDL_SCANCODE_APOSTROPHE,
		Keyboard_rightBracket = SDLK_RIGHTBRACKET,
		Keyboard_leftBracket = SDLK_LEFTBRACKET,
		Keyboard_less = SDLK_LESS,
		Keyboard_greater = SDLK_GREATER,
		Keyboard_graveAccent = SDL_SCANCODE_GRAVE,
		Keyboard_asterisk = SDLK_ASTERISK,
		Keyboard_period = SDLK_PERIOD,
		Keyboard_enter = SDLK_RETURN,
		Keyboard_0 = SDLK_0,
		Keyboard_1 = SDLK_1,
		Keyboard_2 = SDLK_2,
		Keyboard_3 = SDLK_3,
		Keyboard_4 = SDLK_4,
		Keyboard_5 = SDLK_5,
		Keyboard_6 = SDLK_6,
		Keyboard_7 = SDLK_7,
		Keyboard_8 = SDLK_8,
		Keyboard_9 = SDLK_9,
		Keyboard_minus = SDLK_MINUS,
		Keyboard_plus = SDLK_PLUS,

		Keyboard_f1 = SDLK_F1,
		Keyboard_f2 = SDLK_F2,
		Keyboard_f3 = SDLK_F3,
		Keyboard_f4 = SDLK_F4,
		Keyboard_f5 = SDLK_F5,
		Keyboard_f6 = SDLK_F6,
		Keyboard_f7 = SDLK_F7,
		Keyboard_f8 = SDLK_F8,
		Keyboard_f9 = SDLK_F9,
		Keyboard_f10 = SDLK_F10,
		Keyboard_f11 = SDLK_F11,
		Keyboard_f12 = SDLK_F12,

		Keyboard_a = SDLK_a,
		Keyboard_b = SDLK_b,
		Keyboard_c = SDLK_c,
		Keyboard_d = SDLK_d,
		Keyboard_e = SDLK_e,
		Keyboard_f = SDLK_f,
		Keyboard_g = SDLK_g,
		Keyboard_h = SDLK_h,
		Keyboard_i = SDLK_i,
		Keyboard_j = SDLK_j,
		Keyboard_k = SDLK_k,
		Keyboard_l = SDLK_l,
		Keyboard_m = SDLK_m,
		Keyboard_n = SDLK_n,
		Keyboard_o = SDLK_o,
		Keyboard_p = SDLK_p,
		Keyboard_q = SDLK_q,
		Keyboard_r = SDLK_r,
		Keyboard_s = SDLK_s,
		Keyboard_t = SDLK_t,
		Keyboard_u = SDLK_u,
		Keyboard_v = SDLK_v,
		Keyboard_w = SDLK_w,
		Keyboard_x = SDLK_x,
		Keyboard_y = SDLK_y,
		Keyboard_z = SDLK_z,
	};
	enum XInputButtons {
		A,
		B,
		X,
		Y,
		LB,
		RB,
		ScreenShot,
		Start,
		LS,
		RS,
		Home,
		Tray,
	};
	enum XInputHats {
		Hat_Up = 1,
		Hat_Down = 4,
		Hat_Left = 8,
		Hat_Right = 2,

	};
	enum XInputAxis {
		LeftXAxis,
		LeftYAxis,
		RightXAxis,
		RightYAxis,
		LT,
		RT
	};

	const std::map<long, std::string> F_MappedKeyboardCodes =
	{
		{ Keyboard_up, "Keyboard_up" },
		{ Keyboard_down, "Keyboard_down" },
		{ Keyboard_left, "Keyboard_left" },
		{ Keyboard_right, "Keyboard_right" },
		{ Keyboard_space, "Keyboard_space" },
		{ Keyboard_leftCtrl, "Keyboard_leftCtrl" },
		{ Keyboard_leftShift, "Keyboard_leftShift" },
		{ Keyboard_leftAlt, "Keyboard_leftAlt" },
		{ Keyboard_rightCtrl, "Keyboard_rightCtrl" },
		{ Keyboard_rightShift, "Keyboard_rightShift" },
		{ Keyboard_rightAlt, "Keyboard_rightAlt" },
		{ Keyboard_capsLock, "Keyboard_capsLock" },
		{ Keyboard_numLock, "Keyboard_numLock" },
		{ Keyboard_backspace, "Keyboard_backspace" },
		{ Keyboard_escape, "Keyboard_escape" },
		{ Keyboard_tab, "Keyboard_tab" },

		{ Keyboard_printScreen, "Keyboard_printScreen" },
		{ Keyboard_insert, "Keyboard_insert" },
		{ Keyboard_home, "Keyboard_home" },
		{ Keyboard_pageUp, "Keyboard_pageUp" },
		{ Keyboard_pageDown, "Keyboard_pageDown" },
		{ Keyboard_delete, "Keyboard_delete" },

		{ Keyboard_backSlash, "Keyboard_backSlash" },
		{ Keyboard_forwardSlash, "Keyboard_forwardSlash" },
		{ Keyboard_semicolin, "Keyboard_semicolin" },
		{ Keyboard_apostrophe, "Keyboard_apostrophe" },
		{ Keyboard_rightBracket, "Keyboard_rightBracket" },
		{ Keyboard_leftBracket, "Keyboard_leftBracket" },
		{ Keyboard_less, "Keyboard_less" },
		{ Keyboard_greater, "Keyboard_greater" },
		{ Keyboard_graveAccent, "Keyboard_graveAccent" },
		{ Keyboard_asterisk, "Keyboard_asterisk" },
		{ Keyboard_period, "Keyboard_period" },
		{ Keyboard_enter, "Keyboard_enter" },
		{ Keyboard_0, "Keyboard_0" },
		{ Keyboard_1, "Keyboard_1" },
		{ Keyboard_2, "Keyboard_2" },
		{ Keyboard_3, "Keyboard_3" },
		{ Keyboard_4, "Keyboard_4" },
		{ Keyboard_5, "Keyboard_5" },
		{ Keyboard_6, "Keyboard_6" },
		{ Keyboard_7, "Keyboard_7" },
		{ Keyboard_8, "Keyboard_8" },
		{ Keyboard_9, "Keyboard_9" },
		{ Keyboard_minus, "Keyboard_minus" },
		{ Keyboard_plus, "Keyboard_plus" },

		{ Keyboard_f1, "Keyboard_f1" },
		{ Keyboard_f2, "Keyboard_f2" },
		{ Keyboard_f3, "Keyboard_f3" },
		{ Keyboard_f4, "Keyboard_f4" },
		{ Keyboard_f5, "Keyboard_f5" },
		{ Keyboard_f6, "Keyboard_f6" },
		{ Keyboard_f7, "Keyboard_f7" },
		{ Keyboard_f8, "Keyboard_f8" },
		{ Keyboard_f9, "Keyboard_f9" },
		{ Keyboard_f10, "Keyboard_f10" },
		{ Keyboard_f11, "Keyboard_f11" },
		{ Keyboard_f12, "Keyboard_f12" },

		{ Keyboard_a, "Keyboard_a" },
		{ Keyboard_b, "Keyboard_b" },
		{ Keyboard_c, "Keyboard_c" },
		{ Keyboard_d, "Keyboard_d" },
		{ Keyboard_e, "Keyboard_e" },
		{ Keyboard_f, "Keyboard_f" },
		{ Keyboard_g, "Keyboard_g" },
		{ Keyboard_h, "Keyboard_h" },
		{ Keyboard_i, "Keyboard_i" },
		{ Keyboard_j, "Keyboard_j" },
		{ Keyboard_k, "Keyboard_k" },
		{ Keyboard_l, "Keyboard_l" },
		{ Keyboard_m, "Keyboard_m" },
		{ Keyboard_n, "Keyboard_n" },
		{ Keyboard_o, "Keyboard_o" },
		{ Keyboard_p, "Keyboard_p" },
		{ Keyboard_q, "Keyboard_q" },
		{ Keyboard_r, "Keyboard_r" },
		{ Keyboard_s, "Keyboard_s" },
		{ Keyboard_t, "Keyboard_t" },
		{ Keyboard_u, "Keyboard_u" },
		{ Keyboard_v, "Keyboard_v" },
		{ Keyboard_w, "Keyboard_w" },
		{ Keyboard_x, "Keyboard_x" },
		{ Keyboard_y, "Keyboard_y" },
		{ Keyboard_z, "Keyboard_z" },
	};
	const std::map<long, std::string> F_MappedXInputButtonCodes =
	{
		{ A, "XInput_A" },
		{ B, "XInput_B" },
		{ X, "XInput_X" },
		{ Y, "XInput_Y" },
		{ LB, "XInput_LB" },
		{ RB, "XInput_RB" },
		{ ScreenShot, "XInput_ScreenShot" },
		{ Start, "XInput_Start" },
		{ LS, "XInput_LS" },
		{ RS, "XInput_RS" },
		{ Home, "XInput_Home" },
		{ Tray, "XInput_Tray" },
	};
	const std::map<long, std::string> F_MappedXInputDPadCodes =
	{
		{ Hat_Up, "XInput_DPadUp" },
		{ Hat_Down, "XInput_DPadDown" },
		{ Hat_Left, "XInput_DPadLeft" },
		{ Hat_Right, "XInput_DPadRight" }
	};
	const std::map<long, std::string> F_MappedXInputAnalogCodes =
	{
		{ LeftXAxis, "XInput_LeftJoystickX" },
		{ LeftYAxis, "XInput_LeftJoystickY" },
		{ RightXAxis, "XInput_RightJoystickX" },
		{ RightYAxis, "XInput_RightJoystickY" },
		{ LT, "XInput_LT" },
		{ RT, "XInput_RT" }
	};

	struct InputMapping {
		SDL_Event event = SDL_Event();
		std::string keyCode = "";
		std::string actionName = "";
		bool b_fired = false;
		std::string GetKeyCode() { return keyCode; };
		std::string GetActionName() { return actionName; };
	};

	class MappingContext
	{
	public:
		MappingContext();
		~MappingContext();
		std::string GetData();

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		void AddKeyBinding(std::string keyBinding, std::string actionName);		
		bool FireEvent(std::string actionName, SDL_Event event);
		void UnFireEvent(std::string actionName);
		bool Fired(std::string actionName);
		void ClearInputActionEvent(std::string keyBinding);
		SDL_Event GetInputAction(std::string actionName);
		bool ActionPressed(std::string actionName);		
		std::map<std::string, std::shared_ptr<InputMapping>> GetInputActions();
		std::vector<std::shared_ptr<InputMapping>> GetInputMappingsLua();
		void SetWaitingForRemap(bool b_waiting);
		bool WaitingForRemap();
		void SetActionToRemap(std::string actionToRemap);
		std::string GetActionToRemap();
		void SetRemapStartTime(Uint32 startTime);
		bool RemapTimedOut(Uint32 currentTime);
		void SetRemapTimeoutTime(Uint32 timeoutTime);
		Uint32 GetRemapTimeoutTime();

	private:
		std::string m_name;
		std::string m_path;
		std::map<std::string, std::shared_ptr<InputMapping>> m_inputsByBinding;
		// Eventually: map<string, vector<shared_ptr<InputMapping>> so that the same action can be bound to more than one input, ie. Controller/Keyboard can both be used for the same actions
		std::map<std::string, std::shared_ptr<InputMapping>> m_inputsByAction;
		Uint32 m_remapTimeoutTime;
		Uint32 m_remapStartTime;
		std::string m_actionToRemap;
		bool m_b_waitingForRemap;
	};
}