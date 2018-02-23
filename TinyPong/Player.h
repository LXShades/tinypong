#pragma once
#include "Object.h"
#include "Netgame.h"

class Player {
public:
	static const int netTimeoutTime = 5000; // in ms
public:
	Player(int id_, const NetAddr& sourceAddr) : id(id_), isAlive(true), address(sourceAddr) {};

	void Update();

	int GetId() const {
		return id;
	}
	
	void SetAlive(bool alive);

	bool IsAlive() const {
		return isAlive;
	}

	void SetAddr(const NetAddr& addr) {
		address = addr;
	}

	inline void SubmitControlMessage(const NmControlMessage& message);
	inline const NmControlMessage* GetLastControlMessage() const;

	inline const NetAddr& GetAddr() const {
		return address;
	}

	void SetPawn(Object* pawn) {
		this->pawn = pawn;
	}

	Object* GetPawn() {
		return pawn;
	}

private:
	bool isAlive = false;

	// ID of the player in the player list
	int id;

	Object* pawn;

	unsigned int lastActivityTime;

	NetAddr address;
	NmControlMessage* lastControlMessage = nullptr;
};

inline void Player::SubmitControlMessage(const NmControlMessage& message) {
	delete lastControlMessage;
	lastControlMessage = new NmControlMessage(message);
	lastControlMessage->base.playerId = id;

	lastActivityTime = GetTickCount();
}

inline const NmControlMessage* Player::GetLastControlMessage() const {
	return lastControlMessage;
}