#!/usr/bin/env ruby

require 'sfml/system'
require 'sfml/window'
require 'sfml/graphics'
require 'player'

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

# 10 x 8 block window
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
