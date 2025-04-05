#pragma once

#include <Windows.h>

#include "MinHook.h"

#include "HooksException.hpp"

namespace Hooks {
	using tWndProc = WNDPROC;
	using tWglSwapBuffers = BOOL(WINAPI*)(HDC);

	inline HWND windowHandle = nullptr;

	namespace Original {
		inline tWndProc WndProc = nullptr;
		inline tWglSwapBuffers wglSwapBuffers = nullptr;
	}

	namespace Detour {
		extern LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
		extern BOOL WINAPI wglSwapBuffers(HDC);
	}

	void init();

	void destroy();
}
