#include "Game.h"

void Paddle::Spawn(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = startWidth = width;
	this->height = startHeight = height;
}

void Paddle::Update(float deltaTime) {
	if (lastControlMessage) {
		y = lastControlMessage->paddle.position;
	}
}

NmControlMessage Paddle::MakeControlMessage() const {
	return NmControlMessagePaddle(game.input.GetMouseY() - height / 2);
}

void Paddle::Render() {
	SDL_Rect area = {(int)x, (int)y, width, height};

	SDL_SetRenderDrawColor(game.sdlRenderer, 255, 0, 0, 255);
	SDL_RenderFillRect(game.sdlRenderer, &area);
}