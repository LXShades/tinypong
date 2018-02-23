#include "Game.h"

#include <SDL.h>

Game game;

void Game::Run() {
	// Startup the game
	Init();

	// Spawn the paddles (todo: do this in GameStatePlay::Enter())
	paddles[0] = CreateObject<Paddle>();
	paddles[0]->Spawn(0, renderHeight / 2, 1, 10);

	paddles[1] = CreateObject<Paddle>();
	paddles[1]->Spawn(renderWidth - 1, renderHeight / 2, 1, 10);

	// Add a test ball
	Ball* test = CreateObject<Ball>();

	test->Spawn(Vec2(arenaWidth / 2, arenaHeight / 2), Vec2(50.0f, 0.0f));

	// Add the local player
	localPlayer = AddPlayer(0, NetAddr(0, 0, 0, 0, 0));

	// Try to connect to a local server
	bool connected = netgame.Connect();

	if (!connected) {
		// Try to host a server
		bool hosting = netgame.Host();
	}

	// Run the main loop
	isRunning = true;
	frameTime = GetTickCount();

	while (isRunning) {
		// Handle SDL events
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				Quit();
			}
		}

		// Update frame time
		timestamp lastTime = frameTime;
		frameTime = GetTickCount();

		deltaTime = (float)(frameTime - lastTime) / 1000.0f;

		// Only update if a notable amount of time has passed. Such as a millisecond. That's notable.
		if (deltaTime > 0.0f) {
			// Update the game
			Update();

			// Render the game
			Render();
		}
	}

	// Release the game!
	Shutdown();
}

void Game::Init() {
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO);

	// Get screen info
	SDL_Rect screenBounds;
	SDL_GetDisplayBounds(0, &screenBounds);

	// Create the game window
	const char* wndTitle = "TinyPongSplosion";
	const int wndWidth = 640, wndHeight = 480;
	const int screenCentreX = screenBounds.w / 2, screenCentreY = screenBounds.h / 2;

	sdlWindow = SDL_CreateWindow(wndTitle, screenCentreX - (wndWidth / 2), screenCentreY - (wndHeight / 2), wndWidth, wndHeight, SDL_WINDOW_RESIZABLE);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	sdlRendererTarget = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, renderWidth, renderHeight);

	// Init network
	netgame.Init();
}

void Game::Shutdown() {
	netgame.Shutdown();

	SDL_Quit();
}

void Game::Update() {
	// Update input
	input.Update();

	// Update network
	netgame.Update();

	// Update players
	for (Player* player : players) {
		player->Update();
	}

	// Update objects
	for (Object* object : objects) {
		object->Update(deltaTime);
	}

	// Update collisions
	for (int objA = 0; objA < objects.size(); ++objA) {
		for (int objB = objA + 1; objB < objects.size(); ++objB) {
			if (objects[objA]->IsColliding(*objects[objB])) {
				objects[objA]->OnCollision(*objects[objB]);
				objects[objB]->OnCollision(*objects[objA], true);
			}
		}
	}
}

void Game::Render() {
	// Initialise the renderer for drawing to the 60x60 texture
	SDL_SetRenderTarget(sdlRenderer, sdlRendererTarget);

	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);

	// Render the arena edges
	SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(sdlRenderer, 0, 0, arenaWidth, 0);
	SDL_RenderDrawLine(sdlRenderer, 0, arenaHeight-1, arenaWidth, arenaHeight-1);

	// Render the paddles
	for (Object* object : objects) {
		object->Render();
	}

	// Prepare to render
	SDL_SetRenderTarget(sdlRenderer, nullptr);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);

	// Render in a 1:1 centre box
	SDL_Rect renderPosition{0, 0, };
	SDL_GetWindowSize(sdlWindow, &renderPosition.w, &renderPosition.h);

	if (renderPosition.w > renderPosition.h) {
		renderPosition.x = (renderPosition.w - renderPosition.h) / 2;
		renderPosition.w = renderPosition.h;
	} else if (renderPosition.h > renderPosition.w) {
		renderPosition.y = (renderPosition.h - renderPosition.w) / 2;
		renderPosition.h = renderPosition.w;
	}

	viewportX = renderPosition.x;
	viewportY = renderPosition.y;
	viewportWidth = renderPosition.w;
	viewportHeight = renderPosition.h;

	// Bloop
	SDL_RenderCopyEx(sdlRenderer, sdlRendererTarget, nullptr, &renderPosition, 0.0f, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(sdlRenderer);
}

void Game::Quit() {
	isRunning = false;

	// Todo: Quit the network session
}

Player* Game::AddPlayer(int id, const NetAddr& netAddr) {
	if (id == -1) {
		// Find a new player/dead one
		for (id = 0; id < players.size(); ++id) {
			if (!players[id]->IsAlive())
				break;
		}

		// Make a new one if necessary
		if (id == players.size()) {
			players.push_back(new Player(id, netAddr));
		}

		// Live!
		players[id]->SetAlive(true);
		players[id]->SetAddr(netAddr);
		return players[id];
	} else {
		while (id >= players.size()) {
			players.push_back(new Player(id, NetAddr(0, 0, 0, 0, 0)));
		}

		players[id]->SetAlive(true);
		players[id]->SetAddr(netAddr);
		return players[id];
	}
}

void Game::SetLocalPlayerId(int id) {
	localPlayer = AddPlayer(id, NetAddr(0, 0, 0, 0, 0));

	localPlayer->SetPawn(objects[id]);
}