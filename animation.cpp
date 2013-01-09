#include "game.hpp"

Frame::Frame() : position(), shift(0, 0)
{
	duration = 0;
}

Animation::Animation() : frames()
{
	loop = 0;
	duration = 0;
}
