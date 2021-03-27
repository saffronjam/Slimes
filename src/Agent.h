#pragma once

#include <Saffron.h>

namespace Se
{
struct Agent
{
	alignas(16) sf::Vector2f Position;
	float Angle;
};
}
