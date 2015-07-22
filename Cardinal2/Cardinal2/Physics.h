#pragma once

struct Vec2
{
	Vec2() {}
	Vec2(double X, double Y)
	{
		x = X;
		y = Y;
	}
	double x;
	double y;

	void Normalize()
	{
		double length = sqrt(x * x + y * y);
		if (length > 0.0)
		{
			x /= length;
			y /= length;
		}
	}

	//assignment
	Vec2& operator+=(const Vec2& V) { x += V.x; y += V.y; return *this; }
	Vec2& operator-=(const Vec2& V) { x -= V.x; y -= V.y; return *this; }
	Vec2& operator*=(const Vec2& V) { x *= V.x; y *= V.y; return *this; }
	Vec2& operator*=(double scalar) { x *= scalar; y *= scalar; return *this; }
	Vec2& operator/=(double scalar) { x /= scalar; y /= scalar; return *this; }

	//binary
	Vec2 operator+(const Vec2& V) { return Vec2(x + V.x, y + V.y); }
	Vec2 operator-(const Vec2& V) { return Vec2(x - V.x, y - V.y); }
	Vec2 operator*(double scalar) { return Vec2(x * scalar, y * scalar); }
	Vec2 operator/(double scalar) { return Vec2(x / scalar, y / scalar); }
	double operator*(const Vec2& V) { return x * V.x + y * V.y; }
	friend Vec2 operator*(double scalar, const Vec2& V) { return Vec2(V.x * scalar, V.y * scalar); }

	//unary
	Vec2 operator-() { return Vec2(-x, -y); }
};

struct Shape
{
	Shape() {}
	Shape(double X, double Y, double VX, double VY, double IM, double RE) : pos(X, Y), vel(VX, VY)
	{
		invmass = IM;
		restitution = RE;
	}
	virtual void Displace(Vec2& V) = 0;
	Vec2 pos;
	Vec2 vel;
	double invmass; //reciprocal of mass; 0.0 = immobile
	double restitution; //0.0 = no bounce
};

struct Circle : Shape
{
	Circle() {}
	Circle(double X, double Y, double VX, double VY, double R, double IM, double RE) : Shape(X, Y, VX, VY, IM, RE) { radius = R; }
	void Displace(Vec2& V) { pos += V; }
	double radius;
};

struct Line
{
	Line() {}
	Line(double X1, double Y1, double X2, double Y2) : P(X1, Y1), Q(X2, Y2) {}
	Vec2 P;
	Vec2 Q;
};

struct Box : Shape
{
	Box() {}
	Box(Line* L, int C, double VX, double VY, double IM, double RE) : Shape(0.0, 0.0, VX, VY, IM, RE)
	{
		S = new Line[C];
		for (int i = 0; i < C; i++)
			S[i] = L[i];
		count = C;
	}
	~Box() { delete[] S; }
	void Displace(Vec2& V)
	{
		for (int i = 0; i < count; i++)
		{
			S[i].P += V;
			S[i].Q += V;
		}
	}
	Line* S;
	int count;
};

struct Collision
{
	Shape* A;
	Shape* B;
	Vec2 N;
	double penetration;
};

class Physics
{
public:
	static bool CircleToCircle(Circle& A, Circle& B, Collision* C);
	static bool CircleToLine(Circle& A, Line& B, Collision* C);
	static void ResolveCollision(Shape& A, Shape& B, Collision& C);
	static void Impulse(Shape& A, double impulse, Vec2& N, bool cancel = false);
};