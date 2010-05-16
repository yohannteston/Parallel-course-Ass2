#!/usr/local/bin/ruby

# Benchmark for enum1
# For each number of thread launch 20 times the program
# and keep the least time.
[1,4,8,12,16,24,32,48,64].each do |n|

	min_time = 10000000 ;
	min_thread = 0;

	20.times {
		out = `enum1 #{n}` 
		out_split = out.split " " ;
		if(out_split[1].to_f < min_time)
			min_time = out_split[1].to_f;
			min_thread = out_split[0];
		end
	}

	puts "#{min_thread}  #{min_time}" ;
end
