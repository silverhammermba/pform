#!/usr/bin/env ruby

require 'sfml/system'
require 'sfml/window'
require 'sfml/graphics'

include SFML

class Player
	def initialize texture
		@x = 0
		@y = 0
		@sprite = Sprite.new(texture)
		@speed = 100
	end

	attr_reader :sprite, :speed

	def move v
		@x += v[0]
		@y += v[1]
		@sprite.set_position(@x.floor, @y.floor)
	end
end

block_size = 16
ppb = 4

# 10 x 7 block window
w = 10 * block_size * ppb
h = 7 * block_size * ppb
window = RenderWindow.new([w, h], "Pform Ruby Test", Style::Titlebar)
# 4x zoom
zoom = View.new([0, 0], [w / 4, h / 4])
window.set_view zoom

font = Font.new
font.load_from_file("/usr/share/fonts/TTF/VeraMono.ttf")

# for displaying fps
fps_text = Text.new("", font, 12)
fps_text.set_color(Color::Black)

dude = Texture.new
brick = Texture.new
dude.load_from_file("char.png")
brick.load_from_file("block.png")

player = Player.new(dude)

dy, dx = 0, 0

clock = Clock.new

while window.open?

	window.each_event do |event|
		case event.type
		when Event::Closed
			window.close
		when Event::KeyPressed
			case event.key.code
			when Keyboard::Escape
				window.close
			when Keyboard::Left
				dx = -1
			when Keyboard::Right
				dx = 1
			when Keyboard::Up
				dy = -1
			when Keyboard::Down
				dy = 1
			end

		when Event::KeyReleased
			case event.key.code
			when Keyboard::Left
				dx = 0
			when Keyboard::Right
				dx = 0
			when Keyboard::Up
				dy = 0
			when Keyboard::Down
				dy = 0
			end
		end
	end

	time = clock.time.as_seconds
	clock.restart

	# normalize movement
	magnitude = Math::sqrt(dx ** 2 + dy ** 2)

	if magnitude != 0
		dx /= magnitude
		dy /= magnitude

		player.move([dx * player.speed * time, dy * player.speed * time])
	end

	fps = 1 / time
	fps_text.set_string fps.to_i.to_s

	window.clear(Color::White)

	window.draw(player.sprite)

	# draw FPS at regular zoom
	window.set_view(window.get_default_view)
	window.draw(fps_text)
	window.set_view(zoom)

	window.display
end
