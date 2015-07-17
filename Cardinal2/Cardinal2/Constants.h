#pragma once

namespace C
{
	const double TICK = 0.005;
	const double METER = 256.0;
	const double HALF = METER / 2.0;
	const double RADIUS = 100.0;
	const double PI = atan(1.0) * 4.0;
	const double TWOPI = 2 * PI;
	const double BUFFER = 0.100f;

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const double SCREEN_WIDTH = 8.0 * METER;
	const double RATIO = WINDOW_WIDTH / SCREEN_WIDTH;
	const double SCREEN_HEIGHT = WINDOW_HEIGHT / RATIO;
	const double CENTER_X = SCREEN_WIDTH / 2.0;
	const double CENTER_Y = SCREEN_HEIGHT / 2.0;
	const double CAMERA_SCROLL = 2.0 * METER;
}