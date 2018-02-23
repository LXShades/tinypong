#include "Game.h"

void Player::Update() {
	if (pawn) {
		if (this == game.GetLocalPlayer()) {
			// We make our own controls!
			NetMessage controlMessage;

			controlMessage.controlMessage = pawn->MakeControlMessage();

			// Submit it to ourselves
			SubmitControlMessage(controlMessage.controlMessage);
		}

		if (lastControlMessage) {
			// Send controls to the pawn
			pawn->SubmitControlMessage(*lastControlMessage);
		}
	}

	// Do a net timeout if it's been forever
	if (GetTickCount() >= lastActivityTime + netTimeoutTime) {
		SetAlive(false);
	}
}

void Player::SetAlive(bool alive) {
	isAlive = alive;
	lastActivityTime = GetTickCount();

	pawn = game.objects[id];
}