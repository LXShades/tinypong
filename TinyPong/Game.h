#pragma once
#include <SDL.h>

#include <vector>

#include "Paddle.h"
#include "Ball.h"
#include "Input.h"
#include "Netgame.h"
#include "Player.h"

typedef unsigned __int32 timestamp;

class Game {
public:
	// Call Run to run the game! Yay!!
	void Run();

	void Quit();

public:
	// Constants
	// Screen resolution in pixels (note: not actual window width/height)
	static const int renderWidth = 60;
	static const int renderHeight = 60;

	// Size of the areana, should stay the same as render sizes for now
	static const int arenaWidth = 60;
	static const int arenaHeight = 60;

	int viewportX, viewportY;
	int viewportWidth, viewportHeight;

	// Maximum number of players
	static const int maxNumPlayers = 100;

public:
	// Game components, except read-only ones, are public for this project
	// Burn it with fire.

	// SDL components
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer; // Low-resolution renderer
	SDL_Renderer* sdlPresenterRenderer; // Window renderer...?
	SDL_Texture* sdlRendererTarget; // 60x60 render target texture

	// Input
	Input input;

	// Network
	Netgame netgame;

	// Paddle pointers
	Paddle* paddles[2];

	// Objects
	std::vector<Object*> objects;

	// Players
	std::vector<Player*> players;
	Player* localPlayer = nullptr;

	// Timing etc
	timestamp frameTime;
	float deltaTime;

public:
	template<typename ObjType>
	inline ObjType* CreateObject();

	inline Player* GetLocalPlayer();
	void SetLocalPlayerId(int id);

	Player* AddPlayer(int playerId, const NetAddr& address);

	//void SetGameState(const GameState&& gameState);
private:
	void Init();
	void Shutdown();

	void Update();
	void Render();

	bool isRunning = false;


};

// ZOMG GLOBAL VARIABLE!!! BURN IT WITH FIRE
extern Game game;

template<typename ObjType>
inline ObjType* Game::CreateObject() {
	ObjType* object = new ObjType();

	object->SetId(objects.size());
	objects.push_back(object);
	return object;
}

inline Player* Game::GetLocalPlayer() {
	return localPlayer;
}
