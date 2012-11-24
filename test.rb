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

w = 640
h = 480
window = RenderWindow.new([w, h], "Pform Ruby Test", Style::Titlebar)
view = View.new([0, 0], [w / 4, h / 4])
window.set_view view

clock = Clock.new

font = Font.new
font.load_from_file("/usr/share/fonts/TTF/VeraMono.ttf")

fps_text = Text.new("", font, 12)
fps_text.set_color(Color::Black)

dude = Texture.new
dude.load_from_file("char.png")

player = Player.new dude

dy, dx = 0, 0

while window.open?

	STDERR.puts "Processing events"
	window.each_event do |event|
		STDERR.puts event.inspect
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
			case
			when Keyboard::Left
				STDERR.puts "Released Left"
				dx = 0
			when Keyboard::Right
				STDERR.puts "Released Right"
				dx = 0
			when Keyboard::Up
				STDERR.puts "Released Up"
				dy = 0
			when Keyboard::Down
				STDERR.puts "Released Down"
				dy = 0
			end
		end
	end

	# normalize movement
	magnitude = Math::sqrt(dx ** 2 + dy ** 2)

	if magnitude != 0
		dx /= magnitude
		dy /= magnitude

		time = clock.time.as_seconds
		player.move([dx * player.speed * time, dy * player.speed * time])
	end

	window.clear(Color::White)

	fps = 1 / clock.time.as_seconds
	fps_text.set_string fps.to_i.to_s
	clock.restart

	window.draw(player.sprite)
	window.draw(fps_text)

	window.display
end
