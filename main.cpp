#include <sstream>
#include <iostream>
#include "game.hpp"

using std::cerr;
using std::endl;

#define SCALE 4 // pixel scaling factor

int main(int argc, char* argv[])
{
	// window size in blocks
	unsigned int b_width = 10;
	unsigned int b_height = 8;

	// window size in pixels
	unsigned int p_width = b_width * PPB;
	unsigned int p_height = b_height * PPB;

	// set up window and view
	sf::RenderWindow window(sf::VideoMode(p_width * SCALE, p_height * SCALE), "Pform", sf::Style::Titlebar);
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
	sf::Texture panel;
	squid.loadFromFile("char.png");
	panel.loadFromFile("block.png");

	Block block(panel);

	Pform::Level level(b_width, b_height);
	level.set(0, 1, &block);
	level.set(0, 5, &block);
	level.set(0, 6, &block);
	level.set(1, 3, &block);
	level.set(1, 5, &block);
	level.set(1, 6, &block);
	level.set(5, 5, &block);
	level.set(7, 5, &block);
	level.set(9, 5, &block);
	level.set(9, 6, &block);

	Player player(squid, 250, level, 8, 6, 75, 300, 300, 800, 600);

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
						// TODO other player controls
						default:
							break;
					}
				case sf::Event::KeyReleased:
					// TODO player controls
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
		for (unsigned int w = 0; w < b_width; w++)
			for (unsigned int h = 0; h < b_height; h++)
				if (level.get(w, h) != nullptr)
					static_cast<Block*>(level.get(w, h))->draw_on(window);
		player.draw_on(window);

		window.setView(window.getDefaultView());
		window.draw(fps_text);
		window.setView(zoom_view);

		window.display();
	}

	return 0;
}
