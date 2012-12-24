#ifndef WORLD_H_
#define WORLD_H_

class World : public Pform::Level
{
public:
	World(unsigned int w, unsigned int h);
	World(std::string filename);

	void set(int x, int y, Block* block);
};

#endif
