#ifndef ANIMATION_H_
#define ANIMATION_H_

// TODO don't make these all public
class Frame
{
public:
	sf::IntRect position;
	sf::Vector2i shift;
	float duration;

	Frame();
};

// TODO don't make these all public
class Animation
{
public:
	float duration;
	std::vector<Frame> frames;
	unsigned int loop;

	Animation();

	const Frame& get_frame(float time) const;
};

namespace YAML
{
	template<> struct convert<sf::IntRect> {
		static bool decode(const Node& node, sf::IntRect& rect)
		{
			if (!node.IsSequence() || !node.size() == 4)
				return false;

			rect.left   = node[0].as<int>();
			rect.top    = node[1].as<int>();
			rect.width  = node[2].as<int>();
			rect.height = node[3].as<int>();

			return true;
		}
	};

	template<> struct convert<sf::Vector2i> {
		static bool decode(const Node& node, sf::Vector2i& vec)
		{
			if (!node.IsSequence() || !node.size() == 2)
				return false;

			vec.x = node[0].as<int>();
			vec.y = node[1].as<int>();

			return true;
		}
	};

	template<> struct convert<Frame> {
		static bool decode(const Node& node, Frame& frame)
		{
			frame.position = node["pos"].as<sf::IntRect>();

			if (node["shift"])
				frame.shift = node["shift"].as<sf::Vector2i>();
			if (node["dur"])
				frame.duration = node["dur"].as<float>();

			return true;
		}
	};

	template<> struct convert<Animation> {
		static bool decode(const Node& node, Animation& anim)
		{
			if (!node["frames"])
			{
				throw std::runtime_error("No frame data");
				return false;
			}

			for (auto frame = node["frames"].begin(); frame != node["frames"].end(); frame++)
			{
				anim.frames.push_back(frame->as<Frame>());
			}

			if (anim.frames.size() == 0)
			{
				throw std::runtime_error("Empty frame sequence");
				return false;
			}

			for (auto frame = anim.frames.begin(); frame != anim.frames.end(); frame++)
			{
				anim.duration += frame->duration;
			}

			if (anim.frames.size() > 1 && anim.duration == 0)
			{
				throw std::runtime_error("No duration for non-static animation");
				return false;
			}

			if (node["loop"])
				anim.loop = node["loop"].as<unsigned int>();

			if (anim.loop >= anim.frames.size())
			{
				throw std::runtime_error("Invalid loop index");
				return false;
			}

			for (unsigned int i = 0; i < anim.frames.size(); i++)
			{
				if (anim.frames[i].duration == 0 && (i != anim.frames.size() - 1 || anim.loop != anim.frames.size() - 1))
				{
					throw std::runtime_error("Looping frame requires duration");
					return false;
				}
			}

			return true;
		}
	};
}

#endif
