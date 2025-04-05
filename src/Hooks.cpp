#include "Hooks.hpp"

#include <iostream>
#include <assert.h>
#include <thread>

void Hooks::init() {
	windowHandle = FindWindowA(nullptr, "Minecraft 1.8.9");
	if (windowHandle == nullptr)
		throw HooksException("The window handle is null");

	Original::WndProc = (tWndProc)SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)Detour::WndProc);
	if (Original::WndProc == nullptr)
		throw HooksException("Failed to get the original WndProc function");

	HMODULE moduleHandle = GetModuleHandle(L"opengl32.dll");
	if (moduleHandle == nullptr)
		throw HooksException("The OpenGL module handle is null");

	Original::wglSwapBuffers = (tWglSwapBuffers)GetProcAddress(moduleHandle, "wglSwapBuffers");
	if (Original::wglSwapBuffers == nullptr)
		throw HooksException("Failed to get the original wglSwapBuffers function");

	MH_STATUS initStatus = MH_Initialize();
	if (initStatus != MH_OK)
		throw HooksException("Failed to initialize MinHook");

	MH_STATUS createHookStatus = MH_CreateHook(Original::wglSwapBuffers, &Detour::wglSwapBuffers, (void**)&Original::wglSwapBuffers);
	if (createHookStatus != MH_OK)
		throw HooksException("Failed to create WglSwapBuffers hook");

	MH_STATUS enableHookStatus = MH_EnableHook(MH_ALL_HOOKS);
	if (enableHookStatus != MH_OK)
		throw HooksException("Failed to enable all hooks");
}

void Hooks::destroy() {
	if (Original::WndProc != nullptr)
		SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)Original::WndProc);

	MH_DisableHook(MH_ALL_HOOKS);

	MH_RemoveHook(MH_ALL_HOOKS);
}
