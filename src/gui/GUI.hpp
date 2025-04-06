#pragma once
#include "pch.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

#include "Hooks.hpp"

namespace GUI {
	HDC deviceContext = nullptr;
	HGLRC oldContext = nullptr;
	HGLRC newContext = nullptr;
	ImGuiContext* imguiContext = nullptr;

	bool toggled = false;

	void destroy();
}
