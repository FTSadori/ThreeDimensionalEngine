#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

class Coord3;
class Point3;
class Vector3;
class Ray3;

class Object;
class Sphere;

// Класс для объектов, имеющих в основе координаты пространства и способные к + - == !=
class Coord3 {
public:
	float x{ 0 };
	float y{ 0 };
	float z{ 0 };

	Coord3() {}

	Coord3(float _x, float _y = 0, float _z = 0)
		: x(_x), y(_y), z(_z) {}

	Coord3 operator+(const Coord3& _other) const {
		return Coord3(x + _other.x, y + _other.y, z + _other.z);
	}

	Coord3 operator-(const Coord3& _other) const {
		return Coord3(x - _other.x, y - _other.y, z - _other.z);
	}

	bool operator==(const Coord3& _other) const {
		return (x == _other.x && y == _other.y && z == _other.z);
	}

	bool operator!=(const Coord3& _other) const {
		return (x != _other.x || y != _other.y || z != _other.z);
	}

	friend std::ostream& operator<<(std::ostream& out, Coord3& point);
};

// Объект точка
class Point3 : public Coord3 {
public:
	Point3() {}

	Point3(const Coord3& coord)
		: Coord3(coord) {}

	Point3(float _x, float _y = 0, float _z = 0)
		: Coord3(_x, _y, _z) {}

	float distanceTo(const Point3& _other) const;
};

// Объект вектор
class Vector3 : public Coord3 {
public:
	Vector3() {}

	Vector3(float _x, float _y, float _z)
		: Coord3(_x, _y, _z) {}

	Vector3(const Point3& _beg, const Point3& _end)
		: Coord3(_end.x - _beg.x, _end.y - _beg.y, _end.z - _beg.z) {}

	Vector3(Point3&& _beg, Point3&& _end)
		: Coord3(_end.x - _beg.x, _end.y - _beg.y, _end.z - _beg.z) {}

	float modul() const;

	Vector3& normalize();

	Vector3 operator*(float n) const {
		return Vector3(x * n, y * n, z * n);;
	}

	Vector3 operator/(float n) const {
		if (n == 0) return Vector3(x, y, z);
		return Vector3(x / n, y / n, z / n);
	}

	float operator*(const Vector3& vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}
};

// Объект луч
class Ray3 {
public:
	Point3 start{ 0, 0, 0 };
	Vector3 direction{ 0, 0, 0 };

	Ray3() {}

	Ray3(const Vector3& dir);

	Ray3(const Vector3& dir, const Point3& poi);

	Ray3(const Point3& beg, const Point3& end) : start(beg) {
		Vector3 vec(beg, end);
		direction = vec.normalize();
	}
};