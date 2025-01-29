#include "geometry.h"

struct Point {
	int x; 
	int y;
};

void GetConsoleSize(int& widht, int& height);
bool ShowConsoleCursor(bool bShow);
void cursorIn(Point _where);
template<typename T> void inRange(T& val, T mn, T mx);
Point3 getPointOnScreen(int row, int col);

void GetConsoleSize(int& widht, int& height) {
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hCon, &csbi);
	widht = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

bool ShowConsoleCursor(bool bShow)
{
	CONSOLE_CURSOR_INFO cci;
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hCon == INVALID_HANDLE_VALUE)
		return false;
	if (!GetConsoleCursorInfo(hCon, &cci))
		return false;
	cci.bVisible = bShow;
	if (!SetConsoleCursorInfo(hCon, &cci))
		return false;
	return true;
}

// Переместить курсор в точку _where, _where.x это строка а _where.y это символ
void cursorIn(Point _where) {
	HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);
	// Меняем местами, ибо функция принимает сначала символ а потом строку
	SetConsoleCursorPosition(cons, { (SHORT)_where.y, (SHORT)_where.x });
}

std::ostream& operator<<(std::ostream& out, Coord3& point) {
	out << "x = " << point.x << " y = " << point.y << " z = " << point.z;
	return out;
}

float Point3::distanceTo(const Point3& _other) const {
	return Vector3(*this, _other).modul();
}

float Vector3::modul() const {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector3& Vector3::normalize() {
	*this = *this / modul();
	return *this;
}

Ray3::Ray3(const Vector3& dir) {
	direction = dir / dir.modul();
}

Ray3::Ray3(const Vector3& dir, const Point3& poi)
	: start(poi)
{
	direction = dir / dir.modul();
}

class Object {
public:
	std::string gradient;

	virtual bool ifContains(const Point3& p3) const = 0;

	virtual bool intersection(Ray3& r, float& t1, float& t2) const = 0;
};

class Sphere : public Object {
public:
	float radius = 1.f;
	Point3 center{ 0, 0, 0 };

	Sphere(float _radius = 1.f, Point3 _center = { 0, 0, 0 }, std::string _gradient = "@#+*-.")
		: radius(_radius), center(_center) 
	{
		gradient = _gradient;
	}

	bool ifContains(const Point3& p3) const override {
		return (pow(p3.x - center.x, 2) + pow(p3.y - center.y, 2) + pow(p3.z - center.z, 2)) <= pow(radius, 2);
	}

	bool intersection(Ray3& ray, float& t0, float& t1) const override {
		ray.direction.normalize();

		Vector3 oc(center, ray.start);
		float dotOCD = ray.direction * oc;

		if (dotOCD > 0)
			return false;

		float dotOC = oc * oc;
		float discriminant = dotOCD * dotOCD - dotOC + radius * radius;

		if (discriminant < 0)
			return false;

		if (abs(discriminant) < 1E-6)
			discriminant = 0;

		discriminant = sqrt(discriminant);

		t0 = -dotOCD - discriminant;
		t1 = -dotOCD + discriminant;
		
		if (t0 < 0) 
			t0 = t1;

		return true;
	}
};

class Scene {
public:
	Scene() {}
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;

	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) = delete;

	void addObject(Object* x) {
		m_objects.push_back(x);
	}

	Object* operator[](int i) {
		if (i < 0 || i >= m_objects.size())
			return nullptr;
		return m_objects[i];
	}

	int getNumOfObject() {
		return m_objects.size();
	}

	~Scene() {
		for (auto obj : m_objects)
			delete obj;
	}

private:
	std::vector<Object*> m_objects;
};
template<typename T>
void inRange(T& val, T mn, T mx) {
	val = max(min(mx, val), mn);
}

Point3 getPointOnScreen(int row, int col) {
	int cwight = 0, cheight = 0;
	float symbRatio = 24.f / 11.f;

	GetConsoleSize(cwight, cheight);

	float consRatio = (float)cwight / cheight;

	Point3 p;

	p.x = (float)row / (cheight / 2) - 1.f;
	p.y = (float)col / (cwight / 2) - 1.f;

	p.y /= symbRatio;
	p.y *= consRatio;

	return p;
}

void testing() {
	Vector3 a(1, 2, 3), b(3, 2, 5);
	std::cout << a << '\n' << a * a << '\n' << pow(a.modul(), 2);
	Sleep(10000);
}

int main()
{
	int cwight = 0, cheight = 0;
	int new_wight = 0, new_height = 0;
	std::string screen;

	float t = 0;

	Point3 point;
	Point3 pointOnScreen;

	//Vector3 cameraDirection = Vector3(-1, -1, -1);
	Point3 cameraStart = Point3(0, 0, 3.5);
	
	Point3 light(0, 0, 0);
	
	Ray3 camera_ray;
	Ray3 light_ray;
	
	//testing();

	float distance = 0.f;
	float t0, t1;

	Scene scene;

	float light_power = 0.9f;
	/*
	Sphere lightSphere(0.2f, light, "%%");
	Sphere first_sphere(0.4f, { 0, 0, 0 });
	Sphere second_sphere(0.25f, { -0.45f, 0, 0 });
	Sphere third_sphere(0.45f, { 0.5f, 0, 0 });
	
	scene.addObject(&first_sphere);
	scene.addObject(&lightSphere);
	scene.addObject(&second_sphere);
	scene.addObject(&third_sphere);
	//*/

	/*
	Sphere lightSphere(0.5f, light, "%%");
	Sphere first_sphere(0.2f, { 0, 10, 10 });
	Sphere second_sphere(0.2f, { 10, 0, 10 });
	Sphere third_sphere(0.2f, { 10, 10, 0 });
	Sphere fourth_sphere(0.2f, { 10, 10, 0, });

	scene.addObject(&first_sphere);
	scene.addObject(&lightSphere);
	scene.addObject(&second_sphere);
	scene.addObject(&third_sphere);
	scene.addObject(&fourth_sphere);
	//*/

	///*
	Sphere sphere(0.3f, { 0, 0, 0 });

	Sphere lightSphere(0.2f, light, "%%");
	
	Sphere bottom_big_sphere(0.3f, { 1, 0, 0 });
	Sphere up_big_sphere(0.5f, { -0.5, 0, 0 });
	Sphere another_sphere(0.1f, { 0, 0, 0 });
	Sphere upup_big_sphere(1.f, { -1.5, 0, 0 });

	scene.addObject(&sphere);
	scene.addObject(&lightSphere);
	scene.addObject(&bottom_big_sphere);
	scene.addObject(&up_big_sphere);
	scene.addObject(&another_sphere);
	scene.addObject(&upup_big_sphere);
	//*/

	while (true) {
		GetConsoleSize(new_wight, new_height);

		if (new_wight != cwight || new_height != cheight) {
			screen.resize(new_wight * new_height);
			cwight = new_wight;
			cheight = new_height;
			ShowConsoleCursor(false);
		}

		t += 0.1f;
		/*

		light.x = cos(t) * 1.2f;
		light.y = 1./tan(t) * 1.2f;
		light.z = tan(t) * .5f;
		lightSphere.center = light;
		
		//*/
		
		/*
		first_sphere.center.z = sin(t) * 1;
		first_sphere.center.y = cos(t) * 1;

		second_sphere.center.x = sin(t) * 1;
		second_sphere.center.z = cos(t) * 1;

		third_sphere.center.y = tan(t) * 1;
		third_sphere.center.x = cos(t) * 1;

		fourth_sphere.center.x = 1./tan(t) * 1;
		fourth_sphere.center.y = sin(t) * 1;

		lightSphere.radius = abs(sin(t)) / 5 + 0.3;
		//*/

		///*
		another_sphere.center.z = (sin(t * 2) * 0.4f) + cos(t) * 1.2f;
		another_sphere.center.y = (cos(t * 2) * 0.4f) + sin(t) * 1.2f;

		light.z = cos(t) * 1.2f;
		light.y = sin(t) * 1.2f;
		lightSphere.center = light;
		//*/

		screen.assign(cheight * cwight, ' ');

		for (int i = 0; i < cheight; i++) {
			for (int j = 0; j < cwight; j++) {


				pointOnScreen = getPointOnScreen(i, j);
				point = pointOnScreen;

				camera_ray = Ray3(cameraStart, point);
				
				distance = FLT_MAX;

				for (int num = 0; num < scene.getNumOfObject(); num++) {
					if (scene[num]->intersection(camera_ray, t0, t1)) {
						Point3 intersectionPoint = camera_ray.start + camera_ray.direction * t0;
						
						float newDistance = cameraStart.distanceTo(intersectionPoint);
						if (newDistance < distance) {
							distance = newDistance;
							
							light_ray = Ray3(intersectionPoint, light);

							intersectionPoint = intersectionPoint + light_ray.direction / 100.f;

							float light_ray_len = intersectionPoint.distanceTo(light);

							bool flag = false;
							for (int num2 = 0; num2 < scene.getNumOfObject(); num2++) {
								if (num2 == 1) continue;

								if (scene[num2]->intersection(light_ray, t0, t1)) {
									Point3 newInterPoint = light_ray.start + light_ray.direction * t0;
									if (newInterPoint.distanceTo(intersectionPoint) < light_ray_len) {
										flag = true;
										break;
									}
								}
							}

							if (flag)
								screen[i * cwight + j] = scene[num]->gradient.back();
							else {
								if (light_ray_len <= light_power)
									screen[i * cwight + j] = scene[num]->gradient[0];
								else
								{
									light_ray_len -= light_power;
									int shadow = scene[num]->gradient.size() * light_ray_len / light_power;

									inRange(shadow, 1, (int)scene[num]->gradient.size() - 1);
									screen[i * cwight + j] = scene[num]->gradient[shadow];
								}
							}
						}
					}
				}
			}
		}

		cursorIn({ 0, 0 });
		std::cout << screen;
		cursorIn({ 0, 0 });
		std::cout << "camera: " << cameraStart;
		std::cout << "\nlight: " << light;
		std::cout << "\n\ndistance to light: " << cameraStart.distanceTo(lightSphere.center);
	}
}
