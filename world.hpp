#ifndef WORLD_H_
#define WORLD_H_

class World : public Pform::Level
{
	unsigned int players;
	unsigned int starts[4][2];
	unsigned int next_player;
	std::vector<Block*> blocks;
public:
	World(const std::string & filename, const sf::Texture & textures);
	~World();

	const unsigned int* get_next_start();

	Block* get(int x, int y) const;

	void draw_on(sf::RenderWindow& window) const;
};

#endif
