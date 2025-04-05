#include "GUI.hpp"

#include <iostream>
#include <thread>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

static void guiInit() {
	GUI::oldContext = wglGetCurrentContext();

	GUI::imguiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Hooks::windowHandle);
	ImGui_ImplOpenGL3_Init();

	GUI::toggled = true;
}

static void guiDraw() {
	if (!GUI::toggled) return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Window Test");
	ImGui::Text("hello world");
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::destroy() {
	toggled = false;

	wglMakeCurrent(deviceContext, newContext);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(imguiContext);

	wglDeleteContext(newContext);
}

LRESULT WINAPI Hooks::Detour::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (GUI::toggled)
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

	return CallWindowProc(Hooks::Original::WndProc, hwnd, msg, wparam, lparam);
}

BOOL WINAPI Hooks::Detour::wglSwapBuffers(HDC hdc) {
	static bool openglInitialized = false;

	if (!openglInitialized) {
		GUI::deviceContext = hdc;
		GUI::oldContext = wglGetCurrentContext();

		if (GUI::newContext != nullptr)
			wglDeleteContext(GUI::newContext);
		GUI::newContext = wglCreateContext(hdc);

		guiInit();

		openglInitialized = true;
	}

	wglMakeCurrent(hdc, GUI::newContext);
	guiDraw();
	wglMakeCurrent(hdc, GUI::oldContext);

	return Hooks::Original::wglSwapBuffers(hdc);
}
