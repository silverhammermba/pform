#ifndef LEVEL_H_
#define LEVEL_H_

namespace Pform
{
	class Level
	{
		unsigned int width;
		unsigned int height;
		std::vector<Pform::Entity*> grid;
	public:
		Level(unsigned int w, unsigned int h);

		Pform::Entity* get(int x, int y);
		void set(int x, int y, Pform::Entity* entity);
	};

	class LevelException : public std::runtime_error
	{
	public:
		LevelException();
	};
}

#endif
