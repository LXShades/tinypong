#pragma once

#include "Object.h"

class Paddle : public Object {
public:
	void Spawn(int x, int y, int defaultWidth, int defaultHeight);
	void Unspawn();

	void Update(float deltaTime) override;
	void Render() override;
	
	NmControlMessage MakeControlMessage() const override;

	ObjectType GetType() const override {
		return ObjectType::Paddle;
	}
private:
	int startWidth, startHeight;
};