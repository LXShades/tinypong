#pragma once

class Input {
public:
	void Update();

	float GetMouseX() const;
	float GetMouseY() const;

	bool IsLeftButtonBooped() const;

private:
	float mouseX, mouseY;
	bool lDown, lBoop;
};