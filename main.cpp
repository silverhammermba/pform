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
	int* new_player;
public:

	Game(sf::RenderWindow* win, int* np)
	{
		window = win;
		new_player = np;
	}

	virtual bool process_event(const sf::Event& event)
	{
		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
		{
			window->close();

			finished = true;
			return false;
		}
		if (event.type == sf::Event::JoystickButtonReleased && event.joystickButton.button == 7)
		{
			*new_player = event.joystickButton.joystickId;
		}
		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
		{
			*new_player = 10;
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
	sf::Texture player_textures[4];
	player_textures[0].loadFromFile("char0.png");
	player_textures[1].loadFromFile("char1.png");
	player_textures[2].loadFromFile("char2.png");
	player_textures[3].loadFromFile("char3.png");

	sf::Texture block_textures;
	block_textures.loadFromFile("blocks.png");

	World level("test.level", block_textures);

	std::vector<Player*> players;

	sf::Color gray(80, 80, 80);

	std::vector<InputReader*> input_readers;

	int new_player = -1;

	Game game(&window, &new_player);

	input_readers.push_back(&game);

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

		if (new_player >= 0)
		{
			bool taken = false;
			for (auto player = players.begin(); player != players.end(); player++)
			{
				if ((int)((*player)->get_joystick()) == new_player)
				{
					taken = true;
					break;
				}
			}

			if (!taken)
			{
				Player* player = new Player(new_player, new_player == 10, player_textures[players.size()], 16, level, 5, 20, 20, 50, 40);
				players.push_back(player);
				input_readers.push_back(player);
			}
			new_player = -1;
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		sf::Vector2f pos(0, 0);

		for (auto player = players.begin(); player != players.end(); player++)
		{
			(*player)->step(time);
			pos += (*player)->get_sprite_position();
		}

		if (players.size() > 0)
		{
			pos.x = std::round(pos.x / players.size());
			pos.y = std::round(pos.y / players.size());
		}

		zoom_view.setCenter(pos.x, pos.y);

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
					{
						continue;
					}

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

		window.setView(zoom_view);

		fps_string.str("");
		fps_string << (unsigned int)(1 / time);
		fps_text.setString(fps_string.str());

		window.clear(gray);

		level.draw_on(window);
		for (auto player = players.begin(); player != players.end(); player++)
			(*player)->draw_on(window);

		window.setView(window.getDefaultView());
		window.draw(fps_text);

		window.display();
	}

	for (auto player = players.begin(); player != players.end(); player++)
		delete *player;

	return 0;
}
