#include <iostream>
#include <sstream>
#include "game.hpp"

using std::cerr;
using std::endl;

int main(int argc, char* argv[])
{
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

	// window size in blocks
	unsigned int b_width = 20;
	unsigned int b_height = b_width / 2;

	// window size in pixels
	unsigned int p_width = b_width * PPB;
	unsigned int p_height = b_height * PPB;

	// set up window and view
	sf::RenderWindow window(sf::VideoMode(p_width * scale, p_height * scale), "Pform", sf::Style::Titlebar);
	sf::View zoom_view(sf::Vector2f(p_width / 2, p_height / 2), sf::Vector2f(p_width, p_height));
	window.setView(zoom_view);

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

	// game loop
	sf::Clock clock;
	while (window.isOpen())
	{
		// process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					switch (event.key.code)
					{
						case sf::Keyboard::Escape:
							window.close();
						case sf::Keyboard::Left:
							player.set_movement(-1);
							break;
						case sf::Keyboard::Right:
							player.set_movement(1);
							break;
						case sf::Keyboard::Up:
							player.jump();
							break;
						default:
							break;
					}
					break;
				case sf::Event::KeyReleased:
					switch (event.key.code)
					{
						case sf::Keyboard::Left:
						case sf::Keyboard::Right:
							player.set_movement(0);
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		player.step(time);

		fps_string.str("");
		fps_string << (unsigned int)(1 / time);
		fps_text.setString(fps_string.str());

		window.clear(gray);

		level.draw_on(window);
		player.draw_on(window);

		window.setView(window.getDefaultView());
		window.draw(fps_text);
		window.setView(zoom_view);

		window.display();
	}

	return 0;
}
