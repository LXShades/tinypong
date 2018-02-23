#include "Object.h"
#include "Netgame.h"

void Object::Render() {
	return;
}

NmControlMessage Object::MakeControlMessage() const {
	return NmControlMessagePaddle(0.0f, 0.0f);
}

void Object::SubmitControlMessage(const NmControlMessage& message) {
	delete lastControlMessage;
	lastControlMessage = new NmControlMessage(message);
}

bool Object::IsColliding(const Object& other) {
	if (x + width < other.x || y + height < other.y || x > other.x + other.width || y > other.y + other.height) {
		return false;
	} else {
		return true;
	}
}

void Object::OnCollision(const Object& other, bool isSecondary) {
	return;
}

void Object::Update(float deltaTime) {
	return;
}