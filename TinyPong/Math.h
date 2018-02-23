#pragma once
#include <cmath>

class Vec2 {
public:
	Vec2() = default;
	Vec2(float x_, float y_) : x(x_), y(y_) {};

	Vec2 operator+(const Vec2& other) const {return Vec2(x + other.x, y + other.y);}
	Vec2 operator-(const Vec2& other) const {return Vec2(x - other.x, y - other.y);}
	Vec2 operator*(const Vec2& other) const {return Vec2(x * other.x, y * other.y);}
	Vec2 operator/(const Vec2& other) const {return Vec2(x / other.x, y / other.y);}
	Vec2 operator*(const float scalar) const {return Vec2(x * scalar, y * scalar);}
	Vec2 operator/(const float scalar) const {return Vec2(x / scalar, y / scalar);}
	
	Vec2 operator+=(const Vec2& other) {return (*this = *this + other);}
	Vec2 operator-=(const Vec2& other) {return (*this = *this - other);}
	Vec2 operator*=(const Vec2& other) {return (*this = *this * other);}
	Vec2 operator/=(const Vec2& other) {return (*this = *this / other);}
	Vec2 operator*=(const float scalar) {return (*this = *this * scalar);}
	Vec2 operator/=(const float scalar) {return (*this = *this / scalar);}

	float Length() const {
		return sqrt(x * x + y * y);
	}

	static float Distance(const Vec2& vecA, const Vec2& vecB) {
		return sqrt((vecA.x - vecB.x) * (vecA.x - vecB.x) + (vecA.y - vecB.y) * (vecA.y - vecB.y));
	}

	void ScaleTo(float scalar) {
		if (x || y) {
			*this *= scalar / this->Length();
		} else {
			this->y = scalar;
		}
	}

	float x, y;
};