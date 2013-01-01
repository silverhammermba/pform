#ifndef LEVEL_H_
#define LEVEL_H_

namespace Pform
{
	class Level
	{
	private:
		unsigned int size[2];
	protected:
		std::vector<StaticEntity*> grid;

		void set_size(unsigned int w, unsigned int h);
	public:
		Level();
		Level(unsigned int w, unsigned int h);

		const unsigned int* get_size() const { return size; }
		StaticEntity* get(int x, int y) const;
		bool is_passable(int x, int y) const;
		void set(int x, int y, StaticEntity* entity);
	};

	class LevelException : public std::runtime_error
	{
	public:
		LevelException()
			: std::runtime_error("Invalid grid index")
		{}
	};
}

#endif
