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

	Game(sf::RenderWindow* win)
	{
		window = win;
	}

	virtual bool process_event(const sf::Event& event)
	{
		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
		{
			window->close();

			finished = true;
			return false;
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
	// TODO
	unsigned int p_width = 1024 / scale;
	unsigned int p_height = 600 / scale;

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
	sf::Texture zero_texture;
	zero_texture.loadFromFile("sheet.png");

	sf::Texture block_textures;
	block_textures.loadFromFile("panels.png");

	World level("1.level", block_textures);

	sf::Color background(23, 36, 76);

	std::vector<InputReader*> input_readers;
	Player player(0, false, zero_texture, 16, level, 5, 20, 20, 50, 40);

	Game game(&window);

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

		auto pos = zoom_view.getCenter();

		player.step(time);
		pos += player.get_sprite_position();

		pos.x = std::round(pos.x / 2);
		pos.y = std::round(pos.y / 2);

		zoom_view.setCenter(pos.x, pos.y);

		/*
		// check all pairs of players for jumps
		for (auto p1 = players.begin(); p1 != players.end(); p1++)
		{
			auto r1 = (*p1)->get_sprite_bounds();
			for (auto p2 = p1 + 1; p2 != players.end(); p2++)
			{
				auto r2 = (*p2)->get_sprite_bounds();

				// if the sprites intersect
				if (r1.intersects(r2))
				{
					auto pos1 = (*p1)->get_position();
					auto pos2 = (*p2)->get_position();

					double abs[2] = {
						std::fabs(pos1[0] - pos2[0]),
						std::fabs(pos1[1] - pos2[1]),
					};

					double nudge = (1 - abs[0] - (1 - abs[0]) * abs[1]) * 5;
					if (pos1[0] <= pos2[0])
					{
						(*p1)->velocity[0] -= nudge;
						(*p2)->velocity[0] += nudge;
					}
					else
					{
						(*p1)->velocity[0] += nudge;
						(*p2)->velocity[0] -= nudge;
					}

					// if same height, skip it
					if (pos1[1] == pos2[1])
						continue;

					// find higher player, swap pos if necessary
					Player* higher;
					Player* lower;
					if (pos1[1] < pos2[1])
					{
						higher = *p1;
						lower = *p2;
					}
					else
					{
						auto temp = pos1;
						pos1 = pos2;
						pos2 = temp;
						higher = *p2;
						lower = *p1;
					}

					auto vel1 = higher->get_velocity();

					// skip if higher player is not moving downwards
					if (vel1[1] <= 0)
						continue;

					if (abs[0] < 0.5 && abs[1] > 0.75)
					{
						// TODO set jump power properly
						higher->velocity[1] = -16;
						lower->damage();
					}
				}
			}
		}
		*/

		window.setView(zoom_view);

		fps_string.str("");
		fps_string << (unsigned int)(1 / time);
		fps_text.setString(fps_string.str());

		window.clear(background);

		level.draw_on(window);
		player.draw_on(window);

		window.setView(window.getDefaultView());
		window.draw(fps_text);

		window.display();
	}

	return 0;
}
