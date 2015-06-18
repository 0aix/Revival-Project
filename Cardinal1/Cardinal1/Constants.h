#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace C
{
	const float TICK = 0.005f;
	const float METER = 256.0f;
	const float HALF = METER / 2.0f;
	const float RADIUS = 100.0f;
	const float HALF_RADIUS = RADIUS / 2.0f;
	const float QUEUE_TIME = 0.400f;
	const float TURN_MOVE_TIME = 0.300f;

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const float SCREEN_WIDTH = 8.0f * METER;
	const float RATIO = WINDOW_WIDTH / SCREEN_WIDTH;
	const float SCREEN_HEIGHT = WINDOW_HEIGHT / RATIO;
	const float CENTER_X = SCREEN_WIDTH / 2.0f;
	const float CENTER_Y = SCREEN_HEIGHT / 2.0f;
	const float CAMERA_SCROLL = 2.0f * METER;

	const float WARRIOR_SPEED = 0.0f;
	const float WARRIOR_AUTO1 = 0.800f;
	const float WARRIOR_AUTO2 = 0.800f;
	const float WARRIOR_AUTO3 = 0.800f;
	const float WARRIOR_AUTO_CHAIN = 0.400f;
	const float WARRIOR_GUARD = 0.200f;
}

#endif