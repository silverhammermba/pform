#ifndef WORLD_H_
#define WORLD_H_

class World : public Pform::Level
{
	unsigned int starts[4][2];
	bool next_player = 0;
	std::vector<Block*> blocks;
public:
	World(const std::string & filename, const sf::Texture & textures);
	~World();

	const unsigned int* get_next_start() { return starts[next_player++]; }

	Block* get(int x, int y) const;

	void draw_on(sf::RenderWindow& window) const;
};

#endif
