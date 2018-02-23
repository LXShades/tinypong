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
		targetY = lastControlMessage->paddle.targetPosition;
	}

	// Move torwards targetY
	const float acceleration = 800.0f;
	const float maxSpeed = 200.0f;
	const float friction = 1000.0f;
	float projectedTargetY = y + velocity * 0.1f; // position in 0.8 seconds following

	if (abs(velocity) < maxSpeed) {
		if (targetY < projectedTargetY) {
			velocity -= acceleration * deltaTime;
		} else {
			velocity += acceleration * deltaTime;
		}
	}

	// Friction
	if (targetY < projectedTargetY && velocity > 0) {
		velocity -= friction * deltaTime;
	} else if (targetY > projectedTargetY && velocity < 0) {
		velocity += friction * deltaTime;
	}

	// Move
	y += velocity * deltaTime;
	
	// Stop at top and bottom
	if (y < 1) {
		velocity = 0.0f;
		y = 1;
	} else if (y + height > game.arenaHeight - 1) {
		velocity = 0.0f;
		y = game.arenaHeight - height - 1;
	}
}

NmControlMessage Paddle::MakeControlMessage() const {
	return NmControlMessagePaddle(y, game.input.GetMouseY() - height / 2);
}

void Paddle::Render() {
	SDL_Rect area = {(int)x, (int)y, width, height};

	SDL_SetRenderDrawColor(game.sdlRenderer, 255, 0, 0, 255);
	SDL_RenderFillRect(game.sdlRenderer, &area);
}