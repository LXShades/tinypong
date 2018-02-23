#pragma once
#include "Object.h"

class Ball : public Object {
public:
	void Update(float deltaTime) override;
	void Render() override;

	void Spawn(const Vec2& position, const Vec2& velocity = Vec2(0.0f, 0.0f));

	NmControlMessage MakeControlMessage() const;

	ObjectType GetType() const override {
		return ObjectType::Ball;
	}

private:
	Vec2 velocity;

	float lastDashTime = 0.0f;
	float dashingCounter = 0.0f;
};