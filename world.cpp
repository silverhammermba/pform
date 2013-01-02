#include "game.hpp"

World::World(const std::string & filename, const sf::Texture & textures)
	: Pform::Level()
{
	std::ifstream in(filename);
	if (in)
	{
		std::string data;
		in.seekg(0, std::ios::end);
		data.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&data[0], data.size());
		in.close();

		char max = -1;
		int width = -1;
		int height = 0;
		int current_width = 0;
		// find level size, check for format errors
		// TODO more informative errors here
		for (auto s = data.begin(); s != data.end(); s++)
		{
			if (*s == '\n')
			{
				if (width == -1)
					width = current_width;
				else if (current_width != width)
					throw std::runtime_error("Inconsistent width in level file");

				current_width = 0;
				height++;
			}
			else if (*s == ' ' || (*s >= '1' && *s <= '4') || (*s >= 'A' && *s <= 'z'))
			{
				current_width++;
				if (*s > max)
					max = *s;
			}
			else
				throw std::runtime_error("Invalid character in level file");
		}

		if (width == -1)
			throw std::runtime_error("No newline found in level file");

		// prepare block array to load blocks
		blocks.resize(max - 'A' + 1, nullptr);

		// check block texture
		auto size = textures.getSize();
		if (size.x % PPB != 0 || size.y % PPB != 0)
			throw std::runtime_error("Invalid block texture size");
		if ((size.x * size.y) / (PPB * PPB) < blocks.size())
			throw std::runtime_error("Not enough block textures for this level");

		set_size(width, height);
		grid.resize(width * height, nullptr);

		players = 0;
		next_player = 0;
		bool start_set[4] = {false, false, false, false};

		// create blocks, player starts
		unsigned int w = 0, h = 0;
		for (auto s = data.begin(); s != data.end(); s++)
		{
			switch(*s)
			{
				case '\n':
					w = 0;
					h++;
					break;
				case ' ':
					w++;
					break;
				case '1':
				case '2':
				case '3':
				case '4':
					players++;
					starts[*s - '1'][0] = w;
					starts[*s - '1'][1] = h;
					start_set[*s - '1'] = true;
					w++;
					break;
				default:
					unsigned int i = *s - 'A';
					if (blocks[i] == nullptr)
						blocks[i] = new Block(textures, sf::IntRect((i % (size.x / PPB)) * PPB, (i / (size.x / PPB)) * PPB, PPB, PPB), true);
					set(w, h, blocks[i]);
					w++;
			}
		}

		if (players == 0 || players > 4)
			throw std::runtime_error("Invalid number of starting positions");

		for (unsigned int i = 0; i < players; i++)
			if (!start_set[i])
				throw std::runtime_error("Invalid starting positions");
	}
	else
		throw std::runtime_error(strerror(errno));
}

World::~World()
{
	for (auto b = blocks.begin(); b != blocks.end(); b++)
		if (*b != nullptr)
			delete *b;
}

Block* World::get(int x, int y) const
{
	return static_cast<Block*>(Pform::Level::get(x, y));
}

const unsigned int* World::get_next_start()
{
	if (next_player >= players)
		throw std::runtime_error("Level player limit exceeded");
	return starts[next_player++];
}

void World::draw_on(sf::RenderWindow& window) const
{
	const unsigned int* size = get_size();
	for (unsigned int w = 0; w < size[0]; w++)
		for (unsigned int h = 0; h < size[1]; h++)
			if (get(w, h) != nullptr)
				get(w, h)->draw_on(window, w * PPB, h * PPB);
}
