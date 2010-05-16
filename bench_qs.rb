#!/usr/local/bin/ruby

# Benchmark for enum1
[100, 1000, 10000, 100000, 1000_000, 10_000_000, 100_000_000].each do |size|
# For each number of thread launch 20 times the program
# and keep the least time.
(0..6).each do |n|	

	min_time = 10000000 ;

<<<<<<< HEAD:bench_qs.rb
	20.times {
		out = `./quick_sort #{size} #{n}` 
=======
	10.times {
		out = `enum1 #{n}` 
>>>>>>> 6383e27601a052876c4ec12a4dde9350f8fb1c68:labPthreads/bench_enum1.rb
		out_split = out.split " " ;
		if(out_split[1].to_f < min_time)
			min_time = out_split[1].to_f;
		end
	}

	puts "#{size}  #{n}  #{min_time}" ;
end
end
