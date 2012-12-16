#ifndef LEVEL_H_
#define LEVEL_H_

namespace Pform
{
	class Level
	{
		unsigned int width;
		unsigned int height;
		std::vector<StaticEntity*> grid;
	public:
		Level(unsigned int w, unsigned int h);

		StaticEntity* get(int x, int y) const;
		void set(int x, int y, StaticEntity* entity);
	};

	class LevelException : public std::runtime_error
	{
	public:
		LevelException();
	};
}

#endif
