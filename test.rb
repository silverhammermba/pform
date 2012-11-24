#!/usr/bin/env ruby

require 'sfml/system'
require 'sfml/window'
require 'sfml/graphics'

include SFML

def reduce y, x
	if y > 0
		y -= [x, y].min
	elsif y < 0
		y += [x, -y].min
	end
	y
end

class Player
	def initialize texture
		@x = 0
		@y = 0
		@dx = 0
		@dy = 0
		# acceleration when key pressed
		@accel = 200
		# break speed when idle
		@break = 400
		# top speed
		@max = 50
		# current velocity
		@v = 0
		@sprite = Sprite.new(texture)
	end

	attr_accessor :dx, :dy
	attr_reader :sprite, :speed

	def find_relevant_region
	end

	def step seconds
		if @v != 0 and @dx * @v <= 0
			@v = reduce(@v, @break * seconds)
		else # @v == 0 or @dx * @v > 0
			@v += @dx * @accel * seconds
			if @v > @max
				@v = @max
			elsif @v < -@max
				@v = -@max
			end
		end
		@x += @v * seconds
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

gray = Color.new(80, 80, 80)
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
				player.dx = -1
			when Keyboard::Right
				player.dx = 1
			end

		when Event::KeyReleased
			case event.key.code
			when Keyboard::Left
				player.dx = 0
			when Keyboard::Right
				player.dx = 0
			end
		end
	end

	time = clock.time.as_seconds
	clock.restart

	player.step(time)

	fps = 1 / time
	fps_text.set_string fps.to_i.to_s

	window.clear(gray)

	window.draw(player.sprite)

	# draw FPS at regular zoom
	window.set_view(window.get_default_view)
	window.draw(fps_text)
	window.set_view(zoom)

	window.display
end
