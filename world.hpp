#ifndef WORLD_H_
#define WORLD_H_

class World : public Pform::Level
{
public:
	World(unsigned int w, unsigned int h);
	World(std::string filename);

	Block* get(int x, int y) const;

	void draw_on(sf::RenderWindow& window) const;
};

#endif
