#!/usr/bin/env ruby

require 'sfml/system'
require 'sfml/window'
require 'sfml/graphics'

include SFML

$block_size = 16
# zoom factor
$zoom = 4

class Numeric
	def sign
		if self > 0
			return 1
		elsif self < 0
			return -1
		end
		return 0
	end
end

def reduce y, x
	if y > 0
		y -= [x, y].min
	elsif y < 0
		y += [x, -y].min
	end
	y
end

class Player
	def initialize level, texture, x, y
		@level = level
		@x = x * $block_size
		@y = y * $block_size
		find_relevant_region
		@sprite = Sprite.new(texture)
		@sprite.set_position(@x.floor, @y.floor)
		# x direction from keyboard
		@dir = 0
		@dx = 0
		@dy = 0
		# acceleration when key pressed
		@accel = 200
		# break speed when idle
		@break = 400
		# top speed
		@max = 50
		# acceleration from gravity
		@gravity = 0
		# if the player is on the ground
		@standing = false
		# terminal velocity
		@tv = 100
		# current velocity
		@vx = 0
		@vy = 0
	end

	attr_accessor :dir
	attr_reader :sprite, :speed, :minx, :miny, :maxx, :maxy

	def jump
		if @standing
			@vy = -100
			@standing = false
		end
	end

	# find blocks overlapped by the player
	def find_relevant_region
		@minx = (@x / $block_size).floor
		@maxx = (@x / $block_size).ceil
		@miny = (@y / $block_size).floor
		@maxy = (@y / $block_size).ceil
	end

	# move the player
	def step seconds
		if @vx != 0 and @dir * @vx <= 0
			@vx = reduce(@vx, @break * seconds)
		else # @vx == 0 or @dir * @vx > 0
			@vx += @dir * @accel * seconds
			if @vx > @max
				@vx = @max
			elsif @vx < -@max
				@vx = -@max
			end
		end
		if not @standing
			@vy += @gravity * seconds
		end
		@dx = @vx * seconds
		@dy = @vy * seconds
		if @dx != 0 or @dy != 0
			resolve_movement
			find_relevant_region
			@sprite.set_position(@x.floor, @y.floor)
		end
	end

	# avoid overlaps with nearby blocks
	def resolve_movement
		# purely horizontal movement
		if @dy == 0
			nextmin = ((@x + @dx) / $block_size).floor
			nextmax = ((@x + @dx) / $block_size).ceil
			# if you are moving into a new block
			if nextmin != @minx or nextmax != @maxx
				if @dx > 0
					if @level[@miny][nextmax] or @level[@maxy][nextmax]
						@x = nextmin * $block_size
					else
						@x += @dx
					end
				else # @dx < 0
					if @level[@miny][nextmin] or @level[@maxy][nextmin]
						@x = nextmax * $block_size
					else
						@x += @dx
					end
				end
			else
				@x += @dx
			end
		# TODO copy x case
		# purely vertical movement
		elsif @dx == 0
			# moving down
			if @dy > 0
				if @level[@maxy + 1][@minx] or @level[@maxy + 1][@maxx]
					@y = (@y / $block_size).ceil * $block_size
				else
					@y += @dy
				end
			# moving up
			else # @dy < 0
				if @level[@miny - 1][@minx] or @level[@miny - 1][@maxx]
					@y = (@y / $block_size).floor * $block_size
				else
					@y += @dy
				end
			end
		else # @dx != 0 and @dy != 0
			if @dx < 0
			else # @dx > 0
			end
		end
		collision = false
		# TODO possible infinite loop?
		resolve_movement if collision
	end
end

class Block
	def initialize texture, x, y, solid = true
		@sprite = Sprite.new(texture)
		@x = x * $block_size
		@y = y * $block_size
		@sprite.set_position(@x, @y)
		@solid = solid
	end

	attr_reader :sprite, :solid, :x, :y
end

# 10 x 7 block window
bw = 10
bh = 8
w = bw * $block_size * $zoom
h = bh * $block_size * $zoom
window = RenderWindow.new([w, h], "Pform Ruby Test", Style::Titlebar)
# 4x zoom
zoom_view = View.new([bw * $block_size / 2, bh * $block_size / 2], [w / $zoom, h / $zoom])
window.set_view zoom_view

font = Font.new
font.load_from_file("/usr/share/fonts/TTF/VeraMono.ttf")

# for displaying fps
fps_text = Text.new("", font, 12)
fps_text.set_color(Color::Black)

# load textures
dude = Texture.new
$brick = Texture.new
dude.load_from_file("char.png")
$brick.load_from_file("block.png")

# create level
level = Array.new(bh) { |i| Array.new(bw) { |j| nil } }

def block lvl, x, y
	lvl[y][x] = Block.new($brick, x, y)
end

block(level, 0, 0)
for i in (0...bw)
	block(level, i, bh - 1)
end
block(level, 0, bh / 2 + 1)
block(level, bw - 1, bh / 2 + 1)

player = Player.new(level, dude, bw - 2, bh / 2 + 1)

green = Color.new(0, 255, 0, 127)
debug = RectangleShape.new([$block_size, $block_size])
debug.set_fill_color(green)

gray = Color.new(80, 80, 80)

clock = Clock.new

# game loop
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
				player.dir = -1
			when Keyboard::Right
				player.dir = 1
			when Keyboard::Up
				player.jump
			end

		when Event::KeyReleased
			case event.key.code
			when Keyboard::Left
				player.dir = 0
			when Keyboard::Right
				player.dir = 0
			end
		end
	end

	time = clock.time.as_seconds
	clock.restart

	player.step(time)

	fps = 1 / time
	fps_text.set_string fps.to_i.to_s

	window.clear(gray)

	level.each { |row| row.each { |block| window.draw(block.sprite) if block } }
	window.draw(player.sprite)

	for i in ((player.miny)..(player.maxy))
		for j in ((player.minx)..(player.maxx))
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
