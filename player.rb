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

		# if moving and no key pressed, apply breaks
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

		if @vel.all? { |v| v == 0 }
			rounded = @pos.map { |c| c.round }
			if rounded != @pos
				@diff = @pos.map.with_index { |c, i| rounded[i] - c }
			end
		end

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

		# TODO the final movement can align, causing a fall
		if @standing and not (@level[@limit[-1][0]][@limit[1][1] + 1] or @level[@limit[1][0]][@limit[1][1] + 1])
			@standing = false
		end

	end
end
