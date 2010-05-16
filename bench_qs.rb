#!/usr/local/bin/ruby

# Benchmark for enum1
[100, 1000, 10000, 100000, 1000_000, 10_000_000, 100_000_000].each do |size|
# For each number of thread launch 20 times the program
# and keep the least time.
(0..6).each do |n|	

	min_time = 10000000 ;

	20.times {
		out = `./quick_sort #{size} #{n}` 
		out_split = out.split " " ;
		if(out_split[1].to_f < min_time)
			min_time = out_split[1].to_f;
		end
	}

	puts "#{size}  #{n}  #{min_time}" ;
end
end
