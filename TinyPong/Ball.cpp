#include "Game.h"

#include <SDL.h>

void Ball::Render() {
	SDL_Rect area{(int)x, (int)y, 1, 1};

	// Draw 1x1 ball for now
	SDL_SetRenderDrawColor(game.sdlRenderer, 255, 127, 0, 255);
	SDL_RenderFillRect(game.sdlRenderer, &area);
}

void Ball::Spawn(const Vec2& position, const Vec2& velocity_) {
	xy = position;
	velocity = velocity_;
}

NmControlMessage Ball::MakeControlMessage() const {
	// Do a dash if possible!
	if (game.input.IsLeftButtonBooped()) {
		Vec2 destination = Vec2(game.input.GetMouseX(), game.input.GetMouseY());
		Vec2 velocity;
		
		velocity = (destination - xy);
		velocity.ScaleTo(60.0f);

		return NmControlMessageBall(xy, velocity);
	}

	return NmControlMessageBall(xy, velocity);
}

void Ball::OnCollision(const Object& other, bool isSecondary) {
	if (other.GetType() == ObjectType::Ball) {
		if (isSecondary) {
			Vec2 backup = velocity;
			velocity = ((Ball&)other).velocity;
			((Ball&)other).velocity = backup;
		}
	} else if (other.GetType() == ObjectType::Paddle) {
		velocity.x = -velocity.x;

		if (velocity.x > 0.0f) {
			x = other.GetX() + 1;
		} else {
			x = other.GetX() - 1;
		}
	}
}

void Ball::Update(float deltaTime) {
	if (lastControlMessage) {
		xy = lastControlMessage->ball.xy;
		velocity = lastControlMessage->ball.velocity;
	}

	xy += velocity * deltaTime;

	if (x < 0.0f || x >= (float)Game::arenaWidth) {
		velocity.x = -velocity.x;

		OnOutOfBounds();
	}

	if (y < 0.0f || y >= (float)Game::arenaHeight) {
		velocity.y = -velocity.y;
	}
}

void Ball::OnOutOfBounds() {
	xy = Vec2(game.arenaWidth / 2, game.arenaHeight / 2);
}
