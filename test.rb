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
		@trm = [75, 300]
		# x direction from keyboard
		@dir = 0
		# if the player is on the ground
		@standing = false

		# whether the player is holding a key in this direction
		@keypress = [false, false]

		find_relevant_region
		@sprite.set_position(*@pos.map(&:floor))
	end

	attr_accessor :dir, :keypress
	attr_reader :sprite, :limit, :cross, :vel

	def jump
		if @standing
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
		# TODO quickly turning around is TOO SLOW

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

		# TODO need to round to pixel coords if no vel + no keypress

		@diff = @vel.map { |v| v * seconds }
		if @diff.any? { |d| d != 0 }
			resolve_movement(true)
			@sprite.set_position(*@pos.map(&:round))
		end
	end

	def stand!
		@standing = true
		@vel[1] = 0
	end

	def fall!
		@standing = false
	end

	# set diff by absolute position and resolve
	def move_to pos, move
		# convert pos to @diff
		@diff = pos.map.with_index { |p, i| p - @pos[i] }
		resolve_movement move
	end

	# resolve a diff
	def resolve_movement move
		# shift to get the leading corner
		corner = @diff.map { |d| d > 0 ? 1 : 0 }
		# this multiplier accounts for @diffs being negative
		mult = @diff.map(&:sign)
		# where we store the crossings
		@cross = [[], []]
		# for x and y
		(0..1).each do |i|
			# if there is a diff
			if mult[i] != 0
				# start from the limit corner in the direction of the movement
				lower = (@limit[mult[i]][i] + corner[i]) * $block_size
				# end at that corner's final position
				upper = @pos[i] + corner[i] * $block_size + @diff[i]
				# and step by the block size
				((lower * mult[i])...(upper * mult[i])).step($block_size) do |j|
					crs = []
					# the crossing coordinate we get by stepping
					crs[i] = j * mult[i]
					# the other coordinate we have to calculate
					# TODO this has potential for rounding errors
					crs[1 - i] = (@diff[1 - i] * j * mult[i]) / @diff[i].to_f + (@pos[1 - i] + corner[1 - i] * $block_size) - (@diff[1 - i] * (@pos[i] + corner[i] * $block_size)) / @diff[i].to_f
					# TODO but rounding here lets you teleport into corners
					@cross[i] << crs#.map { |c| c.round(1) }
				end
			end
		end

		# TODO for debugging
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

			point.map!.with_index { |c, i| c - corner[i] * $block_size }

			# move to next crossing and subtract from diff
			@diff[0] -= point[0] - @pos[0]
			@diff[1] -= point[1] - @pos[1]
			@pos = point

			find_relevant_region

			collision = false
			nxt = []
			(0..1).each do |i|
				nxt[i] = @limit[mult[i]][i] + mult[i] if mult[i] != 0
			end
			# crossing X
			if type == :x or type == :corner
				# if standing and nothing underneath, fall
				if @standing and not @level[@limit[1][0]][@limit[1][1] + 1]
					@standing = false
					@pos[1] += 1 # TODO bit of hack to make sure gaps can't be skipped
					find_relevant_region
					collision = true
				end
				# check for walls
				if @level[nxt[0]][@limit[-1][1]] or @level[nxt[0]][@limit[1][1]]
					@diff[0] = 0
					collision = true
				end
			end
			# crossing Y
			if type == :y or type == :corner
				if @level[@limit[-1][0]][nxt[1]] or @level[@limit[1][0]][nxt[1]]
					@diff[1] = 0
					# if moving downward, you are now standing, else you hit your head
					if mult[1] > 0
						stand!
					else
						@vel[1] = 0
					end
					collision = true
				end
			end
			if type == :corner and @diff.all? { |c| c != 0 }
				if @level[nxt[0]][nxt[1]]
					@diff[1] = 0 # TODO arbitrary placeholder
					collision = true
				end
			end

			if collision
				return resolve_movement(move)
			end
		end

		@pos.map!.with_index { |c, i| c + @diff[i] }
		find_relevant_region
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

# keypress text
kpt = Text.new("", font, 12)
kpt.set_color(Color::Black)
kpt.set_position(10, 20)

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
msize = 1
mousedot = CircleShape.new(msize)
mousedot.set_origin([msize, msize])
mousedot.set_fill_color(red)

gray = Color.new(80, 80, 80)

clock = Clock.new
overlap = false

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
			when Keyboard::X
				player.keypress[0] = !player.keypress[0]
			when Keyboard::Y
				player.keypress[1] = !player.keypress[1]
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

	kpt.set_string("#{player.keypress.inspect} #{player.vel.inspect}")

	window.clear(gray)

	level.each { |row| row.each { |block| window.draw(block.sprite) if block } }
	window.draw(player.sprite)

	thisoverlap = false
	((player.limit[-1][0])..(player.limit[1][0])).each do |i|
		((player.limit[-1][1])..(player.limit[1][1])).each do |j|
			if level[i][j]
				thisoverlap = true
				if not overlap
					overlap = true
					STDERR.puts "Overlapped level at #{i},#{j}"
					sleep(5)
				end
			end
			debug.set_position(i * $block_size, j * $block_size)
			window.draw(debug)
		end
	end
	overlap = false unless thisoverlap

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
	window.draw(kpt)
	window.set_view(zoom_view)

	window.display
end
