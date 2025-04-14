// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "pch.hpp"

#include "gui/Hooks.hpp"
#include "java/Java.hpp"
#include "modules/Killaura.hpp"

void MainButForReal(HINSTANCE instance) {
	FILE* in = nullptr, *out = nullptr, *err = nullptr;
	AllocConsole();
	SetConsoleOutputCP(CP_UTF8);
	freopen_s(&in, "CONIN$", "r", stdin);
	freopen_s(&out, "CONOUT$", "w", stdout);
	freopen_s(&err, "CONOUT$", "w", stderr);

	try {
		Java::getInstance().initialize();
		std::cout << "Initialized Java\n";
		Hooks::init();
		std::cout << "Initialized Hooks\n";

		Killaura ka;

		std::cout << "Starting...\n";
		while (!GetAsyncKeyState(VK_OEM_3)) {
			ka.update();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	catch(const JavaException& e) {
		std::cerr << "Java Error: " << e.what() << '\n';
		std::cerr << "(Press enter to finish..)\n";
		std::cin.get();
		goto finish;
	}
	catch (const HooksException& e) {
		std::cerr << "Hooks Error: " << e.what() << '\n';
		std::cerr << "(Press enter to finish..)\n";
		std::cin.get();
		goto finish;
	}

finish:
	Java::getInstance().cleanup();
	Hooks::destroy();
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