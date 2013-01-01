#ifndef INPUT_H_
#define INPUT_H_

class InputReader
{
protected:
	bool finished;
public:
	InputReader() { finished = false; }

	bool is_finished() const { return finished; }
	virtual bool process_event(const sf::Event& event) = 0;
};

#endif
