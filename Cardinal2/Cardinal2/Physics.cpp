#include "stdafx.h"
#include "Physics.h"

bool Physics::CircleToCircle(Circle& A, Circle& B, Collision* C)
{
	double dx = B.pos.x - A.pos.x;
	double dy = B.pos.y - A.pos.y;
	double radii = A.radius + B.radius;
	double length = dx * dx + dy * dy;
	if (length < radii * radii)
	{
		if (C != NULL)
		{
			C->N.x = dx;
			C->N.y = dy;
			C->N.Normalize();
			C->penetration = radii - sqrt(length);
		}
		return true;
	}
	return false;
}

bool Physics::CircleToLine(Circle& A, Line& B, Collision* C)
{
	//Find the closest point on the line to the circle's center
	double x0 = A.pos.x, y0 = A.pos.y;
	double x1 = B.P.x, y1 = B.P.y;
	double x2 = B.Q.x, y2 = B.Q.y;
	double dx = x2 - x1, dy = y2 - y1;
	double t = -((x1 - x0) * dx + (y1 - y0) * dy) / (dx * dx + dy * dy);
	double radius = A.radius;
	radius *= radius;
	
	//Check if point is on the line segment
	if (0.0 <= t && t <= 1.0)
	{
		//Check if point is within the circle
		x0 = x1 + dx * t - x0;
		y0 = y1 + dy * t - y0;
		double length = x0 * x0 + y0 * y0;
		if (x0 * x0 + y0 * y0 < radius)
		{
			if (C != NULL)
			{
				C->N.x = x0;
				C->N.y = y0;
				C->N.Normalize();
				C->penetration = A.radius - sqrt(length);
			}
			return true;
		}
		return false;
	}

	//Check if either endpoint is in the circle
	dx = x1 - x0;
	dy = y1 - y0;
	double len1 = dx * dx + dy * dy;
	dx = x2 - x0;
	dy = y2 - y0;
	double len2 = dx * dx + dy * dy;
	if (len1 >= radius && len2 >= radius)
		return false;
	
	//Check which endpoint is closer
	if (C != NULL)
	{
		if (len1 < len2)
		{
			C->N.x = x1 - x0;
			C->N.y = y1 - y0;
			C->penetration = A.radius - sqrt(len1);
		}
		else
		{
			C->N.x = dx;
			C->N.y = dy;
			C->penetration = A.radius - sqrt(len2);
		}
		C->N.Normalize();
	}
	return true;
}

bool Physics::LineToLine(Line& A, Line& B)
{
	double PX1 = A.P.x; double PX2 = A.Q.x;
	double PY1 = A.P.y; double PY2 = A.Q.y;
	double QX1 = B.P.x; double QX2 = B.Q.x;
	double QY1 = B.P.y; double QY2 = B.Q.y;
	
	double DX1 = PX2 - PX1; double DX2 = QX2 - QX1;
	double DY1 = PY2 - PY1; double DY2 = QY2 - QY1;

	double DPQX = QX1 - PX1; 
	double DPQY = QY1 - PY1;

	double Determinant = -DX1 * DY2 + DX2 * DY1;
	double T1 = -DPQX * DY2 + DX2 * DPQY;
	double T2 = -DPQX * DY1 + DX1 * DPQY;

	//Line segments are parallel if determinant is 0
	if (abs(Determinant) >= 0.001)
	{
		T1 /= Determinant;
		T2 /= Determinant;
		return 0.0 <= T1 && T1 <= 1.0 && 0.0 <= T2 && T2 <= 1.0;
	}
	else if (abs(T1) < 0.001 && abs(T2) < 0.001)
	{
		if (DX1 >= 0.001 && DX1 / DY1 < 0.0)
		{
			PY1 *= -1.0; PY2 *= -1.0;
			QY1 *= -1.0; QY2 *= -1.0;
		}
		Vec2 Min1 = Vec2(min(PX1, PX2), min(PY1, PY2));
		Vec2 Min2 = Vec2(min(QX1, QX2), min(QY1, QY2));
		Vec2 Max1 = Vec2(max(PX1, PX2), min(PY1, PY2));
		Vec2 Max2 = Vec2(max(QX1, QX2), max(QY1, QY2));
		Min1 = Vec2(max(Min1.x, Min2.x), max(Min1.y, Min2.y));
		Max1 = Vec2(min(Max1.x, Max2.x), min(Max1.y, Max2.y));
		return Min1.x <= Max1.x && Min1.y <= Max1.y;
	}
	return false;
}

bool Physics::CircleToBox(Circle& A, Box& B, Collision* C)
{
	int count = B.count;
	Line* S = B.S;
	if (!C)
	{
		for (int i = 0; i < count; i++)
			if (CircleToLine(A, S[i], NULL))
				return true;
	}
	else
	{
		Collision coll;
		Vec2 n;
		double p = 0.0;
		for (int i = 0; i < count; i++)
		{
			if (CircleToLine(A, S[i], &coll) && coll.penetration > p)
			{
				n = coll.N;
				p = coll.penetration;
			}
		}
		if (p > 0.0)
		{
			C->N = n;
			C->penetration = p;
			return true;
		}
	}
	return false;
}

bool Physics::BoxToBox(Box& A, Box& B)
{
	int A_count = A.count;
	int B_count = B.count;
	Line* A_S = A.S;
	Line* B_S = B.S;
	for (int i = 0; i < A_count; i++)
		for (int n = 0; n < B_count; n++)
			if (LineToLine(A_S[i], B_S[n]))
				return true;
	return false;
}

void Physics::ResolveCollision(Shape& A, Shape& B, Collision& C)
{
	//Calculate relative velocity
	Vec2 rvel = B.vel - A.vel;

	//Project relative velocity onto normal
	double nvel = rvel * C.N;

	//If nvel is positive, the objects are moving away from each other
	if (nvel > 0.0)
		return;

	//Separate the objects (displacement proportional to mass)
	double A_invmass = A.invmass;
	double B_invmass = B.invmass;
	Vec2 displacement = 0.05 * C.penetration / (A_invmass + B_invmass) * C.N;
	A.Displace(-A_invmass * displacement);
	B.Displace(B_invmass * displacement);

	//Choose the lower restitution (probably not the best idea..)
	double e = min(A.restitution, B.restitution);

	//Calculate impulse scalar
	double j = -(1 + e) * nvel / (A.invmass + B.invmass);

	//Apply impulse
	Impulse(A, -j, C.N);
	Impulse(B, j, C.N);
}

void Physics::Impulse(Shape& A, double impulse, Vec2& N, bool cancel)
{
	//momentum
	if (cancel)
	{
		double nvel = A.vel * N;
		if (nvel < 0.0)
			A.vel -= nvel * N;
	}
	A.vel += (impulse * A.invmass) * N;
}