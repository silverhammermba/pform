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

const Frame& Animation::get_frame(float time) const
{
	if (duration == 0)
		return frames[0];

	unsigned int i = 0;

	while (true)
	{
		time -= frames[i].duration;

		if (time <= 0)
			break;

		i++;
		if (i == frames.size())
		{
			if (loop == i - 1)
				return frames[loop];

			i = loop;
		}
	}

	return frames[i];
}
