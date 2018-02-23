#pragma once
#include "Math.h"
#include "Netgame.h"

enum class ObjectType {
	Paddle,
	Ball
};

class Object {
public:
	Object() : lastControlMessage(nullptr) {};
	Object(const Object& other) : xy(other.xy), width(other.width), height(other.height), id(other.id) {}

	~Object() {
		delete lastControlMessage;
	}

	virtual void Update(float deltaTime);
	virtual void Render();
	
	virtual NmControlMessage MakeControlMessage() const;

	void SubmitControlMessage(const NmControlMessage& message);

	virtual ObjectType GetType() const = 0;

	virtual void OnCollision(const Object& other, bool isSecondary = false);

	bool IsColliding(const Object& other);

	int GetId() const {
		return id;
	}

	void SetId(int id) {
		this->id = id;
	}

	void SetX(float x) {
		this->x = x;
	}

	void SetY(float y) {
		this->y = y;
	}

	float GetX() const {
		return x;
	}
	float GetY() const {
		return y;
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