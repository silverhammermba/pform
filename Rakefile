require 'rake/clean'

$cc = 'g++ -std=c++0x -Wall -Wextra -ggdb'
$bin = 'launch'

task :default => $bin

source = FileList['*.cpp']
objects = source.map { |f| f.ext(?o) }

source.each do |f|
	file f.ext(?o) => [f, 'game.hpp', 'pform.hpp'] do
		sh "#$cc #{f} -c -o #{f.ext(?o)}"
	end
end

file $bin => objects do
	sh "#$cc #{objects} -o #$bin -lyaml-cpp -lsfml-graphics -lsfml-window -lsfml-system"
end

CLEAN.include('*.o', $bin)
