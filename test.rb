#!/usr/bin/env ruby

require 'sfml/system'
require 'sfml/window'
require 'sfml/graphics'

include SFML

$block_size = 16
# zoom factor
$zoom = 4

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
		find_relevant_region
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
	attr_reader :sprite, :speed, :minx, :miny, :maxx, :maxy

	# find blocks overlapped by the player
	def find_relevant_region
		@minx = (@x / $block_size).floor
		@maxx = (@x / $block_size).ceil
		@miny = (@y / $block_size).floor
		@maxy = (@y / $block_size).ceil
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
		find_relevant_region if @v != 0
		@sprite.set_position(@x.floor, @y.floor)
	end
end

# 10 x 7 block window
w = 10 * $block_size * $zoom
h = 7 * $block_size * $zoom
window = RenderWindow.new([w, h], "Pform Ruby Test", Style::Titlebar)
# 4x zoom
zoom_view = View.new([0, 0], [w / $zoom, h / $zoom])
window.set_view zoom_view

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

green = Color.new(0, 255, 0, 127)
debug = RectangleShape.new([$block_size, $block_size])
debug.set_fill_color(green)

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

	for i in ((player.miny - 1)..(player.maxy + 1))
		for j in ((player.minx - 1)..(player.maxx + 1))
			debug.set_position(j * $block_size, i * $block_size)
			window.draw(debug)
		end
	end

	# draw FPS at regular zoom
	window.set_view(window.get_default_view)
	window.draw(fps_text)
	window.set_view(zoom_view)

	window.display
end
