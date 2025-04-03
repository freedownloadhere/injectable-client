#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <iostream>

#include "Java.hpp"
#include "Killaura.hpp"

void MainButForReal(HINSTANCE instance) {
	FILE* in = nullptr, *out = nullptr, *err = nullptr;
	AllocConsole();
	SetConsoleOutputCP(CP_UTF8);
	freopen_s(&in, "CONIN$", "r", stdin);
	freopen_s(&out, "CONOUT$", "w", stdout);
	freopen_s(&err, "CONOUT$", "w", stderr);

	Java::init();

	std::cout << "Starting...\n";
	while (!GetAsyncKeyState(VK_OEM_3)) {
		Killaura::update();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

finish:
	if(in) fclose(in);
	if(out) fclose(out);
	if(err) fclose(err);
	FreeConsole();
	FreeLibrary(instance);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	std::thread mainThread(MainButForReal, instance);
	mainThread.detach();

	return TRUE;
}