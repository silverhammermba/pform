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

	def clamp lb, ub
		if self < lb
			lb
		elsif self > ub
			ub
		else
			self
		end
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
	def initialize level, texture, pos
		@level = level
		@sprite = Sprite.new(texture)

		# position
		@pos = pos.map { |coord| coord * $block_size }
		# distance to move
		@diff = [0, 0]
		# current velocity
		@vel = [0, 0]
		# acceleration from movement and gravity
		@acc = [300, 800]
		# break speed when idle
		@break = @acc[0] * 2
		# terminal velocities
		@trm = [75, 100]
		# x direction from keyboard
		@dir = 0
		# if the player is on the ground
		@standing = false

		find_relevant_region
		@sprite.set_position(*@pos.map(&:floor))
	end

	attr_accessor :dir
	attr_reader :sprite, :limit

	def jump
		if @standing
			# align pixels to allow jumping through narrow gaps
			@pos[0] = @pos[0].round
			@vel[1] = -250
			@standing = false
		end
	end

	# find blocks overlapped by the player
	def find_relevant_region
		# this weird array lets us access the min coords as @limit[-1] and max as @limit[1]
		@limit = [nil, @pos.map { |coord| (coord / $block_size).ceil }, @pos.map { |coord| (coord / $block_size).floor }]
	end

	# move the player
	def step seconds
		# if moving and no key pressed, break
		if @vel[0] != 0 and @dir * @vel[0] <= 0
			@vel[0] = reduce(@vel[0], @break * seconds)
		else # @vel[0] == 0 or @dir * @vel[0] > 0
			@vel[0] += @dir * @acc[0] * seconds
		end
		# fall faster if not standing
		if not @standing
			@vel[1] += @acc[1] * seconds
		end
		# clamp velocity
		@vel.map!.with_index { |v, i| v.clamp(-@trm[i], @trm[i]) }

		@diff = @vel.map { |v| v * seconds }
		if @diff.any? { |d| d != 0 }
			#resolve_movement
			find_relevant_region
			@sprite.set_position(*@pos)
		end
	end

	def stand!
		@standing = true
		@vel[1] = 0
	end

	def fall!
		@standing = false
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
brick = Texture.new
dude.load_from_file("char.png")
brick.load_from_file("block.png")

# create level
level = Array.new(bw) { |i| Array.new(bh) { |j| nil } }

blocks = [
	[0, 1], [0, 5], [1, 5], [1, 3], [1, 6], [0, 6], [7, 5], [7, 6], [9, 5], [9, 6]
]
for i in (0...bw)
	blocks << [i, bh - 1]
end

blocks.each { |x, y| level[x][y] = Block.new(brick, x, y) }

player = Player.new(level, dude, [7, 4])

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

	((player.limit[-1][1])..(player.limit[1][1])).each do |i|
		((player.limit[-1][0])..(player.limit[1][0])).each do |j|
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
