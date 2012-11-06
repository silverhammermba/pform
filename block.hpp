#ifndef BLOCK_H_
#define BLOCK_H_

class Block
{
	bool solid;
public:
	Block(bool s = true);

	inline bool is_solid() const { return solid; };
};

#endif
