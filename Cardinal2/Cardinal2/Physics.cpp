#include "stdafx.h"
#include "Physics.h"

bool Physics::CircleToCircle(Circle& A, Circle& B, Collision* C)
{
	double dx = B.pos.x - A.pos.x;
	double dy = B.pos.y - A.pos.y;
	double radii = A.radius + B.radius;
	double length = dx * dx + dy * dy;
	if (length <= radii * radii)
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
		if (x0 * x0 + y0 * y0 <= radius)
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
	if (len1 > radius && len2 > radius)
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