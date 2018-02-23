#include "Game.h"

#include <SDL.h>

void Input::Update() {
	int mScreenX, mScreenY;
	unsigned int mouseButtonState = SDL_GetMouseState(&mScreenX, &mScreenY);

	int wndW = 0, wndH = 0;

	SDL_GetWindowSize(game.sdlWindow, &wndW, &wndH);

	if (wndW && wndH /* don't divide by 0 */) {
		mouseX = ((float)mScreenX - game.viewportX) * game.renderWidth / (game.viewportWidth);
		mouseY = ((float)mScreenY - game.viewportY) * game.renderHeight / (game.viewportHeight);
	}

	// Update mouse buttons
	if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		lBoop = !lDown;
		lDown = true;
	} else {
		lDown = lBoop = false;
	}
}

float Input::GetMouseX() const {
	return mouseX;
}

float Input::GetMouseY() const {
	return mouseY;
}

bool Input::IsLeftButtonBooped() const {
	return lBoop;
}
