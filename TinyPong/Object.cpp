#include "Object.h"
#include "Netgame.h"

void Object::Render() {
	return;
}

NmControlMessage Object::MakeControlMessage() const {
	return NmControlMessagePaddle(0.0f);
}

void Object::SubmitControlMessage(const NmControlMessage& message) {
	delete lastControlMessage;
	lastControlMessage = new NmControlMessage(message);
}

void Object::Update(float deltaTime) {
	return;
}