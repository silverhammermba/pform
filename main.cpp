#include <iostream>
#include <sstream>
#include "game.hpp"

using std::cerr;
using std::endl;

// class for handling game-related events
class Game : public InputReader
{
	sf::RenderWindow* window;
	std::vector<InputReader*>* input_readers;
public:

	Game(sf::RenderWindow* win, std::vector<InputReader*>* ir)
	{
		window = win;
		input_readers = ir;
	}

	virtual bool process_event(const sf::Event& event)
	{
		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
		{
			window->close();

			finished = true;
			return false;
		}
		if (event.type == sf::Event::JoystickButtonReleased)
		{
			cerr << event.joystickButton.button << "\n";
		}
		return true;
	}
};

int main(int argc, char* argv[])
{
	// parse arguments
	unsigned int scale = 4;

	if (argc == 2)
	{
		std::stringstream arg;
		arg << argv[1];
		if (!(arg >> scale))
		{
			cerr << "Invalid scale '" << argv[1] << "'\n";
			return 1;
		}
	}
	else if (argc >= 3)
	{
		cerr << "usage: " << argv[0] << " [SCALE]\n";
		return 1;
	}

	// window size in pixels
	unsigned int p_width = 612;
	unsigned int p_height = 300;

	// set up window and view
	sf::RenderWindow window(sf::VideoMode(p_width * scale, p_height * scale), "Pform", sf::Style::Titlebar);
	sf::View zoom_view(sf::Vector2f(0, 0), sf::Vector2f(p_width, p_height));

	// set up text
	sf::Font font;
	font.loadFromFile("/usr/share/fonts/TTF/VeraMono.ttf");

	std::stringstream fps_string;

	sf::Text fps_text("", font, 12);
	fps_text.setColor(sf::Color::Black);

	// load textures
	sf::Texture squid;
	sf::Texture block_textures;
	squid.loadFromFile("char.png");
	block_textures.loadFromFile("blocks.png");

	World level("test.level", block_textures);

	Player player(squid, 16, level, 5, 20, 20, 50, 40);

	sf::Color gray(80, 80, 80);

	std::vector<InputReader*> input_readers;

	Game game(&window, &input_readers);

	input_readers.push_back(&game);
	input_readers.push_back(&player);

	// game loop
	sf::Clock clock;
	while (window.isOpen())
	{
		// process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			for (auto r = input_readers.begin(); r != input_readers.end();)
			{
				bool cont = (*r)->process_event(event);
				if ((*r)->is_finished())
					r = input_readers.erase(r);
				else
					r++;
				if (!cont)
					break;
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		player.step(time);

		auto pos = player.get_sprite_position();

		zoom_view.setCenter(pos.x, pos.y);
		window.setView(zoom_view);

		fps_string.str("");
		fps_string << (unsigned int)(1 / time);
		fps_text.setString(fps_string.str());

		window.clear(gray);

		level.draw_on(window);
		player.draw_on(window);

		window.setView(window.getDefaultView());
		window.draw(fps_text);

		window.display();
	}

	return 0;
}
