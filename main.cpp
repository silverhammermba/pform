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
			*new_player = event.joystickButton.joystickId;
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
			Player* player = new Player(event.joystickButton.joystickId, player_textures[players.size()], 16, level, 5, 20, 20, 50, 40);
			players.push_back(player);
			input_readers.push_back(player);
			new_player = -1;
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		for (auto player = players.begin(); player != players.end(); player++)
		{
			(*player)->step(time);
			auto pos = (*player)->get_sprite_position();
			// TODO get average view or something
			zoom_view.setCenter(pos.x, pos.y);
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
