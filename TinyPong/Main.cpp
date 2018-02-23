#include "Game.h"

#pragma comment(lib, "sdl2.lib")
#pragma comment(lib, "sdl2main.lib")

int main(int, char**) {
	AllocConsole();

	FILE* butSecurityTho;
	freopen_s(&butSecurityTho, "CONIN$", "r", stdin); 
	freopen_s(&butSecurityTho, "CONOUT$", "w", stdout); 
	freopen_s(&butSecurityTho, "CONOUT$", "w", stderr); 

	game.Run();

	return 0;
}