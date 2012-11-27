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
		@accel = 300
		# break speed when idle
		@break = @accel * 2
		# top speed
		@max = 75
		# acceleration from gravity
		@gravity = 800
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
			@x = @x.round # to allow jumping through narrow gaps
			@vy = -250
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
			@sprite.set_position(@x, @y)
		end
	end

	def stand!
		@standing = true
		@vy = 0
	end

	def fall!
		@standing = false
	end

	def align_x!
		@x = (@x / $block_size).round * $block_size
	end

	def x_aligned?
		@x % $block_size == 0
	end

	def align_y!
		@y = (@y / $block_size).round * $block_size
	end

	def y_aligned?
		@y % $block_size == 0
	end

	def resolve_movement
		if @dx != 0 and @dy == 0
			minmax = (@dx > 0 ? @maxx : @minx)
			mod = (@dx > 0 ? 1 : -1)
			if x_aligned?
				# fall if shouldn't be standing
				if @standing and @level[@maxy + 1][@minx].nil?
					fall!
					# TODO fall a little?
					return
				else
					unless @level[@miny][minmax + mod] or @level[@maxy][minmax + mod]
						@x += @dx
						return
					end
				end
			else
				if (@x + @dx) * @dx > minmax * $block_size * @dx
					x = @x
					@x = minmax * $block_size
					@dx -= @x - x
					resolve_movement
				else
					@x += @dx
				end
				return
			end
		elsif @dx == 0 and @dy != 0
			minmax = (@dy > 0 ? @maxy : @miny)
			mod = (@dy > 0 ? 1 : -1)
			if y_aligned?
				# fall if shouldn't be standing
				if not @standing and @level[@maxy + 1][@minx]
					stand!
					return
				else
					unless @level[minmax + mod][@minx] or @level[minmax + mod][@maxx]
						@y += @dy
						return
					end
				end
			else
				if (@y + @dy) * @dy > minmax * $block_size * @dy
					y = @y
					@y = minmax * $block_size
					@dy -= @y - y
					resolve_movement
				else
					@y += @dy
				end
				return
			end
		else # @dx != 0 and @dy != 0
			# TODO FUCKING SHIT
		end
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

block(level, 0, 1)
for i in (0...bw)
	block(level, i, bh - 1)
end
block(level, 0, 5)
block(level, 1, 5)
block(level, 1, 3)
block(level, 1, 6)
block(level, 0, 6)
block(level, 7, 5)
block(level, 7, 6)
block(level, 9, 5)
block(level, 9, 6)

player = Player.new(level, dude, 7, 4)

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
			when Keyboard::R
				player = Player.new(level, dude, 7, 4)
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
