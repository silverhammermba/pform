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
	attr_reader :sprite, :limit, :cross

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

	# currently, calculate array of horizontal/vertical grid crossings from @pos to pos
	def move_to pos, move
		# convert pos to @diff (this won't be necessary when doing it for real)
		@diff = pos.map.with_index { |p, i| p - @pos[i] }
		# this multiplier accounts for @diffs being negative
		mult = @diff.map(&:sign)
		# where we store the crossings
		@cross = [[], []]
		# for x and y
		(0..1).each do |i|
			# if there is a diff
			if mult[i] != 0
				# start from the limit corner in the direction of the movement
				lower = (@limit[mult[i]][i] + (@diff[i] > 0 ? 1 : 0)) * $block_size
				# end at that corner's final position
				upper = @pos[i] + (@diff[i] > 0 ? 1 : 0) * $block_size + @diff[i]
				# and step by the block size
				((lower * mult[i])...(upper * mult[i])).step($block_size) do |j|
					crs = []
					# the crossing coordinate we get by stepping
					crs[i] = j * mult[i]
					# the other coordinate we have to calculate
					# TODO this has potential for rounding errors
					crs[1 - i] = (@diff[1 - i] * j * mult[i]) / @diff[i].to_f + (@pos[1 - i] + (@diff[1 - i] > 0 ? 1 : 0) * $block_size) - (@diff[1 - i] * (@pos[i] + (@diff[i] > 0 ? 1 : 0) * $block_size)) / @diff[i].to_f
					# TODO 1/10th of pixel seems good enough...
					@cross[i] << crs.map { |c| c.round(1) }
				end
			end
		end

		return unless move

		until @cross.all?(&:empty?)
			# determine what next crossing is
			type = nil
			if @cross[1].empty?
				type = :x
				point = @cross[0].shift
			elsif @cross[0].empty?
				type = :y
				point = @cross[1].shift
			elsif @cross[0][0] == @cross[1][0]
				type = :corner
				point = @cross[0].shift
				@cross[1].shift
			elsif @cross[0][0][0] * mult[0] < @cross[1][0][0] * mult[0] or @cross[0][0][1] * mult[1] < @cross[1][0][1] * mult[1]
				type = :x
				point = @cross[0].shift
			else
				type = :y
				point = @cross[1].shift
			end

			# move to next crossing and subtract from diff
			@diff.map!.with_index { |d, i| d - point[i] + @pos[i] }
			@pos = point
		end

		@pos.map!.with_index { |c, i| c + @diff[i] }
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

player = Player.new(level, dude, [8, 6])

green = Color.new(0, 255, 0, 127)
debug = RectangleShape.new([$block_size, $block_size])
debug.set_fill_color(green)

red = Color.new(255, 0, 0, 127)
blue = Color.new(0, 0, 255, 127)
msize = 2
mousedot = CircleShape.new(msize)
mousedot.set_origin([msize, msize])
mousedot.set_fill_color(red)

gray = Color.new(80, 80, 80)

clock = Clock.new

# game loop
while window.open?
	click = false

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
				player = Player.new(level, dude, 8, 6)
			end
		when Event::KeyReleased
			case event.key.code
			when Keyboard::Left
				player.dir = 0
			when Keyboard::Right
				player.dir = 0
			end
		when Event::MouseButtonReleased
			click = event.mouse_button.button == 0
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

	m = Mouse.get_position(window)
	m = [m.x.to_f / $zoom, m.y.to_f / $zoom]

	debug.set_position(m)
	window.draw(debug)
	player.move_to(m, click)

	mousedot.set_fill_color(red)
	player.cross[0].each do |d|
		mousedot.set_position(d)
		window.draw(mousedot)
	end
	mousedot.set_fill_color(blue)
	player.cross[1].each do |d|
		mousedot.set_position(d)
		window.draw(mousedot)
	end


	# draw stuff unzoomed
	window.set_view(window.get_default_view)

	window.draw(fps_text)
	window.set_view(zoom_view)

	window.display
end
