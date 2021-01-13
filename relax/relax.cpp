#include <iostream>
#include <vector>

#include "action.h"
#include "sigscanner.h"
#include "error.h"

void startRelax(std::vector<Action> actions, SignatureScanner &ss) {
	// "\x7E\x00\x8B\x76\x00\xDB\x05", "x?xx?xx" | 0x55 and 0x10 don't seem to change but I will leave this sig/mask here just in case.
	const char* sig = "\x7E\x55\x8B\x76\x10\xDB\x05";
	const char* mask = "xxxxxxx";
	auto actionIt = actions.begin();
	int time = -1;

	// need 32-bit addresses
	uint32_t timeSignature = (uint32_t)ss.FindPattern(sig, mask);
	uint32_t timeAddr = ss.ReadMemory<uint32_t>((char*)timeSignature + 0x7);

	std::cout << "Press [ENTER] to start relax" << std::endl;

	while (!GetAsyncKeyState(VK_RETURN)) {
		Sleep(100);
	}

	std::cout << "Running..." << std::endl;

	if (ss.procID) {
		while (actionIt != actions.end()) {
			time = ss.ReadMemory<int>((char*)timeAddr);
			if (time >= actionIt->getTime() + 4) { // 4 ms offset
				std::cout << actionIt->getTime() << "; key: " << actionIt->getKey() << " " << (!actionIt->getPressed() ? "up" : "down") << std::endl;
				INPUT input = actionIt->getInput();
				SendInput(1, &input, sizeof INPUT);
				actionIt++;
			}
		}
	}

	std::cout << "Done." << std::endl;
}

int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			throw Error(0x539, "no beatmap supplied");
		}

		SignatureScanner ss;
		ss.GetProcId(L"osu!.exe");
		ss.GetModule(L"osu!.exe");

		std::vector<Action> actions = parseMap(argv[1]);

		startRelax(actions, ss);
	}
	catch (Error error) {
		std::cout << error.what() << std::endl;
		return error.code();
	}

	return 0;
}