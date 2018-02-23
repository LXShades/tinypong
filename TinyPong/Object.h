#pragma once
#include "Math.h"
#include "Netgame.h"

enum class ObjectType {
	Paddle,
	Ball
};

class Object {
public:
	~Object() {
		delete lastControlMessage;
	}

	virtual void Update(float deltaTime);
	virtual void Render();
	
	virtual NmControlMessage MakeControlMessage() const;

	void SubmitControlMessage(const NmControlMessage& message);

	virtual ObjectType GetType() const = 0;

	int GetId() const {
		return id;
	}

	void SetId(int id) {
		this->id = id;
	}

protected:
	union {
		Vec2 xy;
		struct {
			float x, y;
		};
	};

	NmControlMessage* lastControlMessage = nullptr;

	int width, height;

	int id;
};